#ifndef PROJECT_C_IO_H
#define PROJECT_C_IO_H

#include <stdio.h>

ERR correct_input(int *);
ERR correct_int(unsigned int *);
ERR file_read(const char *, Table *);
char *readline_file(FILE *);

ERR menu(Table *);

#endif //PROJECT_C_IO_H
