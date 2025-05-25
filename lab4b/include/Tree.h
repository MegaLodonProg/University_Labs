#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ANSI escape-коды для цветного вывода в терминале
#define RESET   "\033[0m"

#define GREEN   "\033[32m"
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

typedef unsigned int u_int;

typedef struct InfoList{
    char *info;
    int release;
    struct InfoList *next;
}InfoList;


typedef struct TreeNode{
    u_int keys_arr[3];
    struct TreeNode *node_arr[4];
    InfoList *info_arr[3];
    struct TreeNode *parent;
    int size;
}TreeNode;

typedef struct B_Tree{
    TreeNode *root;
}B_Tree;


B_Tree* create_tree();

void destroy_tree(B_Tree *tree);
static TreeNode *get_max_node(TreeNode *node);
static TreeNode *get_min_node(TreeNode *node);

TreeNode *d_search(const B_Tree *tree, int *key_pos, char **found_inf, ERR *error);
TreeNode* search_node(const B_Tree *tree, u_int key, int release, int *key_pos, char **found_inf);
ERR special_search_node(const B_Tree *tree, u_int key, int release, u_int *found_key, char **found_inf);
static int find_position(TreeNode *node, unsigned int key);

ERR d_insert(B_Tree *tree);
ERR insert_node(B_Tree *tree, u_int key, char *info);

// Удаление ключа из дерева
ERR d_delete(B_Tree *tree);
ERR delete_node(B_Tree *tree, u_int key);

void print_tree(const B_Tree *tree);

ERR tree_import(B_Tree *dest_tree, const char* filename);

TreeNode *create_node(TreeNode *parent);

void tree_split(TreeNode* parent, int child_pos);

ERR update_info_list(InfoList **head, char *info);
int find_insert_position(TreeNode* node, u_int key);
ERR insert_into_leaf(TreeNode* leaf, u_int key, char* info);

void export_node_to_dot(TreeNode* node, FILE* f);
void visualize_b_tree(B_Tree* tree, const char* filename);

static void free_info_list(InfoList *list);
void free_node(TreeNode *node);


static bool is_leaf(TreeNode *node);
static void remove_key_from_leaf(B_Tree *tree, TreeNode *leaf, int pos);
static void borrow_from_left(TreeNode *parent, int pos);
static void borrow_from_right(TreeNode *parent, int pos);
static void merge_with_left(B_Tree *tree, TreeNode *parent, int pos);
static void fix_child(B_Tree *tree, TreeNode *parent, int pos);
static void handle_delete_internal_node(B_Tree *tree, TreeNode *node, unsigned int key);
static InfoList* remove_oldest_info(InfoList **head);



#endif // TREE_H