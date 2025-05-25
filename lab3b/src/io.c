#include "../include/io.h"

ERR correct_command(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%d", num)) == 1 && *num >= 0 && *num <= 6){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return ERR_EOF;
        }
        printf("Command not found! Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return ERR_OK;
}

ERR correct_uint(unsigned int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%u", num)) == 1 && *num >= 0){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return ERR_EOF;
        }
        printf("Invalid input! Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return ERR_OK;
}

// вывод массива с запрашиваемыми элементами
void print_values_arr(const u_int *arr, const int size){
    printf("\nYour arr with values:\n");
    for (int i = 0; i < size; i++){
        printf("[%u] ", arr[i]);
    }
    printf("\n");
}

ERR get_values_arr(const KeySpace *ks, u_int **arr) {
    printf("If you want to get all values, enter 0 in the field below.\n");
    printf("Release: ");
    u_int release;
    ERR err = correct_uint(&release);
    if (err == ERR_EOF) return ERR_EOF;

    const int size_arr = (release != 0) ? 1 : ks->node->release;
    *arr = malloc(size_arr * sizeof(u_int));
    if (*arr == NULL) return ERR_MEM;

    // добавление всех элементов
    if (release == 0){
        Node *cur_node = ks->node;
        for (int i = 0; i < size_arr; i++){
            // для порядка добавления -->
            (*arr)[size_arr - i - 1] = *(cur_node->info);
            cur_node = cur_node->next;
        }
    }
    else {
        if (release > ks->node->release){
            printf("This release is missing!\n");
            return ERR_NOT_FOUND;
        }
        Node *cur_node = ks->node;
        while (cur_node->release != release){
            cur_node = cur_node->next;
        }
        (*arr)[0] = *(cur_node->info);
    }

    print_values_arr(*arr, size_arr);
    return ERR_OK;
}
