#include "../include/io.h"
#include "readline/readline.h"


TreeNode *dialog_search(const TreeNode *root, ERR *error){
    char *key;
    int release;
    key = readline("Key: ");
    if (!key) {
        *error = ERR_MEM;
        return NULL;
    }

    printf("Release: ");
    if (correct_int(&release) == ERR_EOF){
        free(key);
        *error = ERR_EXIT;
        return NULL;
    }

    *error = ERR_OK;
    // Теперь search ответственна за очистку key
    return search(root, key, release);
}

TreeNode* search(const TreeNode* root, char* key, int release) {
    const TreeNode* cur = root;
    int count = 0; // Счётчик найденных версий

    while (cur != NULL) {
        int cmp = strcmp(key, cur->key);

        // Если текущий ключ больше (>) искомого - идём влево, в противном случае (<=) - вправо
        if (cmp < 0) {
            cur = cur->left;
        } else if (cmp > 0) {
            cur = cur->right;
        } else {
            // совпадение по ключу
            count++;

            // Если это нужная версия - возвращаем
            if (count == release) {
                free(key);
                return (TreeNode*)cur;
            }

            // Пока нужная версия не найдена углубляемся влево (<=)
            cur = cur->left;
        }

    }

    free(key);
    return NULL;
}

TreeNode* find_min_node(const TreeNode* root) {
    while (root && root->left) {
        root = root->left;
    }
    return (TreeNode*)root;
}

TreeNode* find_max_node(const TreeNode* root) {
    while (root && root->right) {
        root = root->right;
    }
    return (TreeNode*)root;
}

// Сравнение двух ключей и возврат узла с максимальным отличием от target_key
TreeNode* special_search(const TreeNode* root, const char* target_key) {
    if (!root) return NULL;

    TreeNode* min_node = find_min_node(root);  // Самый левый узел
    TreeNode* max_node = find_max_node(root);  // Самый правый узел

    int dist_min = abs(strcmp(target_key, min_node->key));
    int dist_max = abs(strcmp(target_key, max_node->key));

    return (dist_min > dist_max) ? min_node : max_node;
}




ERR dialog_insert(TreeNode **root_ptr){
    char *key;
    char *info;
    key = readline("Key: ");
    info = readline("Info: ");
    if (!key || !info) {
        free(key);
        free(info);
        return ERR_MEM;
    }

    return insert(root_ptr, key, info);
}

ERR insert(TreeNode** root_ptr, char* key, char* info) {
    // Создаем новый узел
    TreeNode* new_node = calloc(1, sizeof(TreeNode));
    if (!new_node) return ERR_MEM;

    new_node->key = key;
    new_node->info = info;

    // Если дерево пустое
    if (*root_ptr == NULL) {
        *root_ptr = new_node;
        return ERR_OK;
    }

    // Поиск места для вставки
    TreeNode* cur = *root_ptr;
    TreeNode* parent = NULL;
    // Чтоб понять это будет левый или правый ребёнок родителя
    int is_left_child = 0;

    while (cur != NULL) {
        parent = cur;
        int cmp = strcmp(key, cur->key);

        if (cmp <= 0) {  // Дубликаты идут влево
            cur = cur->left;
            is_left_child = 1;
        } else {
            cur = cur->right;
            is_left_child = 0;
        }
    }

    // Вставляем узел
    new_node->parent = parent;
    if (is_left_child) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    // Обновляем прошивку
    update_threads(*root_ptr);

    return ERR_OK;
}

void update_threads(TreeNode* root) {
    if (root == NULL) return;

    TreeNode* cur = find_max_node(root);
    TreeNode* prev_node = NULL;

    TreeNode* node_to_clear = cur;
    while (node_to_clear != NULL) {
        node_to_clear->prev = NULL;
        if (node_to_clear->left) {
            TreeNode* next_right = node_to_clear->left;
            while (next_right->right) {
                next_right = next_right->right;
            }
            node_to_clear = next_right;
        } else {
            TreeNode* parent = node_to_clear->parent;
            while (parent && node_to_clear == parent->left) {
                node_to_clear = parent;
                parent = parent->parent;
            }
            node_to_clear = parent;
        }
    }

    // Теперь строим новую цепочку prev
    cur = find_max_node(root);
    prev_node = NULL;

    while (cur != NULL) {
        if (prev_node != NULL) {
            prev_node->prev = cur;
        }
        prev_node = cur;

        // Переходим к следующему узлу в обратном порядке
        if (cur->left != NULL) {
            cur = cur->left;
            while (cur->right != NULL) {
                cur = cur->right;
            }
        } else {
            TreeNode* parent = cur->parent;
            while (parent != NULL && cur == parent->left) {
                cur = parent;
                parent = parent->parent;
            }
            cur = parent;
        }
    }
}




