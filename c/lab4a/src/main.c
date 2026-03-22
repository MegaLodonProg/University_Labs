#include <stdlib.h>
#include "../include/func_time.h"
#include "../include/Tree.h"
#include "../include/menu.h"

int main(){
    TreeNode *tree = NULL;

    ERR result;
    do {
        result = menu(&tree);
    } while (result == ERR_OK);

    free_tree(&tree);

    func_time();

    return 0;
}