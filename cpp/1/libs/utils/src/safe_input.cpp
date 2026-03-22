#include "safe_input.hpp"

namespace utils {
    string safe_get_str(const string& prompt) {
        if (!prompt.empty()) {
            std::cout << prompt;
        }

        string str;
        // если ввод не удался (не std::cin.good())
        if (!(std::getline(std::cin, str))) {
            // отлавливаем eof (поток достиг конца файла)
            if (std::cin.eof()) {
                throw std::runtime_error("EOF");
            }
            // произошла серьёзная ошибка (например, закончилась память)
            if (std::cin.bad()) {
                throw std::runtime_error("Fatal error");
            }
            // прошла неудачная операция ввода (например, неправильный формат ввода, не тот тип данных хз)
            if (std::cin.fail()) {
                std::cin.clear(); // сбрасывает флаги ошибок в потоке, для возможности дальнейшего ввода
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Input stream is faulty");
            }
        }

        // принимает два необязательных параметра и игнорирует символы из буфера ввода
        // .ignore(max кол-во игнорируемых символов=1, stop символ игнорирования='\n')
        // используем getline - не надо очищать буфер
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return str;
    }

}