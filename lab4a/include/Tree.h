#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

// ANSI escape-коды для цветного вывода в терминале
#define RESET   "\033[0m"

#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

typedef enum {
    ERR_OK,
    ERR_EOF,
    ERR_MEM,
    ERR_EXIT,
    ERR_EMPTY,
    ERR_NOT_FOUND,
    ERR_OPEN,
    ERR_FORMAT,
} ERR;

typedef struct TreeNode{
    char *key;
    char *info;

    struct TreeNode *left;
    struct TreeNode *right;
    struct TreeNode *parent;

    struct TreeNode* prev;
}TreeNode;

// Important : Вставка
ERR dialog_insert(TreeNode**);
ERR insert(TreeNode**, char*, char*);
void update_threads(TreeNode*); // обновление прошивки дерева

ERR dialog_del_node(TreeNode**);
ERR del_node(TreeNode**, char*, int); // удаление элемента

TreeNode *dialog_search(const TreeNode*, ERR*);
TreeNode *search(const TreeNode*, char*, int); // поиск элемента по ключу

TreeNode *special_dialog_search(const TreeNode *);
TreeNode *special_search(const TreeNode*, const char*); // специальный поиск

TreeNode* find_min_node(const TreeNode* node); // поиск минимального элемента
TreeNode* find_max_node(const TreeNode* node); // поиск максимального элемента

void print_nodes_with_prefix(TreeNode* root, const char* prefix); // вывод ключей, начинающихся с заданной подстроки
void export_to_dot(TreeNode* node, FILE* f);
void visualize_tree(TreeNode* root, const char* filename);

//TreeNode** tree_import(const char* filename);
ERR tree_import(TreeNode** dest_tree, const char* filename);

void free_tree(TreeNode**); // освобождение памяти

void print_tree(const TreeNode *node, int level, int is_left);
void print_tree_vertical(TreeNode* root);
void print_vertical_node(TreeNode* node, int depth, char* prefix, int is_tail);

#endif // TREE_H