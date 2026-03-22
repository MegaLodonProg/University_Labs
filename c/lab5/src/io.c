#include "../include/io.h"

ERR correct_u_int(unsigned int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%u", num)) == 1){
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

ERR correct_cell(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%u", num)) == 1 && *num <= 3 && *num >= 1){
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

ERR correct_command(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%u", num)) == 1 && *num <= 9 && *num >= 0){
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
