#include "../include/Tree.h"
#include "../include/io.h"
#include "../include/menu.h"
#include "readline/readline.h"

void check_error(ERR error){
    switch (error) {
        case ERR_MEM:
            fprintf(stderr, "Invalid memory alloc!\n");
            break;
        case ERR_NOT_FOUND:
            printf("Data is not found!\n");
            break;
        case ERR_FORMAT:
            fprintf(stderr, "Wrong format!\n");
            break;
        case ERR_EMPTY:
            printf("Tree is empty!\n");
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
    printf("(1) Insert a new element into the tree\n");
    printf("(2) Remove element from tree\n");
    printf("(3) Search an element by key in a tree\n");
    printf("(4) Print tree\n");
    printf("(5) Special search in a tree\n");
    printf("(6) Import tree from text file\n");
    printf("(0) Exit\n");
}

ERR menu(B_Tree *tree){
    print_menu_info();

    int command;
    int st = correct_command(&command);
    if (st == ERR_EOF) return ERR_EOF;

    ERR err_status;

    switch (command) {
        case 0:
            return ERR_EXIT;
        case 1: {
            err_status = d_insert(tree);
            check_error(err_status);
            if (err_status != ERR_OK) return err_status;
            break;
        }
        case 2:
            err_status = d_delete(tree);
            check_error(err_status);
            if (err_status != ERR_OK && err_status != ERR_NOT_FOUND && err_status != ERR_EMPTY) return err_status;
            break;
        case 3: {
            int key_pos = 0;
            char *found_inf = NULL;
            TreeNode *node = d_search(tree, &key_pos, &found_inf, &err_status);
            if (!node){
                check_error(err_status);
                break;
            }

            if (err_status != ERR_OK) return err_status;

            printf("Key: " "%u" "\t\tValue: " "%s\n", node->keys_arr[key_pos], found_inf);
            free(found_inf);
            break;
        }
        case 4:
            printf("Choose method of print tree:\n");
            const char *msgs[] = {"1. Print all tree", "2. Graphic tree"};
            const int count_choose = sizeof(msgs) / sizeof(msgs[0]);
            for (int i = 0; i < count_choose; i++) puts(msgs[i]);
            int method = 0;
            while (!(1 <= method && method <= 3)){
                if (correct_command(&method) != ERR_OK) return ERR_EXIT;
                if (method > 3) printf("Invalid input! Try again.\n");
            }
            if (method == 1) print_tree(tree);
            else if (method == 2) visualize_b_tree(tree, "b-tree.png");
            break;
        case 5: {
            u_int target_key;
            int release;

            printf("Key : ");
            if (correct_uint(&target_key) != ERR_OK) return ERR_EXIT;

            printf("Release: ");
            if (correct_int(&release) == ERR_EOF) return ERR_EXIT;

            u_int found_key = 0;
            char *found_inf = NULL;

            ERR status = special_search_node(tree, target_key, release, &found_key, &found_inf);
            if (status != ERR_OK){
                printf("The element is the smallest or there are no such versions!\n");
                break;
            }

            printf("Key: " "%u" "\t\tValue: " "%s\n", found_key, found_inf);
            free(found_inf);
            break;
        }
        case 6: {
            char* filename = readline("Filename: ");
            if (!filename) {
                return ERR_MEM;
            }

            ERR import_status = tree_import(tree, filename);
            check_error(import_status);
            free(filename);

            break;
        }
        default:
            printf("Command not found!\n");
            return ERR_EXIT;
    }
    return ERR_OK;
}
