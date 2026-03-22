#ifndef LAB1_FUNCS_ARR_TO_STR_H
#define LAB1_FUNCS_ARR_TO_STR_H

#include <iostream>
#include <vector>
using std::string;

// экземпляр класса std::string
string convert_arr_to_toml(const std::vector<int>& arr, const string& name);

// нуль-терминированная строка
const char* convert_arr_to_toml(const int* arr, size_t size, const char* name);

// с массивом символов и их количеством - точно знаем длину выходящей строки
const char* convert_arr_to_toml(const int* arr, size_t size, const char* name, size_t& out_size);


#endif //LAB1_FUNCS_ARR_TO_STR_H