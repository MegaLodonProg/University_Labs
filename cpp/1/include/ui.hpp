#ifndef LAB1_MENU_H
#define LAB1_MENU_H

#include <vector>
#include <iostream>

void clean_buffer();
void table_info();
std::string get_name();
std::string get_toml_str();
std::vector<int> get_array();
void print_vector(const std::vector<int>&, const std::string&);
void menu();


#endif //LAB1_MENU_H