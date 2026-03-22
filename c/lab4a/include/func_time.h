#ifndef FUNC_TIME_H
#define FUNC_TIME_H

#include "../include/Tree.h"

void func_time();

char* generate_random_key();
char* generate_info();
void prefill_tree(TreeNode** root, int num_elements);

double measure_insert(TreeNode** root, int num_elements);
double measure_search(TreeNode* root, int num_calls);
double measure_delete(TreeNode** root, int num_calls);
double measure_traversal(TreeNode* root);
double measure_prefix_search(TreeNode* root, int num_calls);

void print_menu();

#endif // FUNC_TIME_H