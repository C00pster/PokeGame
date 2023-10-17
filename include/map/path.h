#ifndef PATH_H
#define PATH_H

#include "map/game_map.h"

//Contains the top, bottom, left, and right gate positions
typedef struct {
    __int8_t top, bottom, left, right;
} PathTracker;

void generate_path(GameMap* m, __int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance);

#endif // PATH_H