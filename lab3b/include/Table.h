#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {EMPTY, BUSY, DELETED} BusyType;

typedef enum {
    ERR_OK,
    ERR_EOF,
    ERR_MEM,
    ERR_EXIT,
    ERR_EMPTY,
    ERR_NOT_FOUND,
    ERR_OPEN,
    ERR_FORMAT,
    ERR_OVERFLOW
} ERR;

// important : простое число (рекомендуется)
#define M_SIZE 13
typedef unsigned int u_int;

typedef struct Node {
    u_int release;
    u_int *info;
    struct Node *next;
} Node;

typedef struct KeySpace {
    BusyType busy;
    u_int key;
    Node *node;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    u_int m_size;
} Table;

u_int hash1(u_int key);
u_int hash2(u_int key);

KeySpace *dialog_tbl_search(const Table *table, ERR *error);
KeySpace *tbl_search_by_key(const Table *table, u_int key);

ERR dialog_tbl_insert(Table *table);
ERR tbl_insert(Table *table, u_int key, u_int info);

ERR dialog_tbl_delete(Table *table);
ERR tbl_delete(KeySpace **ks, u_int release);

ERR table_export_binary(const Table* table, const char* filename);
u_int count_busy_cells(const Table* table);

Table* table_import_binary(const char* filename);

void print_table(Table *table);
void free_table(Table *table);

#endif // TABLE_H