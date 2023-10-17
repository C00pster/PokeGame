#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

//Contains a blank pointer array for abstract data types, the capacity of the array, the size of the array, and a comparison function for the abstract data type
typedef struct {
    void** array;
    int capacity;
    int size;
    int (*compare)(void*, void*);
} PriorityQueue;

PriorityQueue* create_priority_queue(int capacity, int (*compare)(void*, void*));
void push(PriorityQueue* pq, void* item);
void* pop(PriorityQueue* pq);
void free_priority_queue(PriorityQueue* pq);

#endif // PRIORITY_QUEUE_H