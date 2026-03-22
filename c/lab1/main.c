#include <stdio.h>
#include "func.h"


int main()
{
    Matrix start_matrix = {0, NULL};
    int status = input_Matrix(&start_matrix);
    if (status){
        printf("\nExit or Errors in program\n");
        return 1;
    }
    Matrix new_matrix = {0, NULL};
    if (elem_filter(&start_matrix, &new_matrix) == 1) {
    	fprintf(stderr, "Memory Error\n");
    	return 1;
    }
    puts("***_Your start matrix_***");
    output_Matrix(&start_matrix);
    puts("\n***_Your new matrix_***");
    output_Matrix(&new_matrix);

    free_Matrix(&start_matrix);
    free_Matrix(&new_matrix);

    return 0;
}
