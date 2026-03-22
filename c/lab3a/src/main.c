#include <stdlib.h>
#include "../include/Table.h"
#include "../include/io.h"

int main(){
    Table *table = calloc(1, sizeof(Table));
    if (!table){
        return 1;
    }
    int result;
    do {
        result = menu(table);  // Сохраняем результат вызова menu
    } while (result == ERR_OK);

    free_table(table);
    free(table);

    return 0;
}
