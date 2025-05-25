#include <stdlib.h>
#include "../include/menu.h"
#include "../include/Graph.h"
#include "../include/io.h"

int main(){
    Graph *graph = init_graph();
    if (!graph) return 1;

    ERR result;
    do {
        result = menu(&graph);
    } while (result == ERR_OK);


    destroy_graph(graph);
    free(graph);

    return 0;
}
