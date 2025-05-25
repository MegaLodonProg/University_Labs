#include "../include/io.h"

u_int hash1(u_int key) {
    key = key * 37;
    return key;
}

u_int hash2(u_int key) {
    return 41 * key + 1;
}

KeySpace *tbl_search_by_key(const Table *table, u_int key) {
    if (table == NULL || table->ks == NULL) {
        return NULL;
    }

    const u_int size = table->m_size;
    u_int idx = hash1(key) % size;
    u_int step = hash2(key) % (size - 1);

    for (u_int i = 0; i < size; i++) {

        u_int cur = (idx + i * step) % size;
        KeySpace *cur_link = &(table->ks[cur]);

        if (cur_link->busy == EMPTY) return NULL;

        if (cur_link->busy == BUSY && cur_link->key == key && cur_link->node != NULL) {
            return cur_link;
        }
    }
    return NULL;
}

KeySpace *dialog_tbl_search(const Table *table, ERR *error) {
    u_int key;
    printf("Key: ");
    if (correct_uint(&key) == ERR_EOF) {
        *error = ERR_EOF;
        return NULL;
    }
    *error = ERR_OK;
    return tbl_search_by_key(table, key);
}

ERR dialog_tbl_insert(Table *table) {
    u_int key;
    u_int info;

    printf("Key: ");
    if (correct_uint(&key) == ERR_EOF) return ERR_EOF;

    printf("Info: ");
    if (correct_uint(&info) == ERR_EOF) return ERR_EOF;

    return tbl_insert(table, key, info);
}

ERR tbl_insert(Table *table, u_int key, u_int info) {
    const u_int size = table->m_size;
    u_int idx = hash1(key) % size;
    u_int step = hash2(key) % (size - 1);

    for (u_int i = 0; i < size; i++) {
        u_int cur = (idx + i * step) % size;
        KeySpace *cur_link = &(table->ks[cur]);

        if (cur_link->busy != BUSY) {

            Node *new_link = calloc(1, sizeof(Node));
            if (!new_link) {free(cur_link); return ERR_MEM;}

            u_int *copy_info = malloc(sizeof(u_int));
            if (!copy_info){
                free(cur_link);
                free(new_link);
                return ERR_MEM;
            }

            *copy_info = info;

            cur_link->key = key;
            cur_link->busy = BUSY;
            new_link->release = 1;
            new_link->info = copy_info;
            new_link->next = NULL;
            cur_link->node = new_link;

            return ERR_OK;
        }

        else if (cur_link->busy == BUSY && cur_link->key == key && cur_link->node != NULL) {
            Node *new_link = calloc(1, sizeof(Node));
            if (!new_link) {free(cur_link); return ERR_MEM;}
            // вставка в начало - важность + проще и быстрее
            u_int *copy_info = malloc(sizeof(u_int));
            if (!copy_info){
                free(cur_link);
                free(new_link);
                return ERR_MEM;
            }

            *copy_info = info;

            new_link->release = cur_link->node->release + 1;
            new_link->info = copy_info;
            new_link->next = cur_link->node;
            cur_link->node = new_link;

            return ERR_OK;
        }
    }
    return ERR_OVERFLOW;
}

ERR dialog_tbl_delete(Table *table){
    u_int key;
    u_int release;

    printf("Key: ");
    if (correct_uint(&key) == ERR_EOF) return ERR_EOF;

    KeySpace *ks = tbl_search_by_key(table, key);
    if (ks == NULL) return ERR_NOT_FOUND;

    printf("Enter 0 to delete all versions in the field below\n");
    printf("Release: ");
    ERR err = correct_uint(&release);
    if (err == ERR_EOF) return ERR_EOF;

    return tbl_delete(&ks, release);
}

ERR tbl_delete(KeySpace **ks, u_int release){
    if (release == 0){
        Node *current = (*ks)->node;
        while (current) {
            Node *next = current->next;
            free(current->info);
            free(current);
            current = next;
        }

        (*ks)->busy = DELETED;
        (*ks)->node = NULL;

        return ERR_OK;
    }
    else {
        Node **prev_ptr = &((*ks)->node);
        Node *current = (*ks)->node;

        while (current) {
            if (current->release == release) {
                *prev_ptr = current->next;
                free(current->info);
                free(current);

                // Если это была последняя версия
                if ((*ks)->node == NULL) {
                    (*ks)->busy = DELETED;
                }
                return ERR_OK;
            }

            prev_ptr = &(current->next);
            current = current->next;
        }

        return ERR_NOT_FOUND;
    }
}



