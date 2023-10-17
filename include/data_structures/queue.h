#include <stdint.h>
#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H

#include "data_structures/point.h"

//Circular queue implementation for Point struct
typedef struct {
    Point** arr;
    __uint16_t front, back, size, capacity;
} Queue;

Queue* create_queue(__uint16_t c);
void enqueue(Queue* q, Point* p);
Point* dequeue(Queue* q);
int queue_size(Queue* q);
Point* queue_front(Queue* q);
Point* queue_back(Queue* queue);
void destroy_queue(Queue* queue);

#endif