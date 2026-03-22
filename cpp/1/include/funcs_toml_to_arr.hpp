#ifndef LAB1_FUNCS_TOML_TO_ARR_H
#define LAB1_FUNCS_TOML_TO_ARR_H

#include <iostream>
#include <vector>
using std::string;

// экземпляр класса std::string
std::vector<int> convert_toml_to_arr(const string& toml_str, string& name);

// нуль-терминированная строка
std::vector<int> convert_toml_to_arr(const char *toml_str, string& name);

// с массивом символов и их количеством - точно знаем длину выходящей строки
std::vector<int> convert_toml_to_arr(const char *toml_str, size_t size, string& name);


#endif //LAB1_FUNCS_TOML_TO_ARR_H