ERR dialog_del_node(TreeNode **root){
    char *key;
    int release;
    key = readline("Key: ");
    if (!key) {
        return ERR_MEM;
    }

    printf("Release: ");
    if (correct_int(&release) == ERR_EOF){
        free(key);
        return ERR_EXIT;
    }
    // Теперь del_node ответственна за очистку key!
    return del_node(root, key, release);
}

ERR del_node(TreeNode** root_ptr, char* key, int release) {
    if (root_ptr == NULL || *root_ptr == NULL || key == NULL) {
        free(key);
        return ERR_NOT_FOUND;
    }

    TreeNode* target = NULL;
    int count = 0;

    // Находим нужный узел для удаления
    TreeNode* cur = find_max_node(*root_ptr);

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            count++;
            if (count == release) {
                target = cur;
                break;
            }
        }
        cur = cur->prev;
    }

    if (target == NULL) {
        free(key);
        return ERR_NOT_FOUND;
    }

    // Случай 1: Узел - лист
    if (target->left == NULL && target->right == NULL) {
        if (target->parent == NULL) {
            *root_ptr = NULL;
        } else if (target->parent->left == target) {
            target->parent->left = NULL;
        } else {
            target->parent->right = NULL;
        }
        free(target->key);
        free(target->info);
        free(target);
    }
        // Случай 2: Узел имеет только левое поддерево
    else if (target->right == NULL) {
        if (target->parent == NULL) {
            *root_ptr = target->left;
            target->left->parent = NULL;
        } else if (target->parent->left == target) {
            target->parent->left = target->left;
            target->left->parent = target->parent;
        } else {
            target->parent->right = target->left;
            target->left->parent = target->parent;
        }

        free(target->key);
        free(target->info);
        free(target);
    }
        // Случай 3: Узел имеет только правое поддерево
    else if (target->left == NULL) {
        if (target->parent == NULL) {
            *root_ptr = target->right;
            target->right->parent = NULL;
        } else if (target->parent->left == target) {
            target->parent->left = target->right;
            target->right->parent = target->parent;
        } else {
            target->parent->right = target->right;
            target->right->parent = target->parent;
        }

        free(target->key);
        free(target->info);
        free(target);
    }
        // Случай 4: Узел имеет оба поддерева
    else {
        TreeNode* max_left = find_max_node(target->left);

        // Освобождаем старые данные перед заменой
        free(target->key);
        free(target->info);

        target->key = strdup(max_left->key);
        if (target->key == NULL) {
            free(key);
            return ERR_MEM;
        }

        target->info = strdup(max_left->info);
        if (target->info == NULL) {
            free(target->key);
            free(key);
            return ERR_MEM;
        }
    }

    update_threads(*root_ptr);
    free(key);
    return ERR_OK;
}


void print_tree(const TreeNode *node, int level, int is_left) {
    if (node == NULL) {
        return;
    }

    // Вывод правого поддерева (верхняя часть)
    print_tree(node->right, level + 1, 0);

    // Отрисовка отступов и ветвей
    for (int i = 0; i < level - 1; i++) {
        printf("    ");
    }

    if (level > 0) {
        printf(is_left ? " └── " : " ┌── ");
    }

    // Вывод узла с цветами
    printf(BLUE "%s" RESET ":" GREEN "%s" RESET "\n", node->key, node->info);

    // Вывод левого поддерева (нижняя часть)
    print_tree(node->left, level + 1, 1);
}

