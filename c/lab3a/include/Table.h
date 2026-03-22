#ifndef TABLE_H
#define TABLE_H

typedef enum {
    ERR_OK = 0,
    ERR_EOF,
    ERR_MEM,
    ERR_EXIT,
    ERR_DUPLICATE,
    ERR_EMPTY,
    ERR_NOT_FOUND,
    ERR_OPEN,
    ERR_FORMAT,
}ERR;

typedef struct KeySpace{
    char *key;
    unsigned int *info;
    struct KeySpace *next;
}KeySpace;

// ПРОСМАТРИВАЕМАЯ таблица на основе списка -
// Чтобы найти элемент, нужно пройтись по списку от начала до конца, пока не будет найден нужный элемент
typedef struct Table {
    KeySpace *ks; // указатель на голову списка
} Table;

KeySpace *search_by_key(const Table *table, const char* key); // поиск элемента по ключу + проверка на уникальность ключа
ERR table_range_keys(Table *table, Table *new_table, const char* key1, const char* key2); // ключ лежит между двум строками, лексиграфическое сравнение

ERR insert(Table *table, const char* key, unsigned int *info);
ERR delete(Table *table, const char* key);

void print_table(Table *table);
void free_table(Table *table);

#endif //TABLE_H
