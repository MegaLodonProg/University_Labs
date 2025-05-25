#include "../include/io.h"
#include <string.h>

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

ERR correct_int(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%d", num)) == 1 && *num > 0){
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

char *readline_file(FILE * file){
    char buf[81] = {0};
    char *res = NULL;
    int len = 0, n = 0;

    do {
        n = fscanf(file, "%80[^\n]", buf);
        if (n < 0){
            if (!res){
                return NULL;
            }
        }
        else if (n > 0) {
            int chunk_len = strlen(buf);
            int str_len = len + chunk_len;
            res = realloc(res, str_len + 1);
            strncpy(res + len, buf, chunk_len);
            len = str_len;
            if (buf[len-1] == 13) len --;
        }
        else {
            // сразу встретили \n
            fscanf(file, "%*c");
        }
    } while (n > 0);

    if (len > 0){
        res[len] = '\0';
    }
    else {
        res = calloc(1, sizeof(char));
    }
    return res;
}
