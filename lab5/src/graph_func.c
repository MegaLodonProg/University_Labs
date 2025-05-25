#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "../include/Graph.h"
#include "../include/queue.h"

// IMPORTANT =================== CREATE FUNCS ===========================

Graph *init_graph() {
    Graph *graph = calloc(1, sizeof(Graph));
    if (!graph) return NULL;

    graph->adj_matrix = calloc(1, sizeof(List_Node *));
    if (!graph->adj_matrix) {
        free(graph);
        return NULL;
    }

    return graph;
}

// IMPORTANT =================== SEARCH FUNCS ===========================

Vertex *find_ver(Graph *graph, u_int x, u_int y) {
    if (!graph || !graph->adj_matrix) return NULL;

    List_Node *cur_ver = graph->vertices;
    while (cur_ver) {
        if (cur_ver->vertex->x == x && cur_ver->vertex->y == y) {
            return cur_ver->vertex;
        }
        cur_ver = cur_ver->next;
    }
    return NULL;
}

// IMPORTANT =================== INSERT FUNCS ===========================

ERR insert_ver(Graph *graph, u_int x, u_int y, CELL_TYPE type) {
    if (!graph) {
        return ERR_MEM;
    }

    List_Node *new_vertex_node = malloc(sizeof(List_Node));
    if (!new_vertex_node) {
        return ERR_MEM;
    }
    new_vertex_node->vertex = malloc(sizeof(Vertex));
    if (!new_vertex_node->vertex) {
        free(new_vertex_node);
        return ERR_MEM;
    }

    new_vertex_node->vertex->x = x;
    new_vertex_node->vertex->y = y;
    new_vertex_node->vertex->type = type;
    new_vertex_node->next = NULL;

    // Вставка в конец
    if (!graph->vertices) {
        graph->vertices = new_vertex_node;
    } else {
        List_Node *cur = graph->vertices;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = new_vertex_node;

    }

    // обновление матрицы смежности
    List_Node **new_adj_matrix = realloc(graph->adj_matrix, (graph->num_vertices + 1) * sizeof(List_Node *));
    if (!new_adj_matrix) {
        return ERR_MEM;
    }

    graph->adj_matrix = new_adj_matrix;
    graph->adj_matrix[graph->num_vertices] = NULL;
    graph->num_vertices++;

    return ERR_OK;
}

ERR insert_edge(Graph *graph, Vertex *v1, Vertex *v2) {
    if (!graph) return ERR_MEM;
    if (!v1 || !v2) return ERR_NOT_FOUND;

    bool status = is_neighbor_ver(v1, v2);
    if (!status) return ERR_ACTION;

    int pos_v1 = find_pos_ver(graph, v1);

    // Проверяем, не существует ли уже такого ребра
    List_Node *edge_v1_v2 = graph->adj_matrix[pos_v1];
    while (edge_v1_v2) {
        if (edge_v1_v2->vertex == v2) {
            return ERR_DUPLICATE; // Ребро уже есть
        }
        edge_v1_v2 = edge_v1_v2->next;
    }

    List_Node *new_node = malloc(sizeof(List_Node));
    if (!new_node) return ERR_MEM;

    new_node->vertex = v2;
    new_node->next = graph->adj_matrix[pos_v1];
    graph->adj_matrix[pos_v1] = new_node;

    return ERR_OK;

}

// IMPORTANT =================== DELETE FUNCS ===========================

ERR delete_edge(Graph *graph, Vertex *v1, Vertex *v2) {
    if (!graph) return ERR_MEM;
    if (!v1 || !v2) return ERR_NOT_FOUND;

    int pos_v1 = find_pos_ver(graph, v1);

    List_Node **head = &(graph->adj_matrix[pos_v1]);
    List_Node *cur = *head;
    List_Node *prev = NULL;

    while (cur) {
        if (cur->vertex == v2) {
            if (prev) {
                prev->next = cur->next;
            } else {
                *head = cur->next;
            }
            free(cur);
            return ERR_OK;
        }
        prev = cur;
        cur = cur->next;
    }

    return ERR_NOT_FOUND;
}


