#include <stdio.h>
#include <stdlib.h>
#include "../include/Table.h"
#include "../include/io.h"
#include "readline/readline.h"

ERR menu(Table *table){
    printf("____MENU____\n");
    printf("(1) Insert a new element into the table\n");
    printf("(2) Remove element from table\n");
    printf("(3) Search an element in a table\n");
    printf("(4) Print table\n");
    printf("(5) Read info from text FILE\n");
    printf("(6) Search for elements specified by a range of keys\n");
    printf("(0) Exit\n");

    int command;
    int st = correct_input(&command);
    if (st == ERR_EOF) return ERR_EOF;

    char *line;
    int status;
    switch (command) {
        case 0:
            return ERR_EXIT;
        case 1: {
            unsigned int *info = malloc(sizeof(unsigned int));  // Выделяем память для info
            if (info == NULL) {
                printf("Ошибка выделения памяти\n");
                return ERR_MEM;
            }

            printf("Value: ");
            status = correct_int(info);
            if (status == ERR_EOF) {
                free(info);
                info = NULL;
                return ERR_EOF;
            }
            line = readline("Key: ");
            if (line == NULL){
                free(info);
                info = NULL;
                return ERR_EOF;
            }
            int res = insert(table, line, info);
            if (res == ERR_DUPLICATE){
                printf("Key [%s] already into the table\n", line);
            }
            free(line);
            line = NULL;
            break;
        }
        case 2:
            line = readline("Key: ");
            if (line == NULL){
                return ERR_EOF;
            }
            int res = delete(table, line);
            if (res == ERR_EMPTY){
                printf("Table is empty!\n");
            }
            else if (res == ERR_NOT_FOUND) {
                printf("The entered key is missing from the table\n\n");
            }
            free(line);
            line = NULL;
            break;
        case 3:
            line = readline("Key: ");
            if (line == NULL){
                return ERR_EOF;
            }
            KeySpace *link = search_by_key(table, line);
            if (link != NULL){
                printf("Value [%u] found for key [%s]\n", *(link->info), link->key);
            } else printf("The entered key is missing from the table\n\n");
            free(line);
            line = NULL;
            break;
        case 4:
            print_table(table);
                break;
        case 5: {
            char *path = readline("Enter the relative path to the file:\n");
            if (path == NULL){
                return ERR_EOF;
            }
            ERR result = file_read(path, table);
            if (result == ERR_FORMAT) printf("Invalid string format, check the table!\n");
            else if (result == ERR_MEM) printf("Invalid memory!\n");
            free(path);
            path = NULL;
            break;
        }
        case 6: {
            Table *new_Table = calloc(1, sizeof(Table));
            if (new_Table == NULL) {
                return ERR_MEM;
            }
            char *key1 = readline("Start - Key: ");
            if (key1 == NULL){
                free_table(new_Table);
                free(new_Table);
                return ERR_EOF;
            }
            char *key2 = readline("End - Key: ");
            if (key2 == NULL){
                free(key1);
                free_table(new_Table);
                free(new_Table);
                return ERR_EOF;
            }
            table_range_keys(table, new_Table, key1, key2);
            printf("New table:\n");
            print_table(new_Table);
            free(key1);
            free(key2);
            free_table(new_Table);
            free(new_Table);
            key1 = key2 = NULL;
            new_Table = NULL;
            break;
        }
        default:
            printf("Command not found!\n");
            return ERR_EXIT;
    }
    return ERR_OK;
}