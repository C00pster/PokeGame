#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "tile.h"

//Includes a 2d array of tile pointers, the top, bottom, left, and right gate positions, the distance from the center of the world, and an array of 2d arrays of tile weights ordered by the TileType enum
typedef struct {
    Tile*** tiles;
    __int8_t top_path, bottom_path, left_path, right_path, horizontal_path_row, vertical_path_col;
    int distance;
    unsigned int*** tile_weight_maps;
} GameMap;

GameMap* generate_game_map(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance);
void free_map(GameMap* game_map);

#endif