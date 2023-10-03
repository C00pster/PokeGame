#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tile.h"

#ifndef GAME_MAP_H
#define GAME_MAP_H

typedef struct {
    Tile*** tiles;
    __int8_t top_path, bottom_path, left_path, right_path, horizontal_path_row, vertical_path_col;
    int distance;
    unsigned int*** distance_maps;
} GameMap;

typedef struct World World;

void generate_map(World *world, int x, int y);
void print_map(GameMap* game_map);
void free_map(GameMap* game_map);

#endif