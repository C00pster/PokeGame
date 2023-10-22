#ifndef POINT_H
#define POINT_H

typedef struct {
    int x, y;
} Point;

Point* create_point(int x, int y);
Point* generate_point(int x_min, int x_max, int y_min, int y_max);

#endif // POINT_H