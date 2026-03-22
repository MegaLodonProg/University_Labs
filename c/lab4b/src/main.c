#include <stdio.h>
#include <stdlib.h>
#include "../include/menu.h"
#include "../include/Tree.h"
#include "../include/func_time.h"

int main() {
    B_Tree* tree = create_tree();
    if (!tree) return 1;

    ERR result;
    do {
        result = menu(tree);
    } while (result == ERR_OK);

    destroy_tree(tree);
    free(tree);
    tree = NULL;

    func_time();

    return 0;
}
