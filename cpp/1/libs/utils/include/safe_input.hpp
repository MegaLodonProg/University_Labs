#ifndef SAFE_INPUT_H
#define SAFE_INPUT_H

#include <iostream>
#include <limits>
#include <string>

using std::string;

namespace utils {

// Important : Аргумент по умолчанию ставить в прототипах! В .cpp нельзя повторно!
string safe_get_str(const string& prompt="");

// Important : Шаблонные функции должны быть полностью определены в заголовочных файлах!
template<typename T>
T safe_input(const string& prompt="") {
    if (!prompt.empty()) {
        std::cout << prompt;
    }

    T value;
    if (!(std::cin >> value)) {
        if (std::cin.eof()) {
            throw std::runtime_error("EOF");
        }
        if (std::cin.bad()) {
            throw std::runtime_error("Fatal error");
        }
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::runtime_error("Input stream is faulty");
        }
    }
    // принимает два необязательных параметра и игнорирует символы из буфера ввода
    // .ignore(max кол-во игнорируемых символов=1, stop символ игнорирования='\n')
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return value;
}

} // namespace utils


#endif //SAFE_INPUT_H