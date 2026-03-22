#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct Item{ // звено, состоящее из пас + указатель на следующего пас
    Person person;
    struct Item *next;
} Item;

struct Queue { // состоит из звеньев
    Item *head; // первое звено
    Item *tail; // последнее
    int size; // текущий размер очереди
};

int is_full(Queue *queue) {
    return 0; // Всегда 0 тк динамическая память
}

int is_empty(Queue *queue){
    return (queue->size == 0);
}

int enqueue(Queue *queue, Person person) {
    Item *new_item = malloc(sizeof(Item)); // выделение памяти под новое звено
    if (new_item == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return ERR_MEM;
    }
    new_item->person = person;
    new_item->next = NULL;

    if (is_empty(queue)) {
        queue->head = new_item;
        queue->tail = new_item;
    } else {
        queue->tail->next = new_item; // сначала переставляем next на новое звено, пока tail ещё на предыдущее указывает
        queue->tail = new_item;
    }
    queue->size++;
    return 0;
}

void dequeue(Queue *queue, Person *person) {
    if (is_empty(queue)) {
        fprintf(stderr, "Очередь пуста\n");
        return; // никогда не будет такого, не вручную делаем
    }
    Item *temp = queue->head; // забираем головное звено, чтобы при переходе не потерять указатель и очистить её
    queue->head = queue->head->next; // переход к следующему звену
    free(temp);
    queue->size--;

    if (queue->head == NULL) {
        queue->tail = NULL; // tail может указывать на удалённый узел, поэтому надо занулить
    }
}

void free_queue(Queue ***queue, int q_count) {
    if (*queue == NULL) return;

    for (int i = 0; i < q_count; i++) {
        Item *current = (*queue)[i]->head;
        while (current != NULL) { // идём по звеньям, пок ане дойдём до конца очереди
            Item *temp = current;
            current = current->next;
            free(temp);
        }
        free((*queue)[i]); // указатель на саму очередь
    }
    free(*queue);
    *queue = NULL;
}

void print_queue(Queue **queue, int q_count, int minute) {
    printf("Time: %d\n", minute);
    for (int i = 0; i < q_count; i++) {
        printf("#%d ", i + 1);
        Item *current = queue[i]->head;
        while (current != NULL) {
            printf("%s ", current->person.id);
            current = current->next;
        }
        printf("\n");
    }
}



int init(Queue ***queue, int num_check_in) {
    *queue = malloc(sizeof(Queue *) * num_check_in);
    if (*queue == NULL) {
        return ERR_MEM;
    }

    for (int i = 0; i < num_check_in; i++) {
        (*queue)[i] = calloc(sizeof(Queue), 1); // под звенья выделяем уже при добавлении
        if ((*queue)[i] == NULL) {
            free_queue(queue, num_check_in);
            return ERR_MEM;
        }
    }
    return 0;
}

Person *get_person(Queue **queue, int num_q){ // получение пас из очереди
    return &((queue)[num_q]->head->person);
}

void time_leave(Queue **queue, int num_q, Person *person, int is_first){ // настройка времени покидания очереди
    if (is_first){
        person->ts += person->ta;
    } else {
        Person prev_person = (queue)[num_q]->tail->person;
        int max = (prev_person.ts > person->ta ? prev_person.ts : person->ta);
        person->ts += max;
    }

}
