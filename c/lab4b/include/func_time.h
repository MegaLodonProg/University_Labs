#ifndef FUNC_TIME_H
#define FUNC_TIME_H

#include "../include/Tree.h"

void func_time();

u_int generate_random_key();
char* generate_info();
void prefill_tree(B_Tree *root, int num_elements);

double measure_insert(B_Tree *tree, int num_elements);
double measure_search(B_Tree *tree, int num_calls);
double measure_delete(B_Tree *tree, int num_calls);
double measure_traversal(B_Tree *tree);
double measure_special_search(B_Tree *tree, int num_calls);

void print_menu();

#endif // FUNC_TIME_H