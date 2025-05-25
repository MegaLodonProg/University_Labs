#ifndef QUEUE_H
#define QUEUE_H

#define MAX_LEN_ID 10 // размер строки id
#define MAX_Q_LEN 5 // можно менять, чтобы определить объём памяти для вектора

typedef enum {
    ERR_EOF = -1,
    ERR_MEM = -2,
}ERR;

typedef struct Queue Queue;

typedef struct {
    char id[MAX_LEN_ID];
    int ta;
    int ts;
}Person;

int is_full(Queue *);
int is_empty(Queue *);
int init(Queue ***, int);

int enqueue(Queue *, Person);
void dequeue(Queue *, Person *);
void free_queue(Queue ***, int);
void print_queue(Queue **, int, int);

Person *get_person(Queue **, int); // получение пас из очереди
void time_leave(Queue **, int, Person *, int); // настройка времени покидания очереди

#endif //QUEUE_H
