#ifndef C_FUNC_H
#define C_FUNC_H


typedef struct Line {
    int n;
    int *ptr;
}Line;

typedef struct Matrix{
    int lines;
    Line *matrix;
}Matrix;

int correct_input(int *, int);
void free_Matrix(Matrix *);
int input_Matrix(Matrix *);
void output_Matrix(Matrix *);
int elem_filter(Matrix *, Matrix *);


#endif //C_FUNC_H
