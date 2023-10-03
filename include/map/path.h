#include "map/world.h"

#ifndef PATH_H
#define PATH_H

typedef struct {
    __int8_t top, bottom, left, right;
} Path_Tracker;

void generate_path(GameMap* m, __int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance);
Path_Tracker* get_paths(World* world, int x, int y);

#endif // PATH_H