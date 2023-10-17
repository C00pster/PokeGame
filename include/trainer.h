#ifndef TRAINER_H
#define TRAINER_H

#include "map/game_map.h"
#include "config.h"

//Contains the different types of trainers
typedef enum {
    PC,
    HIKER,
    RIVAL,
    SWIMMER,
    PACER,
    WANDERER,
    SENTRY,
    EXPLORER,
    NUM_TRAINERS
} TrainerType;

//Contains an array of tile weights for the trainer ordered by the TileType enum, the x and y coordinate of the trainer on the GameMap, and the TrainerType of the trainer
typedef struct {
    int* weights;
    int x, y;
    TrainerType type;
} Trainer;

//Contains a 2d array of trainer pointers, the number of trainers, and an array of 2d array distance maps ordered by the TrainerType enum
typedef struct {
    Trainer*** trainers;
    int num_trainers;
    unsigned int*** distance_maps;
} TrainerMap;

Trainer* create_trainer(TrainerType type, int x, int y);
int* get_trainer_weights(TrainerType type, int* result);
void add_trainers(GameMap* game_map, TrainerMap* trainer_map, int x, int y);
char get_trainer_char(TrainerType trainer_type);
void free_trainer(Trainer* trainer);

TrainerMap* allocate_trainer_map();
void clear_trainer_map(TrainerMap* trainer_map);
void free_trainer_map(TrainerMap* trainer_map);

#endif // TRAINER_H