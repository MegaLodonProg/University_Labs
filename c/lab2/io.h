#ifndef PROJECT_C_IO_H
#define PROJECT_C_IO_H

#include <stdio.h>
#include "queue.h"
#include <stdlib.h>
#include <string.h>

int correct_input(int *);
int correct_line(Person *);
int person_input(Person **, int *);
int create_queue(Queue ***, Person *, const int, const int);



#endif //PROJECT_C_IO_H
