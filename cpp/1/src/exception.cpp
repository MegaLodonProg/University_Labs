#include <iostream>
#include <limits>
#include "exception.hpp"
#include "safe_input.hpp"

// Important : НИЗКОУРОВНЕВАЯ ФУНКЦИЯ - ТОЛЬКО бросает исключения
void validate_input(const std::vector<int>& arr, const string& name) {
    // arr.empty() - метод (для вектора), возвращает true если вектор не содержит элементов
    if (arr.empty()) {
        throw std::invalid_argument("Array cannot be empty!");
    }
    // name.empty() - метод (для строки), возвращает true если строка имеет длину 0
    if (name.empty()) {
        throw std::invalid_argument("Array name cannot be empty!");
    }
}

void validate_input(const int* arr, const size_t size, const char* name) {
    if (arr == nullptr) {
        throw std::invalid_argument("Array cannot be empty!");
    }
    if (name == nullptr) {
        throw std::invalid_argument("Array name cannot be empty!");
    }
    if (size < 1) {
        throw std::invalid_argument("Array size cannot less one!");
    }
}

void validate_input(const string& str) {
    if (str.empty()) {
        throw std::invalid_argument("Array name cannot be empty!");
    }
    if (str.find(' ') != string::npos) {
        throw std::invalid_argument("Name cannot contain spaces!");
    }
}


// Перенести функции в utils
int choose_num_option(const int first_opt, const int last_opt) {
    int option;

    while (true) {
        std::cout << "Choose option [" << first_opt << "-" << last_opt << "]: ";

        std::string input = utils::safe_input<std::string>();
        if (input.empty()) {
            std::cout << "Please ";
            continue;
        }
        if (safe_string_to_int(input, option)) {
            if (option >= first_opt && option <= last_opt) {
                return option;
            }
        }
        std::cerr << "Option not found. Try again!" << std::endl;
    }
}


bool safe_string_to_int(const std::string& input, int& output) {
    if (input.empty()) {
        return false;
    }

    try {
        size_t pos = 0;
        // stol() - или преобразует в long или выкидывает исклюсение, у stoi может быть UB
        // stol(преобразуемая строка, индекс указывающий на первый символ после числа, base)
        const long temp = std::stol(input, &pos);

        // Проверяем что вся строка обработана size - возвращает длину строки в байтах
        if (pos != input.size()) {
            return false;
        }

        // Проверяем диапазон int
        if (temp < std::numeric_limits<int>::min() || temp > std::numeric_limits<int>::max()) {
            return false;
            }

        // C++ стиль явного преобразования типов данных
        output = static_cast<int>(temp);
        return true;

    } catch (const std::exception& e) {
        return false;
    }
}