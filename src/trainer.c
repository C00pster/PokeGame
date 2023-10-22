#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "trainer.h"
#include "map/movement.h"
#include "config.h"

unsigned int* get_pc_weights(unsigned int* weights) {
    weights[BOULDER] = UINT_MAX;
    weights[TREE] = UINT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 10;
    weights[PCNTR] = 10;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = UINT_MAX;
    weights[FOREST] = UINT_MAX;
    weights[WATER] = UINT_MAX;
    weights[GATE] = 10;

    return weights;
}

unsigned int* get_hiker_weights(unsigned int* weights) {
    weights[BOULDER] = UINT_MAX;
    weights[TREE] = UINT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 15;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = 15;
    weights[FOREST] = 15;
    weights[WATER] = UINT_MAX;
    weights[GATE] = UINT_MAX;

    return weights;
}

unsigned int* get_rival_weights(unsigned int* weights) {
    weights[BOULDER] = UINT_MAX;
    weights[TREE] = UINT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = UINT_MAX;
    weights[FOREST] = UINT_MAX;
    weights[WATER] = UINT_MAX;
    weights[GATE] = UINT_MAX;

    return weights;
}

unsigned int* get_swimmer_weights(unsigned int* weights) {
    weights[BOULDER] = UINT_MAX;
    weights[TREE] = UINT_MAX;
    weights[PATH] = UINT_MAX;
    weights[PMART] = UINT_MAX;
    weights[PCNTR] = UINT_MAX;
    weights[TGRASS] = UINT_MAX;
    weights[SGRASS] = UINT_MAX;
    weights[MOUNTAIN] = UINT_MAX;
    weights[FOREST] = UINT_MAX;
    weights[WATER] = 7;
    weights[GATE] = UINT_MAX;

    return weights;
}

unsigned int* get_other_weights(unsigned int* weights) {
    weights[BOULDER] = UINT_MAX;
    weights[TREE] = UINT_MAX;
    weights[PATH] = 10;
    weights[PMART] = 50;
    weights[PCNTR] = 50;
    weights[TGRASS] = 20;
    weights[SGRASS] = 10;
    weights[MOUNTAIN] = UINT_MAX;
    weights[FOREST] = UINT_MAX;
    weights[WATER] = UINT_MAX;
    weights[GATE] = UINT_MAX;

    return weights;
}

Trainer* create_trainer(TrainerType type, int x, int y) {
    Trainer* trainer = (Trainer*)malloc(sizeof(Trainer));
    trainer->type = type;
    trainer->x = x;
    trainer->y = y;
    trainer->direction = rand() % NUM_DIRECTIONS;
    trainer->is_defeated = 0;

    trainer->weights = malloc(sizeof(int) * NUM_TILES);
    get_trainer_weights(type, trainer->weights);

    return trainer;
}

unsigned int* get_trainer_weights(TrainerType type, unsigned int* result) {
    switch (type) {
        case PC:
            get_pc_weights(result);
            break;
        case HIKER:
            get_hiker_weights(result);
            break;
        case RIVAL:
            get_rival_weights(result);
            break;
        case SWIMMER:
            get_swimmer_weights(result);
            break;
        case PACER:
        case WANDERER:
        case SENTRY:
        case EXPLORER:
            get_other_weights(result);
            break;
        case NUM_TRAINERS:
            result = NULL;
            break;
    }

    return result;
}

int is_valid_coor(int x, int y, TrainerMap* trainer_map) {
    if (1==1) return 1;
    return 0;
}

