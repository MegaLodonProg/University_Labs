#ifndef LAB5_QUEUE_H
#define LAB5_QUEUE_H

#include "../include/Graph.h"

typedef struct QueueNode {
    Vertex* vertex;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode *head;
    QueueNode *tail;
} Queue;

Queue* create_queue();
void enqueue(Queue* q, Vertex* vertex);
Vertex* dequeue(Queue* q);
bool is_queue_empty(Queue* q);


#endif //LAB5_QUEUE_H
