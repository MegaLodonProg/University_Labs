#include <sstream>
#include "funcs_toml_to_arr.hpp"
#include "exception.hpp"

std::vector<int> convert_toml_to_arr(const string& toml_str, string& name) {
    validate_input(toml_str);
    std::vector<int> result;

    // позволяет работать со строкой как с потоком (чтобы использовать getline)
    std::stringstream toml_stream(toml_str);
    std::string line;

    std::getline(toml_stream, line);
    // извлекает первый и последний символы строки
    if (line.front() == '[' && line.back() == ']') {
        // извлекает подстроку (как срез в питоне)
        name = line.substr(1, line.size() - 2);
    }
    // getline() - читает данные из входного потока
    // getline(входной поток, строка для сохранения токена, default sep = '\n')
    while (std::getline(toml_stream, line)) {
        const size_t pos = line.find('=');
        //  find возвращает std::string::npos если не нашла искомое;
        int int_elem = stoi(line.substr(pos + 1));
        result.push_back(int_elem);
    }
    return result;
}

std::vector<int> convert_toml_to_arr(const char *toml_str, string& name) {
    // нужно будет менять объект - Нельзя передавать временный объект в неконстантную ссылку (сразу оберткку нельзя)
    return convert_toml_to_arr(std::string(toml_str), name);
}

std::vector<int> convert_toml_to_arr(const char *toml_str, size_t size, string& name) {
    return convert_toml_to_arr(std::string(toml_str), name);
}
