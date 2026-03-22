#include <search.h>
#include "../include/io.h"
#include "readline/readline.h"


// IMPORTANT =================== CREATE FUNCS ========================

B_Tree* create_tree(){
    B_Tree *b_tree = calloc(1, sizeof(B_Tree));
    if (!b_tree) return NULL;

    b_tree->root = NULL; // Корень добавиться при первой вставке

    return b_tree;
}

TreeNode *create_node(TreeNode *parent){
    TreeNode *new_node = calloc(1, sizeof(TreeNode));
    if (!new_node) return NULL;

    new_node->parent = parent;

    return new_node;
}

ERR update_info_list(InfoList **head, char *info){
    InfoList *new_node = calloc(1, sizeof(InfoList));
    if (!new_node) return ERR_MEM;

    new_node->info = strdup(info);
    if (!new_node->info) {
        free(new_node);
        return ERR_MEM;
    }

    if (*head){
        new_node->release = (*head)->release + 1;
        new_node->next = (*head);
        (*head) = new_node;
    } else{
        new_node->release = 1;
        (*head) = new_node;
    }

    free(info);
    return ERR_OK;
}

// IMPORTANT =================== FREE FUNCS ===========================

void free_node(TreeNode *node){
    if (!node) return;
    for (int i = 0; i <= node->size; i++) {
        free_node(node->node_arr[i]);
    }

    for (int i = 0; i < node->size; i++){
        free_info_list(node->info_arr[i]);
    }

    node->parent = NULL;
    free(node);
}

void destroy_tree(B_Tree *tree){
    if (!tree || !tree->root) return;

    free_node(tree->root);
    tree->root = NULL;
}

// IMPORTANT =================== SEARCH FUNCS ===========================

TreeNode* search_node(const B_Tree *tree, u_int key, int release, int *key_pos, char **found_inf){
    if (!tree || !tree->root){
        return NULL;
    }

    TreeNode *cur = tree->root;

    while (cur) {
        int pos = 0;
        // пробегаемся по текущему узлу, чтобы знать куда свернуть
        while (pos < cur->size && key > cur->keys_arr[pos]) {
            pos++;
        }

        if (pos < cur->size && key == cur->keys_arr[pos]) {
            if (release == 0){
                *key_pos = pos;
                *found_inf = NULL;
                return cur;
            }
            InfoList *cur_rel = cur->info_arr[pos];
            if (cur_rel->release < release) return NULL;

            while (cur_rel){
                if (cur_rel->release == release) {
                    *key_pos = pos;
                    *found_inf = strdup(cur_rel->info);
                    return cur;
                }
                cur_rel = cur_rel->next;
            }

        }

        // Переходим к соответствующему поддереву УЧИТЫВААЯ ЧТО ИХ 4 может быть
        cur = cur->node_arr[pos];
    }

    return NULL;
}

ERR special_search_node(const B_Tree *tree, u_int key, int release, u_int *found_key, char **found_inf){
    if (!tree || !tree->root || release < 1){
        return ERR_NOT_FOUND;
    }

    u_int last_key;
    InfoList *info_list;

    TreeNode *cur = tree->root;

    while (cur) {
        int pos = 0;
        // пробегаемся по текущему узлу, чтобы знать куда свернуть
        while (pos < cur->size && key > cur->keys_arr[pos]) {
            last_key = cur->keys_arr[pos];
            info_list = cur->info_arr[pos];
            pos++;
        }

        if (pos < cur->size && key == cur->keys_arr[pos]) {

            InfoList *cur_rel = cur->info_arr[pos];
            if (cur_rel->release < release) return ERR_NOT_FOUND;

            while (cur_rel){
                if (cur_rel->release == release) {
                    *found_key = key;
                    *found_inf = strdup(cur_rel->info);
                    return ERR_OK;
                }
                cur_rel = cur_rel->next;
            }

        }

        cur = cur->node_arr[pos];
    }

    if (key < last_key && info_list->release < release) return ERR_NOT_FOUND;

    while (info_list){
        if (info_list->release == release) {
            *found_key = last_key;
            *found_inf = strdup(info_list->info);
            return ERR_OK;
        }
        info_list = info_list->next;
    }

    return ERR_NOT_FOUND;
}


