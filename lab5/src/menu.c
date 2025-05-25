#include <stdlib.h>
#include <limits.h>
#include "../include/Graph.h"
#include "../include/ui.h"
#include "../include/io.h"


void check_error(ERR error) {
    switch (error) {
        case ERR_MEM:
            fprintf(stderr, "Invalid memory alloc!\n");
            break;
        case ERR_NOT_FOUND:
            printf("Data is not found!\n");
            break;
        case ERR_FORMAT:
            fprintf(stderr, "Wrong format!\n");
            break;
        case ERR_EMPTY:
            printf("Table is empty!\n");
            break;
        case ERR_DUPLICATE:
            printf("Data is already in graph!\n");
            break;
        case ERR_ACTION:
            printf("Wrong action!\n");
            break;
        default:
            break;
    }
}

void print_menu_info() {
    printf("____MENU____\n");
    printf("(1) Insert a new vertex\n");
    printf("(2) Insert a new edge\n");
    printf("(3) Delete a vertex\n");
    printf("(4) Delete a edge\n");
    printf("(5) Print graph\n");
    printf("(6) Check exit reachability from entry (BFS)\n");
    printf("(7) Find shortest path between entry and exit (Dijkstra)\n");
    printf("(8) Find nearest exit from entry (Floyd-Warshall)\n");
    printf("(9) Change type of vertex\n");
    printf("(0) Exit\n");
}

ERR menu(Graph **graph) {
    print_menu_info();

    int command;
    int st = correct_command(&command);
    if (st == ERR_EOF) return ERR_EOF;

    ERR err_status;

    switch (command) {
        case 0:
            return ERR_EXIT;
        case 1: {
            err_status = d_insert_ver(*graph);
            check_error(err_status);
            if (err_status != ERR_OK && err_status != ERR_DUPLICATE) return err_status;
            break;
        }
        case 2:
            err_status = d_insert_edge(*graph);
            check_error(err_status);
            if (err_status == ERR_MEM || err_status == ERR_EXIT) return err_status;
            break;
        case 3: {
            err_status = d_del_ver(*graph);
            check_error(err_status);
            if (err_status == ERR_MEM || err_status == ERR_EXIT) return err_status;
            break;
        }
        case 4:
            err_status = d_del_edge(*graph);
            check_error(err_status);
            if (err_status == ERR_MEM || err_status == ERR_EXIT) return err_status;
            break;
        case 5: {
            printf("Choose method of print graph:\n");
            const char *msgs[] = {"1. Print adjective matrix", "2. Graphic graph"};
            const int count_choose = sizeof(msgs) / sizeof(msgs[0]);
            for (int i = 0; i < count_choose; i++) puts(msgs[i]);
            int method = 0;
            while (!(1 <= method && method <= 2)) {
                if (correct_command(&method) != ERR_OK) return ERR_EXIT;
                if (method > 2) printf("Invalid input! Try again.\n");
            }
            if (method == 1) print_adj_matrix(*graph);
            else visualize_graph(*graph, "graph.png");
            break;
        }
        case 6: {
            u_int x, y;
            err_status = general_ver_input(&x, &y);
            if (err_status == ERR_EOF) return err_status;

            Vertex *entry = find_ver(*graph, x, y);
            if (!entry) {
                check_error(ERR_NOT_FOUND);
                break;
            }
            if (entry->type != ENTRY) {
                printf("This is not an ENTRY!\n");
                check_error(ERR_ACTION);
                break;
            }

            bool result;
            err_status = is_reachable_bfs(*graph, entry, &result);
            if (err_status != ERR_OK) {
                check_error(err_status);
                break;
            }

            if (result){
                puts("Exit is REACHABLE\n");
            } else puts("Exit is NOT REACHABLE\n");

            break;
        }
        case 7: {
            u_int x1, y1, x2, y2;
            err_status = general_ver_input(&x1, &y1);
            if (err_status != ERR_OK) return err_status;

            Vertex *entry = find_ver(*graph, x1, y1);
            if (!entry || entry->type != ENTRY) {
                printf("Invalid ENTRY vertex!\n");
                break;
            }

            err_status = general_ver_input(&x2, &y2);
            if (err_status != ERR_OK) return err_status;

            Vertex *exit = find_ver(*graph, x2, y2);
            if (!exit || exit->type != EXIT) {
                printf("Invalid EXIT vertex!\n");
                break;
            }

            Vertex **path = NULL; // массив со сохранением пути из вершин
            int path_len = 0;
            err_status = find_shortest_path(*graph, entry, exit, &path, &path_len);
            if (err_status != ERR_OK) {
                check_error(err_status);
                break;
            }

            printf("Shortest path (%d steps):\n", path_len - 1);
            for (int i = 0; i < path_len - 1; i++) {
                printf("(%u, %u)%s", path[i]->x, path[i]->y, " -> ");
            }
            printf("(%u, %u)%s", path[path_len - 1]->x, path[path_len - 1]->y, "\n");

            free(path);
            break;
        }
        case 8: {
            u_int x, y;
            err_status = general_ver_input(&x, &y);
            if (err_status != ERR_OK) return err_status;

            Vertex *entry = find_ver(*graph, x, y);
            if (!entry || entry->type != ENTRY) {
                printf("Invalid ENTRY vertex!\n");
                break;
            }

            Vertex *exit = NULL;
            int dist;
            err_status = find_nearest_exit(*graph, entry, &exit, &dist);
            if (err_status != ERR_OK) {
                check_error(err_status);
                return err_status;
            }

            if (dist == INT_MAX) {
                printf("Such exits not found!\n");
            } else {
                printf("Nearest exit at (%u, %u), distance: %d\n", exit->x, exit->y, dist);
            }
            break;
        }
        case 9: {
            err_status = d_change_ver(*graph);
            check_error(err_status);
            if (err_status == ERR_MEM || err_status == ERR_EXIT) return err_status;
            break;
        }
        default:
            printf("Command not found!\n");
            return ERR_EXIT;
    }
    return ERR_OK;
}
