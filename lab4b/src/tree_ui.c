#include "../include/io.h"
#include "readline/readline.h"


TreeNode *d_search(const B_Tree *tree, int *key_pos, char **found_inf, ERR *error){
    if (!tree || !tree->root){
        *error = ERR_EMPTY;
        return NULL;
    }

    u_int key;
    char *info;
    int release;

    printf("Key : ");
    if (correct_uint(&key) != ERR_OK) {
        *error = ERR_EXIT;
        return NULL;
    }


    printf("Release: ");
    if (correct_int(&release) == ERR_EOF){
        *error = ERR_EXIT;
        return NULL;
    }

    *error = ERR_OK;

    TreeNode *result = search_node(tree, key, release, key_pos, found_inf);
    if (!result) {
        *error = ERR_NOT_FOUND;
    }

    return result;
}

ERR d_insert(B_Tree *tree){
    if (!tree){
        return ERR_MEM;
    }

    u_int key;
    char *info;

    printf("Key : ");
    if (correct_uint(&key) != ERR_OK) {
        return ERR_EXIT;
    }

    info = readline("Info: ");
    if (!info) {
        return ERR_MEM;
    }

    ERR result = insert_node(tree, key, info);
    if (result != ERR_OK) {
        return ERR_MEM;
    }

    return result;
}

ERR d_delete(B_Tree *tree){
    if (!tree || !tree->root){
        return ERR_EMPTY;
    }

    u_int key;

    printf("Key : ");
    if (correct_uint(&key) != ERR_OK) {
        return ERR_EXIT;
    }

    ERR result = delete_node(tree, key);
    if (result != ERR_OK) {
        return ERR_NOT_FOUND;
    }

    return result;
}

