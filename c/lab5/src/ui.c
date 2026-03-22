#include "../include/ui.h"


Vertex* d_find_ver(Graph *graph, ERR *err){
    if (!graph || !graph->adj_matrix) {
        *err = ERR_EMPTY;
        return NULL;
    }

    u_int x, y;
    printf("Input x: ");
    if (correct_u_int(&x) != ERR_OK) {
        *err = ERR_EXIT;
        return NULL;
    }
    printf("Input y: ");
    if (correct_u_int(&y) != ERR_OK){
        *err = ERR_EXIT;
        return NULL;
    }

    Vertex* found_ver = find_ver(graph, x, y);
    *err = found_ver ? ERR_OK : ERR_NOT_FOUND;

    return found_ver;
}

ERR d_insert_ver(Graph *graph){
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }

    u_int x, y;
    printf("Input x: ");
    if (correct_u_int(&x) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y: ");
    if (correct_u_int(&y) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver = find_ver(graph, x, y);
    if (found_ver) return ERR_DUPLICATE;

    int cell_choice;
    CELL_TYPE type;
    printf("1 - DEFAULT; 2 - ENTRY; 3 - EXIT\n");
    printf("Cell type: ");
    if (correct_cell(&cell_choice) != ERR_OK) return ERR_EXIT;

    if (cell_choice == 1) type = DEFAULT;
    else if (cell_choice == 2) type = ENTRY;
    else type = EXIT;

    // Гарантированная передача уникальной вершины
    ERR status = insert_ver(graph, x, y, type);

    return status;
}


ERR d_insert_edge(Graph *graph){
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }

    printf("Rule of Add edge (x1, y1) --> (x2, y2)\n");

    u_int x1, y1;
    printf("Input x1: ");
    if (correct_u_int(&x1) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y1: ");
    if (correct_u_int(&y1) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver1 = find_ver(graph, x1, y1);
    if (!found_ver1) return ERR_NOT_FOUND;

    u_int x2, y2;
    printf("Input x2: ");
    if (correct_u_int(&x2) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y2: ");
    if (correct_u_int(&y2) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver2 = find_ver(graph, x2, y2);
    if (!found_ver2) return ERR_NOT_FOUND;


    ERR status = insert_edge(graph, found_ver1, found_ver2);

    return status;
}

ERR d_del_ver(Graph *graph){
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }

    u_int x, y;
    printf("Input x: ");
    if (correct_u_int(&x) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y: ");
    if (correct_u_int(&y) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver1 = find_ver(graph, x, y);
    if (!found_ver1) return ERR_NOT_FOUND;

    ERR status = delete_ver(graph, x, y);

    return status;
}

ERR d_del_edge(Graph *graph){
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }

    u_int x1, y1;
    printf("Input x1: ");
    if (correct_u_int(&x1) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y1: ");
    if (correct_u_int(&y1) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver1 = find_ver(graph, x1, y1);
    if (!found_ver1) return ERR_NOT_FOUND;

    u_int x2, y2;
    printf("Input x2: ");
    if (correct_u_int(&x2) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y2: ");
    if (correct_u_int(&y2) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver2 = find_ver(graph, x2, y2);
    if (!found_ver2) return ERR_NOT_FOUND;


    ERR status = delete_edge(graph, found_ver1, found_ver2);

    return status;
}

ERR d_change_ver(Graph *graph){
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }
    u_int x, y;
    printf("Input x: ");
    if (correct_u_int(&x) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y: ");
    if (correct_u_int(&y) != ERR_OK){
        return ERR_EXIT;
    }

    Vertex* found_ver = find_ver(graph, x, y);
    if (!found_ver) return ERR_NOT_FOUND;


    int cell_choice;
    CELL_TYPE new_type;
    printf("1 - DEFAULT; 2 - ENTRY; 3 - EXIT\n");
    printf("Cell type: ");
    if (correct_cell(&cell_choice) != ERR_OK) return ERR_EXIT;

    if (cell_choice == 1) new_type = DEFAULT;
    else if (cell_choice == 2) new_type = ENTRY;
    else new_type = EXIT;

    found_ver->type = new_type;
    return ERR_OK;
}


ERR general_ver_input(u_int *x, u_int *y) {
    printf("Input x: ");
    if (correct_u_int(x) != ERR_OK) {
        return ERR_EXIT;
    }
    printf("Input y: ");
    if (correct_u_int(y) != ERR_OK){
        return ERR_EXIT;
    }
    return ERR_OK;
}