// IMPORTANT =================== FOR INSERT FUNC =========================

void tree_split(TreeNode* parent, int child_pos){
    // Забираем соответствующую ноду для разбиения по позиции из родительского узла
    TreeNode *child = parent->node_arr[child_pos];
    // Соседний узел для child_split, а parent потому что медианную ноду подняли туда - теперь он родитель для new
    TreeNode *new_node = create_node(parent);

    // Определяем медиану (второй ключ в отсортированном массиве)
    int median_pos = 1;
    u_int median_key = child->keys_arr[median_pos];

    // Переносим последний ключ и данные
    new_node->keys_arr[0] = child->keys_arr[2];
    new_node->info_arr[0] = child->info_arr[2];
    new_node->size = 1;

    // Переносим детей с индексом 2 и 3 тк для самого правого ключа
    if (child->node_arr[2]) {
        new_node->node_arr[0] = child->node_arr[2];
        new_node->node_arr[0]->parent = new_node;
    }
    if (child->node_arr[3]) {
        new_node->node_arr[1] = child->node_arr[3];
        new_node->node_arr[1]->parent = new_node;
    }

    // Сдвигаем ключи родителя, чтобы освободить нужное место не нарушая порядок
    for (int i = parent->size; i > child_pos; i--) {
        parent->keys_arr[i] = parent->keys_arr[i - 1];
        parent->info_arr[i] = parent->info_arr[i - 1];
        parent->node_arr[i + 1] = parent->node_arr[i];
    }

    // Вставляем средний ключ в родителя
    parent->keys_arr[child_pos] = median_key;
    parent->info_arr[child_pos] = child->info_arr[median_pos];
    parent->node_arr[child_pos + 1] = new_node;
    parent->size++;

    child->size = 1;

    child->keys_arr[1] = 0;
    child->keys_arr[2] = 0;
    child->info_arr[1] = NULL;
    child->info_arr[2] = NULL;
    child->node_arr[2] = NULL;
    child->node_arr[3] = NULL;
}

int find_insert_position(TreeNode* node, u_int key) {
    int pos = 0;
    while (pos < node->size && key > node->keys_arr[pos]) {
        pos++;
    }
    return pos;
}

ERR insert_into_leaf(TreeNode* leaf, u_int key, char* info) {
    int pos = find_insert_position(leaf, key);

    // cдвиг элементов вправо для освобождения места
    for (int i = leaf->size; i > pos; i--) {
        leaf->keys_arr[i] = leaf->keys_arr[i-1];
        leaf->info_arr[i] = leaf->info_arr[i-1];
        leaf->info_arr[i - 1] = NULL;
    }

    leaf->keys_arr[pos] = key;
    update_info_list(&(leaf->info_arr[pos]), info);
    leaf->size++;

    return ERR_OK;
}

ERR insert_node(B_Tree *tree, u_int key, char *info){
    if (!tree || !info){
        return ERR_MEM;
    }

    // Случай пустого дерева
    if (!tree->root) {
        tree->root = create_node(NULL);
        tree->root->keys_arr[0] = key;
        update_info_list(&(tree->root->info_arr[0]), info);
        tree->root->size = 1;
        return ERR_OK;
    }

    TreeNode* parent = NULL;
    TreeNode* current = tree->root;
    int pos;

    // Поиск места для вставки с разделением узлов
    while (current) {
        // Разделяем переполненные узлы на пути вниз
        if (current->size == 3) {
            // вдруг уже будет 3 элемента, но захотим вставить повторный - тогда разбивать ещё не надо!
            pos = find_insert_position(current, key);
            if (pos < 3 && current->keys_arr[pos] == key){
                update_info_list(&(current->info_arr[pos]), info);
                return ERR_OK;
            }

            if (!parent) {
                // Разделяем корень
                TreeNode* new_root = create_node(NULL);
                new_root->node_arr[0] = current;
                tree->root = new_root;
                tree_split(new_root, 0);
                parent = new_root;
                pos = 0;
            } else {
                pos = find_insert_position(parent, key);
                tree_split(parent, pos);
                pos = find_insert_position(parent, key);
                current = parent->node_arr[pos];
            }
        }

        // нашли первую развилку, где пора сворачивать (key <=)
        pos = find_insert_position(current, key);
        if (current->keys_arr[pos] == key){
            update_info_list(&(current->info_arr[pos]), info);
            return ERR_OK;
        }
        parent = current;
        current = parent->node_arr[pos];
    }

    // Вставка в лист
    return insert_into_leaf(parent, key, info);
}

