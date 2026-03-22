#include <stdio.h>
#include <stdlib.h>
#include "func.h"


int correct_input(int *num, int flag)
{
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%d", num)) == 1 && (flag ? *num > 0 : 1)){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return 1;
        }
        printf("Invalid input. Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return 0;
}

int input_Matrix(Matrix *my_matrix)
{
    int m, current_n;
    printf("Enter the number of matrix rows: --> ");
    if (correct_input(&m, 1) == 1){
        return 1;
    };
    my_matrix->lines = m;
    my_matrix->matrix = calloc(m, sizeof(Line));

    if (my_matrix->matrix == NULL){
        fprintf(stderr, "Memory Error\n");
        return 1;
    }

    for (int i = 0; i < m; i++){
        printf("Enter the number of items in the current line: --> ");
        if (correct_input(&current_n, 1) == 1){
            free_Matrix(my_matrix);
            return 1;
        };
        my_matrix->matrix[i].n = current_n;
        my_matrix->matrix[i].ptr = calloc(current_n, sizeof(int));

        if (my_matrix->matrix[i].ptr == NULL){
            fprintf(stderr, "Memory Error\n");
            free_Matrix(my_matrix);
            return 1;
        }

        printf("Enter %d numbers, each on a new line.\n", current_n);
        for (int j = 0; j < current_n; j++) {
            printf("[%d]: ", j + 1);
            if (correct_input(&(my_matrix->matrix[i].ptr[j]), 0) == 1){
                free_Matrix(my_matrix);
                return 1;
            };
        }
    }
    return 0;
};

void output_Matrix(Matrix *my_matrix)
{
    for (int i = 0; i < my_matrix->lines; i++){
        for (int j = 0; j < my_matrix->matrix[i].n; j++){
            printf("%d\t", my_matrix->matrix[i].ptr[j]);
        }
        printf("\n");
    }
}