ERR delete_ver(Graph *graph, u_int x, u_int y) {
    if (!graph || !graph->adj_matrix) {
        return ERR_EMPTY;
    }

    Vertex *del_ver = find_ver(graph, x, y);
    if (!del_ver) return ERR_NOT_FOUND;

    int pos_del_ver = find_pos_ver(graph, del_ver);

    List_Node *cur_node = graph->vertices;
    List_Node **head = &(graph->vertices);
    List_Node *prev = NULL;
    // Удаляем рёбра с соседями <--
    while (cur_node) {
        if (is_neighbor_ver(cur_node->vertex, del_ver)) {
            delete_edge(graph, cur_node->vertex, del_ver);
        }
        if (cur_node->vertex == del_ver) {
            List_Node *to_free = cur_node;  // Запоминаем узел для удаления
            if (prev) {
                prev->next = cur_node->next;
            } else {
                *head = cur_node->next;
            }
            cur_node = cur_node->next;
            free(to_free);
            continue;
        }
        prev = cur_node;
        cur_node = cur_node->next;

    }

    List_Node *del_node = graph->adj_matrix[pos_del_ver];
    while (del_node) {
        List_Node *temp = del_node;
        del_node = del_node->next;
        free(temp);
    }

    free(del_ver);

    // Сдвигаем строки матрицы (для удаляемой вершины)
    for (int i = pos_del_ver; i < graph->num_vertices - 1; i++) {
        graph->adj_matrix[i] = graph->adj_matrix[i + 1];
    }

    // 7. Уменьшаем размер матрицы
    graph->adj_matrix = realloc(graph->adj_matrix, (graph->num_vertices - 1) * sizeof(List_Node *));
    graph->num_vertices--;

    return ERR_OK;
}

// IMPORTANT =================== SEARCH-TRAVEL FUNCS ===========================

// Проверка достижимости (BFS)
ERR is_reachable_bfs(Graph *graph, Vertex *entry, bool *result) {
    if (!graph || !entry) return ERR_ACTION;
    if (graph->num_vertices == 0) return ERR_EMPTY;

    // WHITE - не посещена; GRAY - в очереди на обработку; BLACK - обработана
    COLOR *visited = calloc(graph->num_vertices, sizeof(COLOR)); // массив с закрашенными вершинами
    Queue *q = create_queue();

    int start_idx = find_pos_ver(graph, entry);
    if (start_idx == -1) {
        free(visited);
        free(q);
        return ERR_NOT_FOUND;
    }

    visited[start_idx] = GRAY;
    enqueue(q, entry);

    while (!is_queue_empty(q)) {
        Vertex *cur = dequeue(q);

        if (cur->type == EXIT) {
            *result = true; // уже нашли ближайший выход
            free(visited);
            while (!is_queue_empty(q)) {
                dequeue(q);
            }
            free(q);
            return ERR_OK;
        }

        int current_idx = find_pos_ver(graph, cur);
        List_Node *adj = graph->adj_matrix[current_idx]; // Получаем список смежных вершин из матрицы смежности

        while (adj) {
            int adj_idx = find_pos_ver(graph, adj->vertex);
            if (adj_idx != -1 && visited[adj_idx] == WHITE) {
                visited[adj_idx] = GRAY;
                // добавление в очередь на обработку
                enqueue(q, adj->vertex);
            }
            adj = adj->next;
        }

        visited[current_idx] = BLACK; // теперь текущая вершина полностью обработана
    }

    *result = false; // обошли все достижимые вершины и не нашли ни одного выхода
    free(visited);
    free(q);
    return ERR_OK;
}

// Поиск кратчайшего пути по алгоритму Дейкстра
ERR find_shortest_path(Graph *graph, Vertex *entry, Vertex *exit, Vertex ***path, int *path_len) {
    if (!graph || !entry || !exit || !path || !path_len) return ERR_ACTION;

    u_int size = graph->num_vertices;
    int entry_idx = find_pos_ver(graph, entry);
    if (entry_idx == -1) return ERR_NOT_FOUND;

    int exit_idx = find_pos_ver(graph, exit);
    if (exit_idx == -1) return ERR_NOT_FOUND;

    int *dist = malloc(size * sizeof(int)); // массив min расстояний для вершины
    int *prev = malloc(size * sizeof(int)); // массив предыдущих вершин
    bool *visited = calloc(size, sizeof(bool)); // массив с отметкой посещёнными вершинами

    for (int i = 0; i < size; i++) {
        dist[i] = INT_MAX; // сначала все недостижимые устанавливаем
        prev[i] = -1;
    }
    dist[entry_idx] = 0;

    // цикл обрабатывает все вершины графа
    for (int count = 0; count < size; count++) {
        int u = -1; // отвечает за выбор вершины
        for (int i = 0; i < size; i++) {
            // Находит непосещенную вершину с минимальным расстоянием
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                // если вершина ещё необработана или нашли меньшее расстояние обновляем min ver
                u = i;
            }
        }

        // выходим из цикла, если все вершины посещены или остались только недостижимые
        if (u == -1 || dist[u] == INT_MAX) break;

        visited[u] = true;
        List_Node *adj = graph->adj_matrix[u]; // смежные вершины для ver u

        while (adj) {
            int v = find_pos_ver(graph, adj->vertex);
            if (v != -1 && !visited[v] && dist[u] + 1 < dist[v]) { // если нашли путь короче
                dist[v] = dist[u] + 1;
                prev[v] = u;
            }
            adj = adj->next;
        }
    }

    // проверка на достижимость выхода
    if (dist[exit_idx] == INT_MAX) {
        free(dist);
        free(prev);
        free(visited);
        return ERR_NOT_FOUND;
    }

    // Считаем количество рёбер в пути при проходе по предшественникам
    int len = 0;
    int current = exit_idx;
    while (current != -1) {
        len++;
        current = prev[current];
    }

    // Массив указателей на вершины в пути
    Vertex **result_path = malloc(len * sizeof(Vertex *));
    current = exit_idx;
    for (int i = len - 1; i >= 0; i--) {
        List_Node *node = graph->vertices;
        // Продвигаемся до нужной вершины и сохраняем в массив пути
        for (int j = 0; j < current; j++) {
            node = node->next;
        }
        result_path[i] = node->vertex;
        current = prev[current];
    }

    *path = result_path;
    *path_len = len;

    free(dist);
    free(prev);
    free(visited);
    return ERR_OK;
}