// IMPORTANT =================== FOR DELETE FUNCS ===========================

ERR delete_node(B_Tree *tree, u_int key) {
    if (tree->root == NULL) return ERR_MEM;

    int key_pos = 0;
    char *info = NULL;

    TreeNode* del_node = search_node(tree, key, 0, &key_pos, &info);
    if (!del_node) return ERR_NOT_FOUND;

    TreeNode *current = tree->root;
    handle_delete_internal_node(tree, current, key);

    if (tree->root->size == 0) {
        TreeNode *old_root = tree->root;
        if (old_root->node_arr[0] != NULL) {
            tree->root = old_root->node_arr[0];
            tree->root->parent = NULL;
        } else {
            tree->root = NULL;
        }
        free(old_root);
    }
    return ERR_OK;
}

static InfoList* remove_oldest_info(InfoList **head) {
    if (*head == NULL) return NULL;

    InfoList *current = *head;
    InfoList *prev = NULL;

    // Находим самую старую версию
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = NULL;
    }

    free(current->info);
    free(current);
    return *head;
}



// Рекурсивная обработка удаления
static void handle_delete_internal_node(B_Tree *tree, TreeNode *node, unsigned int key) {
    if (node == NULL) return;

    int i;
    for (i = 0; i < node->size; i++) {
        if (node->keys_arr[i] == key) break;
    }

    if (i < node->size) {
        if (is_leaf(node)) {
            // Удаление из листа
            remove_key_from_leaf(tree, node, i);
        } else {
            if (node->info_arr[i] != NULL) {
                // Удаляем самую старую версию
                InfoList *new_head = remove_oldest_info(&node->info_arr[i]);
                if (new_head != NULL) {
                    node->info_arr[i] = new_head;
                    return;
                }
            }

            TreeNode *left_child = node->node_arr[i];
            TreeNode *right_child = node->node_arr[i + 1];

            // Случай 2.1: Замена на максимум из левого поддерева
            if (left_child != NULL && left_child->size >= 2) {
                TreeNode *predecessor = get_max_node(left_child);
                unsigned int pred_key = predecessor->keys_arr[predecessor->size - 1];
                InfoList *pred_info = predecessor->info_arr[predecessor->size - 1];

                free_info_list(node->info_arr[i]);

                node->keys_arr[i] = pred_key;
                node->info_arr[i] = pred_info;
                predecessor->info_arr[predecessor->size - 1] = NULL;

                // Рекурсивное удаление в левом поддереве
                handle_delete_internal_node(tree, left_child, pred_key);

                // Случай 2.2: Замена на минимум из правого поддерева
            } else if (right_child != NULL && right_child->size >= 2) {
                TreeNode *successor = get_min_node(right_child);
                unsigned int succ_key = successor->keys_arr[0];
                InfoList *succ_info = successor->info_arr[0];

                free_info_list(node->info_arr[i]);

                node->keys_arr[i] = succ_key;
                node->info_arr[i] = succ_info;
                successor->info_arr[0] = NULL;

                for (int j = 0; j < successor->size - 1; j++) {
                    successor->keys_arr[j] = successor->keys_arr[j + 1];
                    successor->info_arr[j] = successor->info_arr[j + 1];
                }
                successor->size--;

                // Рекурсивное удаление в правом поддереве
                handle_delete_internal_node(tree, right_child, succ_key);

                // Случай 2.3: Слияние детей
            } else {
                merge_with_left(tree, node, i);
                handle_delete_internal_node(tree, node->node_arr[i], key);
            }
        }
    } else {
        int child_pos = find_position(node, key);

        if (child_pos < 0 || child_pos > node->size) return;

        TreeNode *child = node->node_arr[child_pos];
        if (child == NULL) return;

        if (child->size < 2) {
            fix_child(tree, node, child_pos);
            child_pos = find_position(node, key);
            child = node->node_arr[child_pos];
            if (child == NULL) return;
        }

        handle_delete_internal_node(tree, child, key);
    }
}

