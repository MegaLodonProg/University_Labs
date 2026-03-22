#include <stdio.h>
#include <stdlib.h>
#include "func.h"

void free_Matrix(Matrix *my_matrix)
{
    if (my_matrix == NULL) return;

    for (int i = 0; i < my_matrix->lines; i++){
        if (my_matrix->matrix[i].ptr != NULL){
            free(my_matrix->matrix[i].ptr);
            my_matrix->matrix[i].ptr = NULL;
        }
    }

    if (my_matrix->matrix != NULL){
        free(my_matrix->matrix);
        my_matrix->matrix = NULL;
    }
}

int elem_filter(Matrix *my_matrix, Matrix *new_matrix)
{
    new_matrix->lines = my_matrix->lines;
    new_matrix->matrix = calloc(new_matrix->lines, sizeof(Line));
    if (new_matrix->matrix == NULL) {
        return 1;
    }

    for (int i = 0; i < my_matrix->lines; i++){
        new_matrix->matrix[i].ptr = malloc(my_matrix->matrix[i].n * sizeof(int));
        if (new_matrix->matrix[i].ptr == NULL){
            free_Matrix(my_matrix);
            return 1;
        };


        int index_positive = -1, index_negative = -1;
        int flag = 0, cur_len = 0;
        for (int j = 0; j < my_matrix->matrix[i].n; j++){
            if (my_matrix->matrix[i].ptr[j] > 0 && index_positive == -1) {
                index_positive = j;
                flag++;
            }
            else if (my_matrix->matrix[i].ptr[j] < 0 && index_negative == -1) {
                index_negative = j;
                flag++;
            }
            if (flag == 1){
                new_matrix->matrix[i].ptr[cur_len++] = my_matrix->matrix[i].ptr[j];
            }
            else if (flag == 2){
                new_matrix->matrix[i].ptr[cur_len++] = my_matrix->matrix[i].ptr[j];
                break;
            }
        }
        int *temp_ptr = realloc(new_matrix->matrix[i].ptr, cur_len * sizeof(int));
        if (temp_ptr == NULL && cur_len != 0) {
            free_Matrix(new_matrix);
            free_Matrix(my_matrix);
            return 1;
        }
        new_matrix->matrix[i].ptr = temp_ptr;
        new_matrix->matrix[i].n = cur_len;
    }
    return 0;
}
