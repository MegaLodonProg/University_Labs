#include "../include/func_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* generate_random_key() {
    char* key = malloc(24);
    sprintf(key, "%d", rand() % 10000000);
    return key;
}

char* generate_info() {
    char* info = malloc(2);
    strcpy(info, "1");
    return info;
}

void prefill_tree(TreeNode** root, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        char* key = generate_random_key();
        char* info = generate_info();
        insert(root, key, info);
    }
}

double measure_insert(TreeNode** root, int num_elements) {
    clock_t start = clock();

    for (int i = 0; i < num_elements; i++) {
        char* key = generate_random_key();
        char* info = generate_info();
        insert(root, key, info);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

double measure_search(TreeNode* root, int num_calls) {
    clock_t start = clock();

    for (int i = 0; i < num_calls; i++) {
        char* key = generate_random_key();
        search((TreeNode*)root, key, 1);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

double measure_delete(TreeNode** root, int num_calls) {
    char** keys = malloc(num_calls * sizeof(char*));
    TreeNode* cur = find_max_node(*root);

    for (int i = 0; i < num_calls && cur != NULL; i++) {
        keys[i] = strdup(cur->key);
        cur = cur->prev;
    }

    clock_t start = clock();

    for (int i = 0; i < num_calls; i++) {
        if (keys[i]) {
            del_node(root, keys[i], 1);
        }
    }

    clock_t end = clock();

    for (int i = 0; i < num_calls; i++) {
        if (keys[i]) free(keys[i]);
    }
    free(keys);

    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Таймер обхода всех элементов через prev
double measure_traversal(TreeNode* root) {
    if (!root)
        return 0.0;

    clock_t start = clock();
    int count = 0;

    TreeNode* node = find_max_node((TreeNode*)root);
    TreeNode* last_visited = NULL;

    TreeNode* start_node = find_min_node((TreeNode*)root);
    int max_iterations = 2 * (size_t)root;

    while (node != NULL && count < max_iterations) {
        count++;

        if (node == last_visited) {
            printf("Обнаружен цикл в prev-прошивке\n");
            break;
        }

        last_visited = node;
        node = node->prev;
    }

    clock_t end = clock();

    printf("Пройдено %d узлов за %.6f секунд\n", count, ((double)(end - start)) / CLOCKS_PER_SEC);

    return (double)(end - start);
}

// Таймер поиска по префиксу
double measure_prefix_search(TreeNode* root, int num_calls) {
    clock_t total_time = 0;

    for (int i = 0; i < num_calls; i++) {
        char* prefix = generate_random_key();
        clock_t start = clock();

        TreeNode* node = find_max_node((TreeNode*)root);
        int count = 0;
        while (node != NULL) {
            if (strncmp(node->key, prefix, 2) == 0) {
                count++;
            }
            node = node->prev;
        }

        clock_t end = clock();
        total_time += (end - start);
        free(prefix);
    }

    return ((double)total_time) / CLOCKS_PER_SEC / num_calls;
}

void print_menu() {
    printf("\nВыберите тип теста:\n");
    printf("1. Вставка\n");
    printf("2. Поиск\n");
    printf("3. Удаление\n");
    printf("4. Обход (через prev)\n");
    printf("5. Поиск по префиксу\n");
    printf("6. Выход\n");
    printf("Ваш выбор: ");
}

void func_time() {
    srand((unsigned int) time(NULL));

    int choice = 0;
    const int test_count = 1000; // количество элементов для замера

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
            TreeNode* tree = NULL;

            // === Заполняем дерево вне таймера ===
            prefill_tree(&tree, size);

            double elapsed = 0.0;

            // === Выбираем, что таймировать ===
            switch (choice) {
                case 1:
                    elapsed = measure_insert(&tree, test_count);
                    printf("%d\t\t%.4f\n", size, elapsed);
                    free_tree(&tree);
                    break;

                case 2:
                    elapsed = measure_search(tree, test_count);
                    printf("%d\t\t%.4f\n", size, elapsed);
                    free_tree(&tree);
                    break;

                case 3:
                    elapsed = measure_delete(&tree, test_count);
                    printf("%d\t\t%.4f\n", size, elapsed);
                    free_tree(&tree);
                    break;

                case 4:
                    elapsed = measure_traversal(tree);
                    printf("%d\t\t%.4f\n", size, elapsed);
                    free_tree(&tree);
                    break;

                case 5:
                    elapsed = measure_prefix_search(tree, test_count);
                    printf("%d\t\t%.4f\n", size, elapsed);
                    free_tree(&tree);
                    break;

                default:
                    printf("Неверный выбор.\n");
                    free_tree(&tree);
                    break;
            }
        }

        printf("\n");
    }
}