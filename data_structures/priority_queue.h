#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#define MAX_SIZE 1680

typedef struct {
    int data[MAX_SIZE];
    int size;
} PriorityQueue;

void swap(int *a, int *b);
PriorityQueue* create_priority_queue();
void push(PriorityQueue *pq, int data);
int pop(PriorityQueue *pq);

#endif // PRIORITY_QUEUE_H