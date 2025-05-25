#include <stdio.h>
#include <stdlib.h>
#include "../include/Table.h"
#include "readline/readline.h"
#include <limits.h>
#include <errno.h>

#define DELIM " "

ERR correct_input(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%d", num)) == 1 && *num >= 0 && *num <= 6){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return ERR_EOF;
        }
        printf("Command not found! Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return ERR_OK;
}

ERR correct_int(unsigned int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%u", num)) == 1){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return ERR_EOF;
        }
        printf("Invalid input! Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return ERR_OK;
}

char *readline_file(FILE * file){
    char buf[81] = {0};
    char *res = NULL;
    int len = 0, n = 0;

    do {
        n = fscanf(file, "%80[^\n]", buf);
        if (n < 0){
            if (!res){
                return NULL;
            }
        }
        else if (n > 0) {
            int chunk_len = strlen(buf);
            int str_len = len + chunk_len;
            res = realloc(res, str_len + 1);
            strncpy(res + len, buf, chunk_len);
            len = str_len;
        }
        else {
            // сразу встретили \n
            fscanf(file, "%*c");
        }
    } while (n > 0);

    if (len > 0){
        res[len] = '\0';
    }
    else {
        res = calloc(1, sizeof(char));
    }
    return res;
}

ERR file_read(const char *filename, Table *table) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Invalid read FILE");
        return ERR_OPEN;
    }

    char *input = NULL;
    while ((input = readline_file(file)) != NULL) {

        char *key = strtok(input, DELIM);
        char *value_str = strtok(NULL, DELIM);

        if (key == NULL || value_str == NULL) {
            free(input);
            fclose(file);
            return ERR_FORMAT;
        }

        // Преобразуем вторую часть в unsigned int
        char *endptr;
        errno = 0; // Хранит код последней произошедшей ошибки
        // value_str - указатель на преобразуемую строку
        // endptr - указатель на переменную, куда будет записан адрес первого символа, который не был преобразован
        unsigned long value = strtoul(value_str, &endptr, 10);

        // Проверяем ошибки преобразования
        if ((*endptr != '\0' && !isspace(*endptr)) || endptr == value_str || errno == ERANGE) {
            free(input);
            fclose(file);
            return ERR_FORMAT;
        }

        // проверка на выход из диапазона unsigned int
        if (value > UINT_MAX) {
            free(input);
            fclose(file);
            return ERR_FORMAT;
        }

        char *key_copy = strdup(key);
        if (key_copy == NULL) {
            free(input);
            fclose(file);
            return ERR_MEM;
        }

        unsigned int *value_ptr = malloc(sizeof(unsigned int));
        if (value_ptr == NULL) {
            free(input);
            free(key_copy);
            fclose(file);
            return ERR_MEM;
        }
        *value_ptr = (unsigned int)value;

        insert(table, key_copy, value_ptr);
        free(input);
        free(key_copy);
    }

    fclose(file);
    return ERR_OK;
}