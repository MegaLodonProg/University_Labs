#ifndef PROJECT_C_IO_H
#define PROJECT_C_IO_H

#include "Table.h"

ERR correct_command(int *);
ERR correct_uint(unsigned int *);
ERR get_values_arr(const KeySpace *, u_int **);

void print_values_arr(const u_int *, int);

#endif //PROJECT_C_IO_H
