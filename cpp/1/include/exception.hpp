#ifndef LAB1_EXCEPTION_H
#define LAB1_EXCEPTION_H

#include <iostream>
#include <vector>

using std::string;

void validate_input(const std::vector<int>& arr, const string& name);
void validate_input(const int* arr, size_t size, const char* name);
void validate_input(const string& str);

bool safe_string_to_int(const std::string& input, int& output);
int choose_num_option(const int first_opt, const int last_opt);

#endif //LAB1_EXCEPTION_H