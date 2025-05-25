#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef unsigned int u_int;

typedef enum {
    ERR_OK,
    ERR_EOF,
    ERR_MEM,
    ERR_EXIT,
    ERR_EMPTY,
    ERR_NOT_FOUND,
    ERR_FORMAT,
    ERR_DUPLICATE,
    ERR_ACTION,
} ERR;

typedef struct Coord {
    unsigned int x;
    unsigned int y;
} Coord;

typedef enum {
    WHITE,
    GRAY,
    BLACK,
} COLOR;

typedef enum {
    DEFAULT,
    ENTRY,
    EXIT,
} CELL_TYPE;


typedef struct Vertex {
    u_int x, y;
    CELL_TYPE type;
} Vertex;

typedef struct List_Node {
    Vertex *vertex; // Вершина, в которую ведёт ребро
    struct List_Node* next;
} List_Node;

typedef struct Graph{
    List_Node* vertices;
    List_Node **adj_matrix;
    u_int num_vertices; // Количество вершин
}Graph;


Graph* init_graph();

Vertex* find_ver(Graph *graph, u_int x, u_int y);

ERR insert_ver(Graph *graph, u_int x, u_int y, CELL_TYPE type);
ERR insert_edge(Graph *graph, Vertex *v1, Vertex *v2);

ERR delete_ver(Graph *graph, u_int x, u_int y);
ERR delete_edge(Graph *graph, Vertex *v1, Vertex *v2);

void print_adj_matrix(Graph *graph);
void visualize_graph(Graph *graph, const char *output_filename);

void destroy_graph(Graph *graph);

bool is_neighbor_ver(Vertex *v1, Vertex *v2);
int find_pos_ver(Graph *graph, Vertex *target_ver);

ERR is_reachable_bfs(Graph* graph, Vertex* entry, bool* result);
ERR find_shortest_path(Graph* graph, Vertex* entry, Vertex* exit, Vertex*** path, int* path_len);
ERR find_nearest_exit(Graph* graph, Vertex* entry, Vertex** exit, int* dist);

#endif //GRAPH_H
