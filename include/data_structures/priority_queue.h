#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct {
    void** array;
    int capacity;
    int size;
    int (*compare)(void*, void*); //Comparison function
} PriorityQueue;

PriorityQueue* create_priority_queue(int capacity, int (*compare)(void*, void*));
bool is_empty(PriorityQueue* pq);
void push(PriorityQueue* pq, void* item);
void* pop(PriorityQueue* pq);
void free_priority_queue(PriorityQueue* pq);

#endif // PRIORITY_QUEUE_H