static int find_position(TreeNode *node, unsigned int key) {
    int pos = 0;
    while (pos < node->size && key > node->keys_arr[pos]) pos++;
    return pos;
}

static bool is_leaf(TreeNode *node) {
    for (int i = 0; i < 4; i++)
        if (node->node_arr[i] != NULL) return false;
    return true;
}

static void free_info_list(InfoList *list) {
    while (list != NULL) {
        InfoList *temp = list;
        list = list->next;
        free(temp->info);
        free(temp);
    }
}

static void remove_key_from_leaf(B_Tree *tree, TreeNode *leaf, int pos) {
    if (leaf == NULL || pos < 0 || pos >= leaf->size) return;

    // Удаляем самую старую версию
    InfoList *new_head = remove_oldest_info(&leaf->info_arr[pos]);

    if (new_head != NULL) {
        leaf->info_arr[pos] = new_head;
        return;
    }

    free_info_list(leaf->info_arr[pos]);

    for (int i = pos; i < leaf->size - 1; i++) {
        leaf->keys_arr[i] = leaf->keys_arr[i+1];
        leaf->info_arr[i] = leaf->info_arr[i+1];
    }
    leaf->size--;

    leaf->keys_arr[leaf->size] = 0;
    leaf->info_arr[leaf->size] = NULL;

    // Удаление пустого узла
    if (leaf->size == 0 && leaf->parent != NULL) {
        TreeNode *parent = leaf->parent;
        for (int i = 0; i <= parent->size; i++) {
            if (parent->node_arr[i] == leaf) {
                parent->node_arr[i] = NULL;
                break;
            }
        }
        free(leaf);
    }
}

static void borrow_from_left(TreeNode *parent, int pos) {
    TreeNode *child = parent->node_arr[pos];
    TreeNode *left_sib = parent->node_arr[pos-1];

    // Сдвиг ключей ребенка вправо
    for (int i = child->size; i > 0; i--) {
        child->keys_arr[i] = child->keys_arr[i-1];
        child->info_arr[i] = child->info_arr[i-1];
    }

    // Перенос ключа из родителя
    child->keys_arr[0] = parent->keys_arr[pos-1];
    child->info_arr[0] = parent->info_arr[pos-1];

    // Обновление родителя
    parent->keys_arr[pos-1] = left_sib->keys_arr[left_sib->size-1];
    parent->info_arr[pos-1] = left_sib->info_arr[left_sib->size-1];

    // Перенос детей
    if (!is_leaf(child)) {
        for (int i = child->size+1; i > 0; i--)
            child->node_arr[i] = child->node_arr[i-1];
        child->node_arr[0] = left_sib->node_arr[left_sib->size];
    }

    left_sib->size--;
    child->size++;
}

