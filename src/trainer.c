#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "trainer.h"
#include "map/movement.h"
#include "config.h"

int* get_pc_weights(int* weights) {
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

int* get_hiker_weights(int* weights) {
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

int* get_rival_weights(int* weights) {
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

int* get_swimmer_weights(int* weights) {
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

int* get_other_weights(int* weights) {
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

    int* weights = malloc(sizeof(int) * NUM_TILES);
    trainer->weights = get_trainer_weights(type, weights);

    return trainer;
}

int* get_trainer_weights(TrainerType type, int* result) {
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

Trainer* add_hiker(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 1) + 1;
    int y = rand() % (Y_WIDTH - 1) + 1;

    int is_valid = 1;
    while (is_valid != 0) {
        if (game_map->tiles[y][x]->type != WATER && trainer_map->trainers[y][x] == NULL) {
            is_valid = 0;
        } else {
            x = rand() % (X_WIDTH - 1) + 1;
            y = rand() % (Y_WIDTH - 1) + 1;
        }
    }

    Trainer* trainer = create_trainer(HIKER, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_rival(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 1) + 1;
    int y = rand() % (Y_WIDTH - 1) + 1;

    int is_valid = 1;
    while (is_valid != 0) {
        TileType tile_type = game_map->tiles[y][x]->type;
        if (tile_type != MOUNTAIN && tile_type != WATER && tile_type != FOREST && trainer_map->trainers[y][x] == NULL) {
            is_valid = 0;
        } else {
            x = rand() % (X_WIDTH - 1) + 1;
            y = rand() % (Y_WIDTH - 1) + 1;
        }
    }

    Trainer* trainer = create_trainer(RIVAL, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_swimmer(GameMap* game_map, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 1) + 1;
    int y = rand() % (Y_WIDTH - 1) + 1;

    int is_valid = 1;
    while (is_valid != 0) {
        if (game_map->tiles[y][x]->type == WATER && trainer_map->trainers[y][x] == NULL) {
            is_valid = 0;
        } else {
            x = rand() % (X_WIDTH - 1) + 1;
            y = rand() % (Y_WIDTH - 1) + 1;
        }
    }

    Trainer* trainer = create_trainer(SWIMMER, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

Trainer* add_other(GameMap* game_map, TrainerType trainer_type, TrainerMap* trainer_map) {
    int x = rand() % (X_WIDTH - 1) + 1;
    int y = rand() % (Y_WIDTH - 1) + 1;

    int is_valid = 1;
    while (is_valid != 0) {
        TileType tile_type = game_map->tiles[y][x]->type;
        if (tile_type != WATER && tile_type != MOUNTAIN && tile_type != FOREST && trainer_map->trainers[y][x] == NULL) {
            is_valid = 0;
        } else {
            x = rand() % (X_WIDTH - 1) + 1;
            y = rand() % (Y_WIDTH - 1) + 1;
        }
    }

    Trainer* trainer = create_trainer(trainer_type, x, y);
    trainer_map->trainers[y][x] = trainer;
    return trainer;
}

void add_trainer(TrainerType type, GameMap* game_map, TrainerMap* trainer_map) {
    Trainer* trainer;
    switch(type) {
        case PC:
            exit(1);
        case HIKER:
            trainer = add_hiker(game_map, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case RIVAL:
            trainer = add_rival(game_map, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case SWIMMER:
            trainer = add_swimmer(game_map, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case PACER:
            trainer = add_other(game_map, PACER, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case WANDERER:
            trainer = add_other(game_map, WANDERER, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case SENTRY:
            trainer = add_other(game_map, SENTRY, trainer_map);
            //Sentries don't move
            break;
        case EXPLORER:
            trainer = add_other(game_map, EXPLORER, trainer_map);
            add_trainer_to_movement_queue(trainer, 0);
            break;
        case NUM_TRAINERS:
            exit(1);
    }
}

void add_trainers(GameMap* game_map, TrainerMap* trainer_map, int x, int y) {
    TrainerType trainer_types[6] = {HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER}; //excluding swimmer for now
    int i;

    for (i = 0; i < num_trainers; i++) {
        if (i == 0) add_trainer(HIKER, game_map, trainer_map);
        else if (i == 1) add_trainer(RIVAL, game_map, trainer_map);
        else {
            TrainerType type = trainer_types[rand() % 6];
            add_trainer(type, game_map, trainer_map);
        }
        trainer_map->num_trainers++;
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
            exit(1);
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