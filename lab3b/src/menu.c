#include "../include/Table.h"
#include "../include/io.h"
#include "../include/menu.h"
#include "readline/readline.h"

void check_error(ERR error){
    switch (error) {
        case ERR_MEM:
            fprintf(stderr, "Invalid memory alloc!\n");
            break;
        case ERR_OVERFLOW:
            printf("Table is overflow!\n");
            break;
        case ERR_NOT_FOUND:
            printf("Data is not found!\n");
            break;
        case ERR_FORMAT:
            fprintf(stderr, "Wrong format!\n");
            break;
        case ERR_EMPTY:
            printf("Table is empty!\n");
            break;
        case ERR_OPEN:
            printf("Wrong open. Check the filename!\n");
            break;
        default:
            break;
    }
}

void print_menu_info(){
    printf("____MENU____\n");
    printf("(1) Insert a new element into the table\n");
    printf("(2) Remove element from table\n");
    printf("(3) Search an element in a table\n");
    printf("(4) Print table\n");
    printf("(5) Export table to binary file\n");
    printf("(6) Import table from binary file\n");
    printf("(0) Exit\n");
}

ERR menu(Table **table){
    print_menu_info();

    int command;
    int st = correct_command(&command);
    if (st == ERR_EOF) return ERR_EOF;

    ERR err_status;

    switch (command) {
        case 0:
            return ERR_EXIT;
        case 1: {
            err_status = dialog_tbl_insert(*table);
            check_error(err_status);
            if (err_status != ERR_OK && err_status != ERR_OVERFLOW) return err_status;
            break;
        }
        case 2:
            err_status = dialog_tbl_delete(*table);
            check_error(err_status);
            if (err_status != ERR_OK && err_status != ERR_NOT_FOUND) return err_status;
            break;
        case 3: {
            KeySpace *ks = dialog_tbl_search(*table, &err_status);
            check_error(err_status);
            if (ks == NULL){
                check_error(ERR_NOT_FOUND);
                break;
            }

            if (err_status != ERR_OK) return err_status;

            u_int *arr = NULL;
            ERR err = get_values_arr(ks, &arr);
            check_error(err);
            if (err != ERR_OK && err != ERR_NOT_FOUND) return err;

            free(arr);
            break;
        }
        case 4:
            print_table(*table);
            break;
        case 5: {
            char* filename = readline("Filename: ");
            if (!filename) return ERR_MEM;

            ERR err = table_export_binary(*table, filename);
            check_error(err);
            free(filename);
            break;
        }
        case 6: {
            char* filename = readline("Filename: ");
            if (!filename) return ERR_MEM;

            Table *import_table = table_import_binary(filename);
            free(filename);

            if (!import_table) {
                printf("Invalid file reading!\n");
                break;
            }

            Table *old_table = *table;

            // Заменяем таблицу
            *table = import_table;
            print_table(import_table);

            free_table(old_table);
            free(old_table);

            break;
        }
        default:
            printf("Command not found!\n");
            return ERR_EXIT;
    }
    return ERR_OK;
}
