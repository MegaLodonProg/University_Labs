#ifndef LAB5_UI_H
#define LAB5_UI_H

#include <stdio.h>
#include "../include/io.h"
#include "../include/Graph.h"


Vertex* d_find_ver(Graph *graph, ERR *err);

ERR d_insert_ver(Graph *graph);
ERR d_insert_edge(Graph *graph);
ERR d_del_ver(Graph *graph);
ERR d_del_edge(Graph *graph);
ERR d_change_ver(Graph *graph);
ERR general_ver_input(u_int *x, u_int *y);


#endif //LAB5_UI_H
