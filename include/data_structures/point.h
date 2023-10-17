#ifndef POINT_H
#define POINT_H

typedef struct {
    int x, y;
} Point;

Point* create_point(__uint8_t x, __uint8_t y);
Point* generate_point(__uint8_t x_min, __uint8_t x_max, __uint8_t y_min, __uint8_t y_max);

#endif // POINT_H