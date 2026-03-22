#include "readline/readline.h"
#include "../include/Tree.h"
#include "../include/io.h"
#include "../include/menu.h"

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
    printf("(1) Insert a new element into the tree\n");
    printf("(2) Remove element from tree\n");
    printf("(3) Search an element by key in a tree\n");
    printf("(4) Print tree\n");
    printf("(5) Special search in a tree\n");
    printf("(6) Import tree from text file\n");
    printf("(0) Exit\n");
}

ERR menu(TreeNode **tree){
    print_menu_info();

    int command;
    int st = correct_command(&command);
    if (st == ERR_EOF) return ERR_EOF;

    ERR err_status;

    switch (command) {
        case 0:
            return ERR_EXIT;
        case 1: {
            err_status = dialog_insert(tree);
            check_error(err_status);
            if (err_status != ERR_OK) return err_status;
            break;
        }
        case 2:
            err_status = dialog_del_node(tree);
            check_error(err_status);
            if (err_status != ERR_OK && err_status != ERR_NOT_FOUND) return err_status;
            break;
        case 3: {
            TreeNode *node = dialog_search(*tree, &err_status);
            check_error(err_status);
            if (!node){
                check_error(ERR_NOT_FOUND);
                break;
            }

            if (err_status != ERR_OK && err_status != ERR_NOT_FOUND) return err_status;

            printf("Key: " BLUE "%-10s" RESET "Value: " GREEN "%s\n" RESET, node->key, node->info);
            break;
        }
        case 4:
            printf("Choose method of print tree:\n");
            const char *msgs[] = {"1. Print all tree", "2. Graphic tree", "3. Keys by prefix"};
            const int count_choose = sizeof(msgs) / sizeof(msgs[0]);
            for (int i = 0; i < count_choose; i++) puts(msgs[i]);
            int method = 0;
            while (!(1 <= method && method <= 3)){
                if (correct_command(&method) != ERR_OK) return ERR_EXIT;
                if (method > 3) printf("Invalid input! Try again.\n");
            }
            if (method == 1) print_tree(*tree, 0, 0);
            else if (method == 2) visualize_tree(*tree, "tree.png");
            else {
                char *pref = readline("Prefix: ");
                if (!pref) return ERR_MEM;
                print_nodes_with_prefix(*tree, pref);
                free(pref);
            }

            break;
        case 5: {
            char *target_key = readline("Target key: ");
            TreeNode *node = special_search(*tree, target_key);
            free(target_key);

            if (!node) printf("Tree is empty\n");
            else printf("Key: %s    Value: %s\n", node->key, node->info);
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