static void borrow_from_right(TreeNode *parent, int pos) {
    TreeNode *child = parent->node_arr[pos];
    TreeNode *right_sib = parent->node_arr[pos+1];

    // Перенос ключа из родителя
    child->keys_arr[child->size] = parent->keys_arr[pos];
    child->info_arr[child->size] = parent->info_arr[pos];

    // Обновление родителя
    parent->keys_arr[pos] = right_sib->keys_arr[0];
    parent->info_arr[pos] = right_sib->info_arr[0];

    // Сдвиг ключей в правом брате
    for (int i = 0; i < right_sib->size-1; i++) {
        right_sib->keys_arr[i] = right_sib->keys_arr[i+1];
        right_sib->info_arr[i] = right_sib->info_arr[i+1];
    }

    // Перенос детей
    if (!is_leaf(child)) {
        child->node_arr[child->size+1] = right_sib->node_arr[0];
        for (int i = 0; i < right_sib->size; i++)
            right_sib->node_arr[i] = right_sib->node_arr[i+1];
    }

    right_sib->size--;
    child->size++;
}

static void merge_with_left(B_Tree *tree, TreeNode *parent, int pos) {
    TreeNode *left = parent->node_arr[pos-1];
    TreeNode *right = parent->node_arr[pos];

    left->keys_arr[left->size] = parent->keys_arr[pos-1];
    left->info_arr[left->size] = parent->info_arr[pos-1];
    left->size++;

    for (int i = 0; i < right->size; i++) {
        left->keys_arr[left->size+i] = right->keys_arr[i];
        left->info_arr[left->size+i] = right->info_arr[i];
    }

    if (!is_leaf(right)) {
        for (int i = 0; i <= right->size; i++) {
            left->node_arr[left->size+i] = right->node_arr[i];
            if (left->node_arr[left->size+i])
                left->node_arr[left->size+i]->parent = left;
        }
    }

    left->size += right->size;
    free(right);

    for (int i = pos-1; i < parent->size-1; i++) {
        parent->keys_arr[i] = parent->keys_arr[i+1];
        parent->info_arr[i] = parent->info_arr[i+1];
        parent->node_arr[i+1] = parent->node_arr[i+2];
    }
    parent->size--;
    parent->node_arr[pos-1] = left;

    // Обработка пустого корня
    if (parent->size == 0 && parent == tree->root) {
        tree->root = left;
        left->parent = NULL;
        free(parent);
    }
}

static void fix_child(B_Tree *tree, TreeNode *parent, int pos) {
    // Проверка левого брата
    if (pos > 0 && parent->node_arr[pos-1] != NULL &&
        parent->node_arr[pos-1]->size >= 2) {
        borrow_from_left(parent, pos);
    }
        // Проверка правого брата
    else if (pos < parent->size && parent->node_arr[pos+1] != NULL &&
             parent->node_arr[pos+1]->size >= 2) {
        borrow_from_right(parent, pos);
    }
    else {
        if (pos > 0) {
            merge_with_left(tree, parent, pos);
        } else {
            merge_with_left(tree, parent, pos+1);
        }
    }
}

static TreeNode *get_max_node(TreeNode *node) {
    while (!is_leaf(node)) {
        node = node->node_arr[node->size];
    }
    return node;
}

static TreeNode *get_min_node(TreeNode *node) {
    while (!is_leaf(node)) {
        node = node->node_arr[0];
    }
    return node;
}



// IMPORTANT =================== IMPORT-EXPORT FUNCS ===========================

ERR tree_import(B_Tree *dest_tree, const char* filename) {
    if (dest_tree == NULL || filename == NULL) {
        return ERR_MEM;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return ERR_OPEN;
    }

    // Создаем временное дерево
    B_Tree *import_tree = create_tree();
    ERR status = ERR_OK;

    u_int read_key = 0;
    char* key_str = NULL;
    char* read_info = NULL;

    while ((key_str = readline_file(file)) != NULL && (read_info = readline_file(file)) != NULL) {
        if (!read_info) {
            status = ERR_FORMAT;
            break;
        }

        sscanf(key_str, "%u", &read_key);
        free(key_str);
        ERR insert_status = insert_node(import_tree, read_key, read_info);
        if (insert_status != ERR_OK) {
            free(read_info);
            status = insert_status;
            break;
        }
    }

    if (status != ERR_OK) {
        destroy_tree(import_tree);
        free(import_tree);
        return status;
    }

    destroy_tree(dest_tree);
    dest_tree->root = import_tree->root;

    free(import_tree);
    fclose(file);

    return ERR_OK;
}

