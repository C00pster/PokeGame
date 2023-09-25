#include <limits.h>
#include <stdlib.h>

#include "character.h"
#include "map/tile.h"

int* get_pc_weights() {
    int* weights = malloc(sizeof(int) * NUM_TILES);
    weights[BOULDER] = INT_MAX;
    weights[TREE] = INT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 10;
    weights[PCNTR] = 10;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = INT_MAX;
    weights[FOREST] = INT_MAX;
    weights[WATER] = INT_MAX;
    weights[GATE] = 10;

    return weights;
}

int* get_hiker_weights() {
    int* weights = malloc(sizeof(int) * NUM_TILES);
    weights[BOULDER] = INT_MAX;
    weights[TREE] = INT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 15;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = 15;
    weights[FOREST] = 15;
    weights[WATER] = INT_MAX;
    weights[GATE] = INT_MAX;

    return weights;
}

int* get_rival_weights() {
    int* weights = malloc(sizeof(int) * NUM_TILES);
    weights[BOULDER] = INT_MAX;
    weights[TREE] = INT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = INT_MAX;
    weights[FOREST] = INT_MAX;
    weights[WATER] = INT_MAX;
    weights[GATE] = INT_MAX;

    return weights;
}

int* get_swimmer_weights() {
    int* weights = malloc(sizeof(int) * NUM_TILES);
    weights[BOULDER] = INT_MAX;
    weights[TREE] = INT_MAX;
    weights[PATH] = INT_MAX;
    weights[PMART] = INT_MAX;
    weights[PCNTR] = INT_MAX;
    weights[TGRASS] = INT_MAX;
    weights[SGRASS] = INT_MAX;
    weights[MOUNTAIN] = INT_MAX;
    weights[FOREST] = INT_MAX;
    weights[WATER] = 7;
    weights[GATE] = INT_MAX;

    return weights;
}

int* get_other_weights() {
    int* weights = malloc(sizeof(int) * NUM_TILES);
    weights[BOULDER] = INT_MAX;
    weights[TREE] = INT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = INT_MAX;
    weights[FOREST] = INT_MAX;
    weights[WATER] = INT_MAX;
    weights[GATE] = INT_MAX;

    return weights;
}

Trainer* create_trainer(TrainerType type, int x, int y) {
    Trainer* trainer = (Trainer*)malloc(sizeof(Trainer));
    trainer->type = type;
    trainer->x = x;
    trainer->y = y;

    trainer->weights = get_trainer_weights(type);

    return trainer;
}

int* get_trainer_weights(TrainerType type) {
    int* result;
    switch (type) {
        case PC:
            result = get_pc_weights();
            break;
        case HIKER:
            result = get_hiker_weights();
            break;
        case RIVAL:
            result = get_rival_weights();
            break;
        case SWIMMER:
            result = get_swimmer_weights();
            break;
        case OTHER:
            result = get_other_weights();
            break;
        case NUM_TRAINERS:
            result = NULL;
            break;
    }

    return result;
}

void free_trainer(Trainer* trainer) {
    free(trainer->weights);
    free(trainer);
}