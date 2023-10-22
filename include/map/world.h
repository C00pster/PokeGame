#ifndef WORLD_H
#define WORLD_H

#include "map/game_map.h"
#include "trainer.h"

//Contains a 2d array of GameMap pointers (game_map.h) and a TrainerMap pointer (trainer.h)
typedef struct World {
    GameMap*** maps;
    TrainerMap* trainer_map;
    Trainer* pc;
} World;

World* create_world();
void generate_map(World* world, int x, int y);
void print_map(World* world, int x, int y, int pc_x, int pc_y);
void free_world(World* world);

int display_trainers(World* world, int world_x, int world_y, int pc_x, int pc_y);

#endif // WORLD_H