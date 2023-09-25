#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tile.h"

#ifndef MAP_GENERATION_H
#define MAP_GENERATION_H

typedef struct {
    Tile*** map;
    __int8_t top_path, bottom_path, left_path, right_path, horizontal_path_row, vertical_path_col;
    int distance;
    int** rival_distance_map;
    int** hiker_distance_map;
} Map;

void generate_map(Map*** world, int x, int y);
void free_map(Map* map);

Map*** create_world();
void free_world(Map*** world);

#endif