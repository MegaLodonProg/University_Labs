#include "../include/func_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned int generate_random_key() {
    // Инициализация генератора (только при первом вызове)
    static int is_initialized = 0;
    if (!is_initialized) {
        srand(time(NULL));  // Зависит от времени, поэтому при каждом запуске — новая последовательность
        is_initialized = 1;
    }
    return rand();  // Возвращает случайное число от 0 до RAND_MAX
}


char* generate_info() {
    char* info = malloc(2);
    strcpy(info, "1");
    return info;
}

void prefill_tree(B_Tree *tree, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        u_int key = generate_random_key();
        char* info = generate_info();
        insert_node(tree, key, info);
    }
}

double measure_insert(B_Tree *tree, int num_elements) {
    clock_t start = clock();

    for (int i = 0; i < num_elements; i++) {
        u_int key = generate_random_key();
        char* info = generate_info();
        insert_node(tree, key, info);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

double measure_search(B_Tree *tree, int num_calls) {
    int found_pos_key = 0;
    char *found_info = NULL;

    clock_t start = clock();

    for (int i = 0; i < num_calls; i++) {
        u_int key = generate_random_key();
        search_node(tree, key, 1, &found_pos_key, &found_info);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}




double measure_delete(B_Tree *tree, int num_calls) {
    clock_t start = clock();

    for (int i = 0; i < num_calls; i++) {
        u_int key = generate_random_key();
        delete_node(tree, key);
    }

    clock_t end = clock();

    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Рекурсивный in-order обход
void b_tree_in_order_traverse(TreeNode *node) {
    if (node == NULL) return;

    // Обходим все ключи и дочерние узлы
    for (int i = 0; i < node->size; i++) {
        // Сначала левое поддерево (node_arr[i])
        if (node->node_arr[i] != NULL) {
            b_tree_in_order_traverse(node->node_arr[i]);
        }
    }

    // Последнее правое поддерево (node_arr[size])
    if (node->node_arr[node->size] != NULL) {
        b_tree_in_order_traverse(node->node_arr[node->size]);
    }
}

// Функция замера времени обхода
double measure_traversal(B_Tree *tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("Дерево пусто!\n");
        return 0.0;
    }

    clock_t start = clock();
    b_tree_in_order_traverse(tree->root);
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    return time_taken;
}

// Таймер поиска по префиксу
double measure_special_search(B_Tree *tree, int num_calls) {
    int found_pos_key = 0;
    char *found_info = NULL;

    clock_t start = clock();

    for (int i = 0; i < num_calls; i++) {
        u_int key = generate_random_key();
        search_node(tree, key, 1, &found_pos_key, &found_info);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

void print_menu() {
    printf("\nВыберите тип теста:\n");
    printf("1. Вставка\n");
    printf("2. Поиск\n");
    printf("3. Удаление\n");
    printf("4. Обход\n");
    printf("5. Специальный поиск\n");
    printf("6. Выход\n");
    printf("Ваш выбор: ");
}

void func_time() {
    srand((unsigned int) time(NULL));

    int choice = 0;
    const int test_count = 10000; // количество элементов для замера

    int sizes[] = {500000, 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000, 10000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    while (1) {
        print_menu();
        scanf("%d", &choice);

        if (choice == 6) break;

        printf("\nResults:\n");
        printf("Elements\tTime (s)\n");
        printf("---------------------------\n");

        for (int i = 0; i < num_sizes; i++) {
            int size = sizes[i];
            B_Tree *tree = create_tree();

            // === Заполняем дерево вне таймера ===
            prefill_tree(tree, size);

            double elapsed = 0.0;

            // === Выбираем, что таймировать ===
            switch (choice) {
                case 1:
                    elapsed = measure_insert(tree, test_count);
                    printf("%d\t\t%.6f\n", size, elapsed);
                    destroy_tree(tree);
                    break;

                case 2:
                    elapsed = measure_search(tree, test_count);
                    printf("%d\t\t%.6f\n", size, elapsed);
                    destroy_tree(tree);
                    break;

                case 3:
                    elapsed = measure_delete(tree, test_count);
                    printf("%d\t\t%.6f\n", size, elapsed);
                    destroy_tree(tree);
                    break;

                case 4:
                    elapsed = measure_traversal(tree);
                    printf("%d\t\t%.6f\n", size, elapsed);
                    destroy_tree(tree);
                    break;

                case 5:
                    elapsed = measure_special_search(tree, test_count);
                    printf("%d\t\t%.6f\n", size, elapsed);
                    destroy_tree(tree);
                    break;

                default:
                    printf("Неверный выбор.\n");
                    destroy_tree(tree);
                    break;
            }
        }

        printf("\n");
    }
}
