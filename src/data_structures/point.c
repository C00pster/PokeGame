#include <stdlib.h>

#include "data_structures/point.h"


Point* create_point(__uint8_t x, __uint8_t y) {
    Point* point = (Point*) malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}

Point* generate_point(__uint8_t x_min, __uint8_t x_max, __uint8_t y_min, __uint8_t y_max) {
    Point* point = (Point*) malloc(sizeof(Point));
    point->x = x_min + rand() % (x_max - x_min + 1);
    point->y = y_min + rand() % (y_max - y_min + 1);
    return point;
}