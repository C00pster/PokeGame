#include <stdint.h>
#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* arr;
    __uint16_t front, back, size, capacity;
} Queue;

Queue* create_queue(__uint16_t c);
void enqueue(Queue *q, Point *p);
Point* dequeue(Queue *q);
int queue_size(Queue *q);
Point* queue_front(Queue *q);
Point* queue_back(Queue *queue);
void destroy_queue(Queue *queue);

Point* create_point(__uint8_t x, __uint8_t y);
Point* generate_point(__uint8_t x_min, __uint8_t x_max, __uint8_t y_min, __uint8_t y_max);

#endif