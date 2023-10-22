#ifndef MAP_MOVEMENT_H
#define MAP_MOVEMENT_H

#include "map/game_map.h"
#include "trainer.h"

typedef struct {
    Trainer* trainer;
    unsigned long distance;
} TrainerDistance;

TrainerDistance* next_movement(GameMap* map, TrainerMap* trainer_map, int pc_x, int pc_y);
void add_trainer_to_movement_queue(Trainer* trainer, unsigned long distance);
int move_pc(TrainerDistance* trainer_distance, GameMap* map, TrainerMap* trainer_map, Direction direction);
int enter_building(GameMap* map, int x, int y);
void init_movement_queue();
void clear_movement_queue();
void free_movement_queue();

#endif // MAP_MOVEMENT_H