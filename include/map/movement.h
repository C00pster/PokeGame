#ifndef MAP_MOVEMENT_H
#define MAP_MOVEMENT_H

#include "map/game_map.h"
#include "trainer.h"

Trainer* next_movement(GameMap* map, TrainerMap* trainer_map);
void add_trainer_to_movement_queue(Trainer* trainer, int distance);
void init_movement_queue();

#endif // MAP_MOVEMENT_H