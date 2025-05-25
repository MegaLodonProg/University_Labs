#include <stdlib.h>
#include "../include/Table.h"
#include "../include/menu.h"

int main(){
    Table *table = calloc(1, sizeof(Table));
    if (!table) return 1;

    table->ks = calloc(M_SIZE, sizeof(KeySpace));
    if (!table->ks) {
        free(table);
        return 1;
    }

    table->m_size = M_SIZE;

    ERR result;
    do {
        result = menu(&table);
    } while (result == ERR_OK);

    free_table(table);
    free(table);
}