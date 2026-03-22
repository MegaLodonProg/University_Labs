#ifndef LAB4a_IO_H
#define LAB4a_IO_H

#include "Tree.h"

ERR correct_command(int *);
ERR correct_int(int *);
ERR correct_uint(unsigned int *num);
char *readline_file(FILE * file);

#endif //LAB4a_IO_H
