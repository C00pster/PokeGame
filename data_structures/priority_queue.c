#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>

/*
Using binary heap implementation of a priority queue as generated by ChatGPT
Prompt = "Implement a priority queue in C"
*/

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp; 
}

PriorityQueue* create_priority_queue() {
    PriorityQueue* priority_queue = (PriorityQueue*) malloc(sizeof(PriorityQueue));
    priority_queue->size = 0;
    return priority_queue;
}

void push(PriorityQueue* pq, int data) {
    if (pq->size >= MAX_SIZE) {
        printf("Priority Queue is full!\n");
        return;
    }
    pq->data[pq->size] = data;
    int current_index = pq->size;

    //Bubble up
    while (current_index > 0 && pq->data[current_index] > pq->data[(current_index-1)/2]) {
        swap(&pq->data[current_index], &pq->data[(current_index-1)/2]);
        current_index = (current_index-1)/2;
    }
}

int pop(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("Priority Queue is empty.\n");
        return -1;
    }

    int ret = pq->data[0];
    pq->data[0] = pq->data[--pq->size];

    int current = 0;
    while (current * 2 + 1 < pq->size) {
        int child;
        if (current * 2 + 2 == pq->size) {
            child = current * 2 + 1;
        } else {
            child = pq->data[current * 2 + 1] > pq->data[current * 2 + 2] ? current * 2 + 1 : current * 2 + 2;
        }

        if (pq->data[current] >= pq->data[child]) {
            break;
        }

        swap(&pq->data[current], &pq->data[child]);
        current = child;
    }

    return ret;
}