Trainer* add_hiker(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 2) + 1;
    int y = rand() % (Y_WIDTH - 2) + 1;

    int valid = 1;
    while (valid != 0) {
        if (is_valid_coor(x, y, trainer_map) && game_map->tiles[y][x]->type != WATER) {
            valid = 0;
        } else {
            x = rand() % (X_WIDTH - 2) + 1;
            y = rand() % (Y_WIDTH - 2) + 1;
        }
    }

    Trainer* trainer = create_trainer(HIKER, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_rival(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 2) + 1;
    int y = rand() % (Y_WIDTH - 2) + 1;

    int valid = 1;
    while (valid != 0) {
        TileType tile_type = game_map->tiles[y][x]->type;
        if (is_valid_coor(x, y, trainer_map) && tile_type != MOUNTAIN && tile_type != WATER && tile_type != FOREST) {
            valid = 0;
        } else {
            x = rand() % (X_WIDTH - 2) + 1;
            y = rand() % (Y_WIDTH - 2) + 1;
        }
    }

    Trainer* trainer = create_trainer(RIVAL, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_swimmer(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 2) + 1;
    int y = rand() % (Y_WIDTH - 2) + 1;

    int valid = 1;
    while (valid != 0) {
        if (is_valid_coor(x, y, trainer_map) && game_map->tiles[y][x]->type == WATER) {
            valid = 0;
        } else {
            x = rand() % (X_WIDTH - 2) + 1;
            y = rand() % (Y_WIDTH - 2) + 1;
        }
    }

    Trainer* trainer = create_trainer(SWIMMER, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_other(GameMap* game_map, TrainerType trainer_type, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 2) + 1;
    int y = rand() % (Y_WIDTH - 2) + 1;

    int valid = 1;
    while (valid != 0) {
        TileType tile_type = game_map->tiles[y][x]->type;
        if (is_valid_coor(x, y, trainer_map) && tile_type != WATER && tile_type != MOUNTAIN && tile_type != FOREST) {
            valid = 0;
        } else {
            x = rand() % (X_WIDTH - 2) + 1;
            y = rand() % (Y_WIDTH - 2) + 1;
        }
    }

    Trainer* trainer = create_trainer(trainer_type, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

void add_trainer(TrainerType type, GameMap* game_map, TrainerMap* trainer_map) {
    Trainer* trainer;
    int x = rand() % (X_WIDTH - 2) + 1;
    int y = rand() % (Y_WIDTH - 2) + 1;

    int valid = 1;
    while (valid != 0) {
        if (trainer_map->trainers[y][x] == NULL && game_map->tile_weight_maps[type][y][x] != UINT_MAX) {
            valid = 0;
        } else {
            x = rand() % (X_WIDTH - 2) + 1;
            y = rand() % (Y_WIDTH - 2) + 1;
        }
    }

    trainer = create_trainer(type, x, y);
    trainer_map->trainers[y][x] = trainer;
    
    switch(type) {
        case PC:
            exit(-1);
        case HIKER:
        case RIVAL:
        case SWIMMER:
        case PACER:
        case WANDERER:
        case EXPLORER:
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case SENTRY:
            //Sentries don't move
            break;
        case NUM_TRAINERS:
            exit(-1);
    }
}

void add_trainers(GameMap* game_map, TrainerMap* trainer_map, int x, int y) {
    clear_trainer_map(trainer_map);
    clear_movement_queue();
    TrainerType trainer_types[6] = {HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER}; //excluding swimmer for now
    int i;

    for (i = 0; i < num_trainers; i++) {
        if (i == 0) add_trainer(HIKER, game_map, trainer_map);
        else if (i == 1) add_trainer(RIVAL, game_map, trainer_map);
        else {
            TrainerType type = trainer_types[rand() % 6];
            add_trainer(type, game_map, trainer_map);
        }
    }
}

char get_trainer_char(TrainerType type) {
    switch (type) {
        case PC:
            return '@';
            break;
        case HIKER:
            return 'h';
            break;
        case RIVAL:
            return 'r';
            break;
        case SWIMMER:
            return 's';
            break;
        case PACER:
            return 'p';
            break;
        case WANDERER:
            return 'w';
            break;
        case SENTRY:
            return 'x';
            break;
        case EXPLORER:
            return 'e';
            break;
        case NUM_TRAINERS:
            exit(-1);
    }
    return '!'; //This should never happen
}

TrainerMap* allocate_trainer_map() {
    int i, j;
    TrainerMap* trainer_map = (TrainerMap*) malloc(sizeof(TrainerMap));
    trainer_map->trainers = (Trainer***) malloc(sizeof(Trainer**) * Y_WIDTH);
    if (!trainer_map->trainers) {
        printf("Error allocating memory for trainer_map\n");
        return NULL;
    }
    for (i = 0; i < Y_WIDTH; i++) {
        trainer_map->trainers[i] = (Trainer**) malloc(sizeof(Trainer*) * X_WIDTH);
        if (!trainer_map->trainers[i]) {
            printf("Error allocating memory for trainer_map\n");
            return NULL;
        }
        for (j = 0; j < X_WIDTH; j++) {
            trainer_map->trainers[i][j] = NULL;
        }
    }

    return trainer_map;
}

void clear_trainer_map(TrainerMap* trainer_map) {
    int i, j;
    for (i = 0; i < Y_WIDTH; i++) {
        for (j = 0; j < X_WIDTH; j++) {
            trainer_map->trainers[i][j] = NULL;
        }
    }
}

void free_trainer(Trainer* trainer) {
    free(trainer->weights);
    free(trainer);
}