// Поиск ближайшего выхода по алгоритму Флойда-Уоршелла
ERR find_nearest_exit(Graph *graph, Vertex *entry, Vertex **exit, int *distance) {
    if (!graph || !entry || !exit || !distance) return ERR_ACTION;

    u_int size = graph->num_vertices;
    int entry_idx = find_pos_ver(graph, entry);
    if (entry_idx == -1) return ERR_NOT_FOUND;

    int **dist = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        dist[i] = malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) {
            dist[i][j] = i == j ? 0 : INT_MAX;
        }
    }

    // Инициализация матрицы смежности
    for (int u = 0; u < size; u++) {
        List_Node *adj = graph->adj_matrix[u];
        while (adj) {
            int v = find_pos_ver(graph, adj->vertex);
            if (v != -1) dist[u][v] = 1;
            adj = adj->next;
        }
    }

    for (int k = 0; k < size; k++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX &&
                    dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    // Поиск ближайшего выхода
    int min_dist = INT_MAX;
    int exit_idx = -1;
    List_Node *node = graph->vertices;
    for (int i = 0; i < size; i++) {
        if (node->vertex->type == EXIT && dist[entry_idx][i] < min_dist) {
            min_dist = dist[entry_idx][i];
            exit_idx = i;
        }
        node = node->next;
    }

    for (int i = 0; i < size; i++) free(dist[i]);
    free(dist);

    if (exit_idx == -1) return ERR_NOT_FOUND;

    *distance = min_dist;
    node = graph->vertices;
    for (int i = 0; i < exit_idx; i++) node = node->next;
    *exit = node->vertex;

    return ERR_OK;
}

// IMPORTANT =================== PRINT FUNCS ===========================

void print_adj_matrix(Graph *graph) {
    if (!graph || !graph->adj_matrix) {
        printf("Graph is empty!\n");
        return;
    }

    // Проходим по всем вершинам
    List_Node *vertex_node = graph->vertices;
    for (u_int i = 0; i < graph->num_vertices && vertex_node != NULL; i++, vertex_node = vertex_node->next) {
        Vertex *v = vertex_node->vertex;
        printf("Ver {%u, %u}: ", v->x, v->y);

        // Печатаем список смежности для текущей вершины
        List_Node *edge_node = graph->adj_matrix[i];
        if (!edge_node) {
            printf(" - ");
        } else {
            while (edge_node) {
                Vertex *adjacent = edge_node->vertex;
                printf("{%u, %u} ", adjacent->x, adjacent->y);
                edge_node = edge_node->next;
            }
        }
        printf("\n");
    }
}

int compare_coords(const void *a, const void *b) {
    Coord *coord_a = (Coord *)a;
    Coord *coord_b = (Coord *)b;
    if (coord_a->x != coord_b->x)
        return coord_a->x - coord_b->x;
    return coord_a->y - coord_b->y;
}