void print_nodes_with_prefix(TreeNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL) {
        printf("Tree is empty!\n");
        return;
    }

    int prefix_len = strlen(prefix);
    TreeNode* current = find_max_node(root);
    int count = 0;

    while (current != NULL) {
        // Проверяем, начинается ли ключ с заданной подстроки
        if (strncmp(current->key, prefix, prefix_len) == 0) {
            count++;
            printf("%d. Key:" BLUE "%-10s" RESET "Value:" GREEN "%s\n" RESET, count, current->key, current->info);
        }

        // Переходим к предыдущему узлу в порядке прошивки
        current = current->prev;
    }

    if (count == 0) {
        printf("Data not found!\n");
    }
}

void export_to_dot(TreeNode* node, FILE* f) {
    if (!node) return;

    // Уникальный идентификатор узла (адрес в памяти)
    fprintf(f, "    \"%p\" [label=\"<left> | <value> %s\\n%s | <right>\"];\n",
            (void*)node, node->key, node->info);

    if (node->left) {
        fprintf(f, "    \"%p\":left -> \"%p\":value;\n", (void*)node, (void*)node->left);
        export_to_dot(node->left, f);
    }

    if (node->right) {
        fprintf(f, "    \"%p\":right -> \"%p\":value;\n", (void*)node, (void*)node->right);
        export_to_dot(node->right, f);
    }

    // Добавляем связи прошивки (пунктирные линии)
    if (node->prev) {
        fprintf(f, "    \"%p\" -> \"%p\" [style=dashed, color=blue, constraint=false];\n",
                (void*)node, (void*)node->prev);
    }
}

void visualize_tree(TreeNode* root, const char* filename) {
    FILE* f = fopen("tree.dot", "w");
    if (!f) {
        perror("Ошибка создания файла");
        return;
    }

    fprintf(f, "digraph BST {\n");
    fprintf(f, "    node [fontname=\"Arial\", shape=Mrecord, style=filled, fillcolor=lightblue];\n");
    fprintf(f, "    graph [nodesep=0.5, ranksep=0.5];\n");

    if (root) {
        export_to_dot(root, f);
    } else {
        fprintf(f, "    \"empty\" [label=\"Дерево пустое\", shape=box];\n");
    }

    fprintf(f, "}\n");
    fclose(f);

    // Генерация изображения (требуется установленный Graphviz)
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng tree.dot -o %s && rm tree.dot", filename);
    int result = system(command);

    if (result == 0) {
        printf("Графическое представление сохранено в %s\n", filename);
    } else {
        printf("Ошибка генерации изображения. Установите Graphviz.\n");
    }
}

ERR tree_import(TreeNode** dest_tree, const char* filename) {
    if (dest_tree == NULL || filename == NULL) {
        return ERR_MEM;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        return ERR_OPEN;
    }

    // Создаем временное дерево
    TreeNode* import_tree = NULL;
    ERR status = ERR_OK;

    char* read_key = NULL;
    char* read_info = NULL;

    while ((read_key = readline_file(file)) != NULL && (read_info = readline_file(file)) != NULL) {
        if (!read_info) {
            free(read_key);
            status = ERR_FORMAT;
            break;
        }

        ERR insert_status = insert(&import_tree, read_key, read_info);
        if (insert_status != ERR_OK) {
            free(read_key);
            free(read_info);
            status = insert_status;
            break;
        }
    }

    if (status != ERR_OK) {
        free_tree(&import_tree);
        return status;
    }

    // Удаляем старое дерево и заменяем новым + настраиваем прошивку
    free_tree(dest_tree);
    *dest_tree = import_tree;
    update_threads(*dest_tree);

    fclose(file);

    return ERR_OK;
}

void free_tree(TreeNode** root_ptr) {
    if (root_ptr == NULL || *root_ptr == NULL) {
        return;
    }

    TreeNode* current = find_max_node(*root_ptr);

    TreeNode* prev_node;
    while (current != NULL) {
        prev_node = current->prev;

        current->left = NULL;
        current->right = NULL;
        current->parent = NULL;
        current->prev = NULL;

        free(current->key);
        free(current->info);
        free(current);

        current = prev_node;
    }

    *root_ptr = NULL;
}
