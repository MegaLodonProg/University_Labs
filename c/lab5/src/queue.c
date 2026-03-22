#include <stdlib.h>
#include "../include/queue.h"

Queue* create_queue() {
    Queue* q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void enqueue(Queue* q, Vertex* vertex) {
    QueueNode* node = malloc(sizeof(QueueNode));
    node->vertex = vertex;
    node->next = NULL;

    if (q->tail) {
        q->tail->next = node;
    }
    else {
        q->head = node;
    }
    q->tail = node;
}

Vertex* dequeue(Queue* q) {
    if (!q->head) return NULL;

    QueueNode* temp = q->head;
    Vertex* vertex = temp->vertex;

    q->head = q->head->next;
    if (!q->head) {
        q->tail = NULL;
    }

    free(temp);
    return vertex;
}

bool is_queue_empty(Queue* q) {
    return q->head == NULL;
}

