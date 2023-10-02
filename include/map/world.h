#include "map/game_map.h"
#include "character.h"

#ifndef WORLD_H
#define WORLD_H

typedef struct World {
    GameMap*** maps;
    int pc_x, pc_y;
    int**** distance_maps;
    Trainer** trainers;
} World;

World* create_world();
void free_world(World* world);

#endif // WORLD_H