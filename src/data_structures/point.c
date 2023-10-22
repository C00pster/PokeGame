#include <stdlib.h>

#include "data_structures/point.h"


Point* create_point(int x, int y) {
    Point* point = (Point*) malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}

Point* generate_point(int x_min, int x_max, int y_min, int y_max) {
    Point* point = (Point*) malloc(sizeof(Point));
    point->x = x_min + rand() % (x_max - x_min + 1);
    point->y = y_min + rand() % (y_max - y_min + 1);
    return point;
}