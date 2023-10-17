#include "data_structures/queue.h"

Queue* create_queue(__uint16_t c) {
    Queue* q = (Queue*) malloc(sizeof(Queue));
    if (!q) {
        return NULL;
    }
    q->front=q->size=0;
    q->back=c-1;
    q->capacity=c;
    q->arr = (Point**) malloc(c * sizeof(Point*));
    if (!q->arr) {
        return NULL;
    }
    return q;
}

void enqueue(Queue* q, Point* p)  {
    q->back = (q->back + 1) % q->capacity;
    q->arr[q->back] = p;
    q->size++;
    return;
}

Point* dequeue(Queue* q) {
    Point* point = q->arr[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size -= 1;
    return point;
}

int queue_size(Queue* q) {
    return q->size;
}

Point* queue_front(Queue* q) {
    return q->arr[q->front];
}

Point* queue_back(Queue* q) {
    return q->arr[q->back];
}

void destroy_queue(Queue* q) {
    while (q->size > 0) {
        free(dequeue(q));
    }
    free(q->arr);
    free(q);
    return;
}