void print_table(Table *table){
    if (table == NULL || table->ks == NULL) {
        printf("Table is empty!\n");
        return;
    }

    const u_int size = table->m_size;
    int flag = 0;

    for (int i = 0; i < size; i++) {
        printf("Busy : %d,  Key : %u     Values : {", table->ks[i].busy, table->ks[i].key);
        flag = 1;
        Node *cur_node = table->ks[i].node;
        while (cur_node != NULL) {
            printf(" [%u : %u] ", cur_node->release, *(cur_node->info));
            cur_node = cur_node->next;
        }
        printf("}\n");
    }
    if (flag == 0) printf("Table is empty!\n");
}



ERR table_export_binary(const Table* table, const char* filename) {
    if (!table || !filename) return ERR_OPEN;

    FILE* file = fopen(filename, "wb");
    if (!file) return ERR_OPEN;

    // Запись метаданных
    u_int metadata[2] = { table->m_size, count_busy_cells(table) };
    fwrite(metadata, sizeof(u_int), 2, file);

    // Запись BUSY ячеек
    for (u_int i = 0; i < table->m_size; i++) {
        KeySpace* ks = &(table->ks[i]);
        if (ks->busy != BUSY) continue;

        // Данные ячейки: индекс, ключ, количество версий
        u_int cell_header[3] = {i, ks->key, ks->node->release};
        fwrite(cell_header, sizeof(u_int), 3, file);

        // Список всех версий
        Node* current = ks->node;
        while (current) {
            u_int version_data[2] = {current->release, *(current->info)};
            fwrite(version_data, sizeof(u_int), 2, file);
            current = current->next;
        }
    }

    fclose(file);
    return ERR_OK;
}

u_int count_busy_cells(const Table* table) {
    u_int count = 0;
    for (u_int i = 0; i < table->m_size; i++) {
        if (table->ks[i].busy == BUSY) count++;
    }
    return count;
}

Table* table_import_binary(const char* filename) {
    if (!filename) return NULL;

    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    // Считывание метаданных
    u_int metadata[2];
    if (fread(metadata, sizeof(u_int), 2, file) != 2) {
        fclose(file);
        return NULL;
    }

    Table* table = calloc(1, sizeof(Table));
    if (!table) {
        fclose(file);
        return NULL;
    }

    table->m_size = metadata[0];
    table->ks = calloc(table->m_size, sizeof(KeySpace));
    if (!table->ks) {
        free(table);
        fclose(file);
        return NULL;
    }

    // Заполнение таблицы
    for (int i = 0; i < metadata[1]; i++) {
        u_int cell_header[3];
        // Данные ячейки cell_header: индекс, ключ, количество версий
        if (fread(cell_header, sizeof(u_int), 3, file) != 3) {
            free_table(table);
            fclose(file);
            return NULL;
        }

        KeySpace* ks = &(table->ks[cell_header[0]]);
        ks->busy = BUSY;
        ks->key = cell_header[1];

        // Обработка всех записанных версий
        Node** prev_ptr = &(ks->node);
        for (int j = 0; j < cell_header[2]; j++) {
            // информация - версия и само info
            u_int version_data[2];
            if (fread(version_data, sizeof(u_int), 2, file) != 2) {
                free_table(table);
                fclose(file);
                return NULL;
            }

            Node* new_node = malloc(sizeof(Node));
            if (!new_node) {
                free_table(table);
                fclose(file);
                return NULL;
            }

            new_node->release = version_data[0];
            new_node->info = (u_int *) malloc(sizeof(u_int));
            if (!new_node->info) {
                free(new_node);
                free_table(table);
                fclose(file);
                return NULL;
            }
            *(new_node->info) = version_data[1];
            new_node->next = NULL;

            *prev_ptr = new_node;
            prev_ptr = &(new_node->next);
        }
    }

    fclose(file);
    return table;
}


void free_table(Table *table){
    if (table == NULL || table->ks == NULL) {
        return;
    }

    const u_int size = table->m_size;
    for (int i = 0; i < size; i++){
        while (table->ks[i].node != NULL){
            Node *cur_node = table->ks[i].node;
            table->ks[i].node = cur_node->next;
            free(cur_node->info);
            cur_node->info = NULL;
            free(cur_node);
            cur_node = NULL;
        }
        table->ks[i].node = NULL;
        table->ks[i].key = 0;
        table->ks[i].busy = EMPTY;
    }
    free(table->ks);
    table->ks = NULL;
}