void export_node_to_dot(TreeNode* node, FILE* f) {
    if (!node) return;

    // Создаем label с ключами и информацией
    fprintf(f, "    \"%p\" [label=\"", (void*)node);

    // Формируем таблицу с ключами и информацией
    fprintf(f, "<f0> ");  // Первый дочерний узел
    for (int i = 0; i < node->size; i++) {
        fprintf(f, "|<f%d> %u\\n", i+1, node->keys_arr[i]);
        InfoList* cur = node->info_arr[i];
        while (cur) {
            fprintf(f, "\\nv%d: %s", cur->release, cur->info);
            cur = cur->next;
        }
        fprintf(f, "|<f%d> ", i+node->size+1);
    }
    fprintf(f, "\", shape=Mrecord, style=filled, fillcolor=lightblue];\n");

    // Рекурсивно обрабатываем дочерние узлы
    for (int i = 0; i <= node->size; i++) {
        if (node->node_arr[i]) {
            fprintf(f, "    \"%p\":f%d -> \"%p\";\n",
                    (void*)node, i, (void*)node->node_arr[i]);
            export_node_to_dot(node->node_arr[i], f);
        }
    }
}

void visualize_b_tree(B_Tree* tree, const char* filename) {
    FILE* f = fopen("b_tree.dot", "w");
    if (!f) {
        perror("Invalid crete file");
        return;
    }

    fprintf(f, "digraph BTree {\n");
    fprintf(f, "    node [fontname=\"Arial\"];\n");
    fprintf(f, "    edge [arrowhead=vee];\n");
    fprintf(f, "    graph [nodesep=0.5, ranksep=0.5];\n");

    if (tree->root) {
        export_node_to_dot(tree->root, f);
    } else {
        fprintf(f, "    \"empty\" [label=\"B-tree is empty\", shape=box];\n");
    }

    fprintf(f, "}\n");
    fclose(f);

    // Генерация PNG (требуется Graphviz)
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng b_tree.dot -o %s && rm b_tree.dot", filename);

    if (system(command) == 0) {
        printf("Phg save at в %s\n", filename);
    } else {
        printf("Error: install Graphviz (sudo apt-get install graphviz)\n");
    }
}


// IMPORTANT =================== PRINT FUNCS ===========================

void print_tree_node(const TreeNode *node, int level, int child_pos) {
    if (node == NULL) {
        return;
    }

    print_tree_node(node->node_arr[3], level + 1, 3);

    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    switch (child_pos) {
        case 0: printf("  └── "); break;
        case 1: printf("  ├── "); break;
        case 2: printf("  ├── "); break;
        case 3: printf("  ┌── "); break;
        default: printf("─── "); break;
    }

    // Выводим ключи текущего узла
    printf(BLUE "[");
    for (int i = 0; i < node->size; i++) {
        printf("%u", node->keys_arr[i]);
        if (i < node->size - 1) printf(", ");
    }
    printf("]" RESET);

    if (node->size > 0){
        printf("(");
        for (int i = 0; i < node->size; i++){
            if (node->info_arr[i]){
                InfoList *cur_node = (InfoList *)node->info_arr[i];
                printf("| ");
                while (cur_node){
                    printf(GREEN "%s " RESET, cur_node->info);
                    cur_node = cur_node->next;
                }
                printf("|");
            }
        }
        printf(")\n");
    }

    // Рекурсивно обрабатываем все дочерние узлы
    print_tree_node(node->node_arr[2], level + 1, 2);
    print_tree_node(node->node_arr[1], level + 1, 1);
    print_tree_node(node->node_arr[0], level + 1, 0);
}

void print_tree(const B_Tree *tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("Tree is empty!\n");
        return;
    }

    print_tree_node(tree->root, 0, -1);
}
