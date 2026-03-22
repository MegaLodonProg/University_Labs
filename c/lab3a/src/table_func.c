#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Table.h"

KeySpace *search_by_key(const Table *table, const char* key){
    if (table == NULL) {
        return NULL;
    }

    KeySpace *cur = table->ks; // сохраняем голову списка неподвижной
    while (cur != NULL){
        if (strcmp(cur->key, key) == 0){
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}


ERR insert(Table *table, const char *key, unsigned int *info) {
    if (search_by_key(table, key) != NULL) {
        free(info);
        return ERR_DUPLICATE;
    }

    KeySpace *newLink = calloc(1, sizeof(KeySpace));
    if (newLink == NULL) {
        free(info);
        return ERR_MEM;
    }
    newLink->key = strdup(key); // копирование с выделением памяти в отличие от strncpy
    if (newLink->key == NULL){
        free(newLink);
        free(info);
        return ERR_MEM;
    }
    newLink->info = info;
    newLink->next = NULL;

    if (table->ks == NULL){
        table->ks = newLink;
    }
    else {
        newLink->next = table->ks;
        table->ks = newLink;
    }
    return ERR_OK;
}

ERR delete(Table *table, const char* key){
    if (table == NULL || table->ks == NULL){
        return ERR_EMPTY;
    }

    if (strcmp(table->ks->key, key) == 0){
        KeySpace *cur = table->ks;
        table->ks = table->ks->next;
        free(cur->key);
        free(cur->info);
        free(cur);
        return ERR_OK;
    }
    else {
        KeySpace *cur = table->ks;
        while (cur->next != NULL){
            if (strcmp(cur->next->key, key) == 0){
                KeySpace *temp = cur->next;
                cur->next = temp->next;
                free(temp->key);
                free(temp->info);
                free(temp);
                return ERR_OK;
            }
            cur = cur->next;
        }
    }
    return ERR_NOT_FOUND;
}


void print_table(Table *table){
    if (table == NULL || table->ks == NULL) {
        printf("Table is empty!\n");
        return;
    }

    KeySpace *cur = table->ks;
    while (cur != NULL){
        printf("key : %-8s value : %u\n", cur->key, *(cur->info)); // выравнивание по левому краю
        cur = cur->next;
    }
    free(cur);
    printf("\n");
}

void free_table(Table *table){
    if (table == NULL || table->ks == NULL) {
        return;
    }

    while (table->ks != NULL){
        KeySpace *cur = table->ks;
        table->ks = table->ks->next;
        free(cur->key);
        free(cur->info);
        free(cur);
    }
}

ERR table_range_keys(Table *table, Table *new_table, const char* key1, const char* key2){
    if (table == NULL || table->ks == NULL) {
        return ERR_MEM;
    }
    KeySpace *cur = table->ks;
    while (cur != NULL){
        if (strcmp(cur->key, key1) >= 0 && strcmp(cur->key, key2) <= 0){ // лексигарфически больше key1 и меньше key2 = между ними
            unsigned int *new_info = malloc(sizeof(unsigned int)); // иначе будет один указатель на две таблицы
            if (new_info == NULL) {
                return ERR_MEM;
            }
            *new_info = *(cur->info); // Копируем значение info
            int st = insert(new_table, cur->key, new_info);
            if (st == ERR_DUPLICATE || st == ERR_MEM){
                free(new_info);
                new_info = NULL;
            }
        }
        cur = cur->next;
    }
    return ERR_OK;
}