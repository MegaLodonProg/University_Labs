#include <cstring>
#include <ranges>
#include <vector>
#include <algorithm>
#include "funcs_arr_to_toml.hpp"

using std::string;
using std::to_string;

string convert_arr_to_toml(const std::vector<int>& arr, const string& name) {
    using namespace std::views;

    auto indices = iota(1u, arr.size() + 1);

    // Для каждого индекса создаем строку "index=value"
    auto lines = transform(indices, [&arr](size_t i) {
        return std::to_string(i) + "=" + std::to_string(arr[i-1]) + "\n";
    });

    std::string body;
    for (const auto& line : lines) {
        body += line;
    }

    return "[" + name + "]\n" + body;
}

const char* convert_arr_to_toml(const int* arr, const size_t size, const char* name) {
    // явное приведение массива в вектор (const int* arr -> vector). Встроенного нет!
    // vec(указатель на начало массива, указатель на первый элемент не входящий в массив);
    const std::vector<int> vec(arr, arr + size);

    // std::string имеет метод .c_str() который возвращает const char*
    // Компилятор автоматически вызывает .c_str() когда видит несоответствие типов
    const string res = convert_arr_to_toml(vec, name);

    char* toml_str = new char[res.size() + 1];

    // strcpy(куда копировать, указатель на копируемую строку)
    // strcpy(toml_str, res.c_str());
    std::copy_n(res.begin(), res.size(), toml_str);
    // Добавляем нулевой терминатор в конец строки
    toml_str[res.size()] = '\0';

    return toml_str;

}

const char* convert_arr_to_toml(const int* arr, const size_t size, const char* name, size_t& out_size) {
    const std::vector<int> vec(arr, arr + size);
    const string res = convert_arr_to_toml(vec, name);

    // Выделяем память ТОЛЬКО под данные, без лишнего байта для '\0'
    out_size = res.size();
    char* char_array = new char[out_size];

    // Копируем только данные, без \0
    // memcpy(char_array, res.c_str(), out_size);
    std::copy_n(res.begin(), res.size(), char_array);
    // Добавляем нулевой терминатор в конец строки

    return char_array;
}