void visualize_graph(Graph *graph, const char *output_filename) {
    if (!graph || !output_filename) return;

    FILE *dot_file = fopen("temp_graph.dot", "w");
    if (!dot_file) {
        perror("Failed to create DOT file");
        return;
    }

    // Настройки графа
    fprintf(dot_file, "digraph G {\n");
    fprintf(dot_file, "    rankdir=TB;\n");  // Вертикальное направление
    fprintf(dot_file, "    node [shape=square, style=filled, width=0.8, height=0.8, fixedsize=true];\n");
    fprintf(dot_file, "    edge [arrowsize=0.7, color=\"#666666\"];\n\n");

    List_Node *v_node = graph->vertices;

    // Подсчёт количества вершин
    int vertex_count = 0;
    while (v_node) {
        vertex_count++;
        v_node = v_node->next;
    }

    // Создание массива координат для сортировки
    Coord *coords = malloc(vertex_count * sizeof(Coord));
    if (!coords) {
        perror("Memory allocation failed");
        fclose(dot_file);
        remove("temp_graph.dot");
        return;
    }

    v_node = graph->vertices;
    for (int i = 0; v_node; i++, v_node = v_node->next) {
        coords[i].x = v_node->vertex->x;
        coords[i].y = v_node->vertex->y;
    }

    // Сортировка координат для группировки по X
    qsort(coords, vertex_count, sizeof(Coord), compare_coords);

    v_node = graph->vertices;
    while (v_node) {
        Vertex *v = v_node->vertex;
        const char *color;
        const char *fontcolor = "black";

        switch (v->type) {
            case ENTRY:
                color = "#9678b6";
                fontcolor = "white";
                break;
            case EXIT:
                color = "#7da67d";
                fontcolor = "white";
                break;
            default:
                color = "#e8e8e8";
                break;
        }

        fprintf(dot_file, "    \"%u_%u\" [label=\"%u,%u\", fillcolor=\"%s\", fontcolor=\"%s\"];\n",
                v->x, v->y, v->x, v->y, color, fontcolor);
        v_node = v_node->next;
    }

    // Группируем вершины по одинаковым X
    fprintf(dot_file, "\n    // Horizontal alignment (by X)\n");
    unsigned int current_x = ~0U;

    v_node = graph->vertices;
    while (v_node) {
        Vertex *v = v_node->vertex;
        if (v->x != current_x) {
            if (current_x != ~0U) fprintf(dot_file, "    }\n");
            current_x = v->x;
            fprintf(dot_file, "    { rank=same; ");
        }
        fprintf(dot_file, "\"%u_%u\"; ", v->x, v->y);
        v_node = v_node->next;
    }
    if (current_x != ~0U) fprintf(dot_file, "    }\n");

    fprintf(dot_file, "\n    // Invisible connections for vertical alignment (by Y)\n");

    // Для каждой уникальной Y-координаты строим цепочку сверху вниз
    int total_coords = vertex_count;
    for (int i = 0; i < total_coords; ++i) {
        unsigned int y = coords[i].y;
        unsigned int x = coords[i].x;

        bool found_above = false;
        for (int j = 0; j < total_coords; ++j) {
            if (coords[j].x == x && coords[j].y == y - 1) {
                found_above = true;
                fprintf(dot_file, "    \"%u_%u\" -> \"%u_%u\" [style=invis];\n",
                        coords[j].x, coords[j].y, x, y);
                break;
            }
        }
    }

    // Добавляем рёбра
    fprintf(dot_file, "\n    // Vertex connections\n");
    for (u_int i = 0; i < graph->num_vertices; i++) {
        List_Node *edge = graph->adj_matrix[i];
        if (!edge) continue;

        List_Node *tmp = graph->vertices;
        for (u_int j = 0; j < i && tmp; j++) tmp = tmp->next;
        if (!tmp) continue;

        Vertex *from = tmp->vertex;
        while (edge) {
            Vertex *to = edge->vertex;
            fprintf(dot_file, "    \"%u_%u\" -> \"%u_%u\";\n",
                    from->x, from->y, to->x, to->y);
            edge = edge->next;
        }
    }

    fprintf(dot_file, "}\n");
    fclose(dot_file);
    free(coords);

    // Генерация изображения
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng temp_graph.dot -o %s", output_filename);
    system(command);
    remove("temp_graph.dot");

    printf("Graph saved as: %s\n", output_filename);
}

// IMPORTANT =================== FREE FUNCS ===========================

void destroy_graph(Graph *graph) {
    if (!graph) return;

    if (graph->adj_matrix) {
        for (int i = 0; i < graph->num_vertices; i++) {
            List_Node *cur = graph->adj_matrix[i];
            while (cur) {
                List_Node *temp = cur;
                cur = cur->next;
                free(temp);
            }
        }
        free(graph->adj_matrix);
    }

    List_Node *cur_ver = graph->vertices;
    while (cur_ver) {
        List_Node *temp = cur_ver;
        cur_ver = cur_ver->next;

        if (temp->vertex) {
            free(temp->vertex);
        }
        free(temp);
    }

    graph->vertices = NULL;
    graph->adj_matrix = NULL;
    graph->num_vertices = 0;
}

// IMPORTANT =================== HELP FUNCS ===========================

bool is_neighbor_ver(Vertex *v1, Vertex *v2) {
    int res = (abs(((int) v1->x - (int) v2->x)) + abs((int) v1->y - (int) v2->y));
    if (res == 1) {
        return true;
    }
    return false;
}

int find_pos_ver(Graph *graph, Vertex *target_ver) {
    int pos = 0;
    List_Node *cur_v1 = graph->vertices;
    while (cur_v1 && cur_v1->vertex != target_ver) {
        pos++;
        cur_v1 = cur_v1->next;
    }
    if (!cur_v1 || cur_v1->vertex != target_ver) return -1;

    return pos;
}
