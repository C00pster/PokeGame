#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

#include "config.h"
#include "map/movement.h"
#include "data_structures/priority_queue.h"

PriorityQueue* movement_queue;

typedef struct {
    Trainer* trainer;
    int distance;
} TrainerDistance;

int compare_trainer_distance(void* a, void* b) {
    TrainerDistance* t_a = (TrainerDistance*) a;
    TrainerDistance* t_b = (TrainerDistance*) b;
    return t_a->distance - t_b->distance;
}

void init_movement_queue() {
    movement_queue = create_priority_queue(num_trainers, compare_trainer_distance);
}

void add_trainer_to_movement_queue(Trainer* trainer, int distance) {
    TrainerDistance* trainer_distance = (TrainerDistance*) malloc(sizeof(TrainerDistance));
    trainer_distance->trainer = trainer;
    trainer_distance->distance = distance;
    push(movement_queue, trainer_distance);
}

void move_trainer(TrainerDistance* trainer_distance, GameMap* map, TrainerMap* trainer_map) {
    int cur_x, cur_y, new_x, new_y;
    cur_x = new_x = trainer_distance->trainer->x;
    cur_y = new_y = trainer_distance->trainer->y;
    int min = INT_MAX;
    int i, j;

    for (j = -1; j <= 1; j++) {
        for (i = -1; i <= 1; i++) {
            if (i == 0 && j == 0) continue; //Don't calculate the current tile
            if (cur_x + i < 1 || cur_x + i >= X_WIDTH - 1 || cur_y + j < 1 || cur_y + j >= Y_WIDTH - 1) continue; //Don't calculate tiles outside of the map
            if (trainer_map->distance_maps[trainer_distance->trainer->type][cur_y + j][cur_x + i] < min) { //If the distance to the tile is less than the current minimum, set the new minimum and the new x and y coordinates
                min = trainer_map->distance_maps[trainer_distance->trainer->type][cur_y+j][cur_x+i];
                new_x = cur_x+i;
                new_y = cur_y+j;
            }
        }
    }

    trainer_distance->trainer->x = new_x;
    trainer_distance->trainer->y = new_y;

    trainer_map->trainers[cur_y][cur_x] = NULL;
    trainer_map->trainers[new_y][new_x] = trainer_distance->trainer;

    int movement_cost = map->tile_weight_maps[trainer_distance->trainer->type][new_y][new_x];
    trainer_distance->distance += movement_cost;

    push(movement_queue, trainer_distance);
}

Trainer* next_movement(GameMap* map, TrainerMap* trainer_map) {
    TrainerDistance* trainer_distance = (TrainerDistance*) pop(movement_queue);

    move_trainer(trainer_distance, map, trainer_map);

    return trainer_distance->trainer;
}