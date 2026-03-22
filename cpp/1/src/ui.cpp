#include <vector>
#include <limits>
#include <ranges>
#include "ui.hpp"
#include "exception.hpp"
#include "funcs_arr_to_toml.hpp"
#include "funcs_toml_to_arr.hpp"
#include "safe_input.hpp"


void clean_buffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void table_info() {
    std::cout << "===== ARRAY -> TOML ======\n";
    std::cout << "1 - std::string\n";
    std::cout << "2 - const char*\n";
    std::cout << "3 - const char* + size_t\n\n";

    std::cout << "===== TOML -> ARRAY =====\n";
    std::cout << "4 - std::string\n";
    std::cout << "5 - const char*\n";
    std::cout << "6 - const char* + size_t\n";

    std::cout << "7 - Exit" << std::endl;

}

std::string get_name() {
    while (true) {
        // не отлавливаем исключение - пропускаем дальше до menu
        std::string arr_name = utils::safe_get_str("Enter array name: ");

        try {
            validate_input(arr_name);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input: " << e.what() << std::endl;
            std::cerr << "Try again!" << std::endl;
            continue;
        }
        return arr_name; // Возвращается КОПИЯ строки
    }
}

void print_vector(const std::vector<int>& vec, const std::string& name) {
    using namespace std::views;

    auto string_values = vec | transform([](int x) {
        return std::to_string(x);
    });

    std::cout << name << " = [";

    bool first = true;
    for (const auto& str : string_values) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << str;
        first = false;
    }

    std::cout << "]" << std::endl;
}

std::string get_toml_str() {
    std::string toml_str;
    int expected_index = 1;
    std::cout << "Enter the TOML string format (end with empty line):" << std::endl;
    std::cout << "Format: [array_name] followed by 1=value, 2=value, etc." << std::endl;

    while (true) {
        std::cout << "> ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            if (std::cin.eof()) throw std::runtime_error("EOF");
            throw std::runtime_error("Input error");
        }

        if (line.empty()) {
            if (toml_str.empty()) {
                std::cout << "TOML cannot be empty. Fill it!" << std::endl;
                continue;
            }
            if (toml_str.find('=') == std::string::npos) {
                std::cout << "You entered only header. Input data!" << std::endl;
                continue;
            }
            break;
        }

        // Обработка заголовка
        if (toml_str.empty()) {
            try {
                validate_input(line);
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " << e.what() << std::endl;
                continue;
            }
            if (line.front() == '[' && line.back() == ']' && line.size() >= 3) {
                toml_str += line + "\n";
            } else {
                std::cout << "First line format: [array_name]" << std::endl;
            }
            continue;
        }

        // Обработка последующих строк
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cout << "Line must contain '=' symbol" << std::endl;
            continue;
        }

        std::string index_str = line.substr(0, pos);
        std::string value_str = line.substr(pos + 1);

        int index, value;
        if (!safe_string_to_int(index_str, index) || !safe_string_to_int(value_str, value)) {
            std::cout << "Index and value must be numbers" << std::endl;
            continue;
        }

        if (index != expected_index) {
            std::cout << "Expected index: " << expected_index << ", got: " << index << std::endl;
            continue;
        }

        toml_str += line + "\n";
        expected_index++;
    }

    return toml_str;
}


std::vector<int> get_array() {
    std::vector<int> array;

    std::cout << "Enter integer values one per line (end with empty line):" << std::endl;

    while (true) {
        std::string input = utils::safe_get_str("> ");

        if (input.empty() && !array.empty()) {
            break;
        }

        // elem виден ТОЛЬКО внутри if блока
        if (int elem; safe_string_to_int(input, elem)) {
            array.push_back(elem);
        }
        else {
            if (array.empty() && input.empty()) {
                std::cout << "Array cannot be empty!" << std::endl;
                continue;
            }
            std::cout << "Invalid input. Try again!" << std::endl;
        }
    }

    return array;
}

void menu() {

    while (true){
        try {
            table_info();
            const int user_option = choose_num_option(1, 7);

            switch (user_option) {
                case 1: {
                    const std::string name_array = get_name();
                    const std::vector<int> array = get_array();

                    const std::string result = convert_arr_to_toml(array, name_array);
                    std::cout << result << std::endl;

                    break;
                }
                case 2: {
                    const std::string name_array = get_name();
                    const std::vector<int> vec = get_array();

                    const char* result = convert_arr_to_toml(vec.data(), vec.size(), name_array.c_str());
                    std::cout << result << std::endl;

                    delete[] result;
                    break;
                }
                case 3: {
                    const std::string name_array = get_name();
                    const std::vector<int> vec = get_array();
                    size_t out_size = 0;

                    const char* result = convert_arr_to_toml(vec.data(), vec.size(), name_array.c_str(), out_size);
                    // Выводим как массив символов заданной длины, а не как строку - иначе ищет /0 и invalid read
                    std::cout.write(result, out_size);
                    std::cout << std::endl;

                    delete[] result;
                    break;
                }
                case 4: {
                    const std::string toml_str = get_toml_str();
                    std::string name_array;

                    const std::vector<int> vec = convert_toml_to_arr(toml_str, name_array);

                    print_vector(vec, name_array);
                    break;
                }
                case 5: {
                    const std::string toml_str = get_toml_str();
                    std::string name_array;

                    const std::vector<int> vec = convert_toml_to_arr(toml_str.c_str(), name_array);

                    print_vector(vec, name_array);
                    break;
                }
                case 6: {
                    const std::string toml_str = get_toml_str();
                    std::string name_array;

                    const std::vector<int> vec = convert_toml_to_arr(toml_str.c_str(), toml_str.size(), name_array);

                    print_vector(vec, name_array);
                    break;
                }
                case 7: {
                    return;
                }
                default:
                    std::cout << "Command not found. Try again!" << std::endl;
            }
            // Ловит ВСЕ исключения, унаследованные от std::exception (std::runtime_error,
            // std::invalid_argument, std::out_of_range,std::bad_alloc)
        }
        catch (const std::runtime_error& e) {
            if (e.what() == std::string("EOF")) {
                std::cout << "\nExit!" << std::endl;
                exit(0);
                return;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "❌ Error: " << e.what() << '\n';
            std::cout << "Please try again.\n" << std::endl;

            // Очистка состояния cin после ошибки
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
