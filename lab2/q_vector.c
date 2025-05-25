#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct Queue {
    Person *people;
    int head;
    int tail;
    int capacity; // всего памяти под одну очередь
    int size; // текущий размер очереди
};

int is_full(Queue *queue){
    return (queue->size == queue->capacity);
}

int is_empty(Queue *queue) {
    return (queue->size == 0);
}

int enqueue(Queue *queue, Person person){
    if (is_full(queue)) {
        printf("Очередь переполнена\n");
        printf("Увеличьте MAX_Q_LEN, для большего числа пассажиров\n");
        return ERR_MEM;
    }
    queue->people[queue->tail] = person;
    queue->tail++;
    queue->size++;
    return 0;
}

void dequeue(Queue *queue, Person *person){
    if (is_empty(queue)){
        fprintf(stderr, "Очередь пуста\n");
        return;
    }

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
}

void free_queue(Queue ***queue, int q_count){
    if (*queue == NULL) return;

    for (int i = 0; i < q_count; i++){
        if((*queue)[i]->people != NULL){
            free((*queue)[i]->people);
            (*queue)[i]->people = NULL; // зануление висячих указателей
        }
        free((*queue)[i]);
    }
    free(*queue);
    *queue = NULL;
}


void print_queue(Queue **queue, int q_count, int minute) {
    printf("Time: %d\n", minute);
    for (int i = 0; i < q_count; i++) {
        printf("#%d ", i + 1);
        for (int j = 0; j < queue[i]->size; j++) {
            int index = (queue[i]->head + j) % queue[i]->capacity;
            printf("%s ", queue[i]->people[index].id);
        }
        printf("\n");
    }
}

int init(Queue ***queue, int num_check_in){
    *queue = malloc(sizeof(Queue *) * num_check_in);
    if (*queue == NULL){
        return ERR_MEM;
    }

    for (int i = 0; i < num_check_in; i++){
        (*queue)[i] = calloc(sizeof(Queue), 1); // выделение памяти под каждую очередь
        if ((*queue)[i] == NULL){
            return ERR_MEM;
        }
        (*queue)[i]->people = calloc(sizeof(Person), MAX_Q_LEN); // память под массив людей
        if ((*queue)[i]->people == NULL){
            return ERR_MEM;
        }
        (*queue)[i]->capacity = MAX_Q_LEN; // инициализация capacity выбранным значением
    }

    return 0;

}

Person *get_person(Queue **queue, int num_q){
    int index = (queue)[num_q]->head;
    return &((queue)[num_q]->people[index]);
}

void time_leave(Queue **queue, int num_q, Person *person, int is_first){
    if (is_first){
        person->ts += person->ta;
    } else {
        int prev_person = ((queue)[num_q]->tail - 1);
        int max = ((queue)[num_q]->people[prev_person].ts > person->ta ? (queue)[num_q]->people[prev_person].ts : person->ta);
        person->ts += max;
    }
}
