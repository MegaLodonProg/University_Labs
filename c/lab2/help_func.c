#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void free_arr(Person **arr){
    if (*arr == NULL) return;
    free(*arr);
	*arr = NULL;
}


int compare_persons(const void *a, const void *b) {
    const Person *person_a = (const Person *)a;
    const Person *person_b = (const Person *)b;

    if (person_a->ta < person_b->ta) return -1;
    if (person_a->ta > person_b->ta) return 1;

    return 0;
}

void time_sort_persons(Person *array, int size) {
    qsort(array, size, sizeof(Person), compare_persons);
}
