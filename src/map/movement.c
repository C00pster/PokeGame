#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

#include "config.h"
#include "map/movement.h"
#include "data_structures/priority_queue.h"
#include "data_structures/point.h"

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

Point* point_by_direction(int cur_x, int cur_y, Direction direction) {
    switch (direction) {
        case NORTH:
            return create_point(cur_x, cur_y+1);
        case NORTHEAST:
            return create_point(cur_x+1, cur_y+1);
        case EAST:
            return create_point(cur_x+1, cur_y);
        case SOUTHEAST:
            return create_point(cur_x+1, cur_y-1);
        case SOUTH:
            return create_point(cur_x, cur_y-1);
        case SOUTHWEST:
            return create_point(cur_x-1, cur_y-1);
        case WEST:
            return create_point(cur_x-1, cur_y);
        case NORTHWEST:
            return create_point(cur_x-1, cur_y+1);
        case NUM_DIRECTIONS:
            exit(1); //This shouldn't happen
    }
    return NULL;
}

void move_trainer(TrainerDistance* trainer_distance, GameMap* map, TrainerMap* trainer_map) {
    int cur_x, cur_y, new_x, new_y, counter, movement_cost;
    cur_x = new_x = trainer_distance->trainer->x;
    cur_y = new_y = trainer_distance->trainer->y;
    Point* p;
    Direction direction;

    switch(trainer_distance->trainer->type) {
        case PC:
            exit(1); //This shouldn't happen
            break;
        case HIKER:
        case RIVAL:
            int min = INT_MAX;
            int i, j;

            for (j = -1; j <= 1; j++) {
                for (i = -1; i <= 1; i++) {
                    if (i == 0 && j == 0) continue; //Don't calculate the current tile
                    if (cur_x + i < 1 || cur_x + i >= X_WIDTH - 1 || cur_y + j < 1 || cur_y + j >= Y_WIDTH - 1) continue; //Don't calculate tiles outside of the map
                    if (trainer_map->trainers[cur_y + j][cur_x + i] != NULL) continue; //Don't allow movement into other trainers
                    if (trainer_map->distance_maps[trainer_distance->trainer->type][cur_y + j][cur_x + i] < min) { //If the distance to the tile is less than the current minimum, set the new minimum and the new x and y coordinates
                        min = trainer_map->distance_maps[trainer_distance->trainer->type][cur_y+j][cur_x+i];
                        new_x = cur_x+i;
                        new_y = cur_y+j;
                    }
                }
            }

            break;
        case SWIMMER:
            exit(1); //This shouldn't happen
            break;
        case PACER:
            direction = trainer_distance->trainer->direction;
            p = point_by_direction(cur_x, cur_y, direction);
            if (map->tile_weight_maps[trainer_distance->trainer->type][p->y][p->x] == INT_MAX || trainer_map->trainers[p->y][p->x] != NULL) { //If the tile is impassable or occupied, turn around
                direction = (direction + 4) % NUM_DIRECTIONS;
                p = point_by_direction(cur_x, cur_y, direction);
            }
            if (map->tile_weight_maps[trainer_distance->trainer->type][p->y][p->x] != INT_MAX && trainer_map->trainers[p->y][p->x] == NULL) { //If the tile is passable and unoccupied, move forward
                new_x = p->x;
                new_y = p->y;
            }
            trainer_distance->trainer->direction = direction;
            free(p);
            break;
        case WANDERER:
            counter = 10;
            direction = trainer_distance->trainer->direction;
            p = point_by_direction(cur_x, cur_y, direction);
            while (counter > 0 && (map->tiles[p->y][p->x]->type != map->tiles[cur_y][cur_x]->type || trainer_map->trainers[p->y][p->x] != NULL)) { //If the tile is impassable or occupied, or the trainer has been wandering for too long, 
                int rand_num = rand() % (NUM_DIRECTIONS - 1) + 1; //Generate a random number to add to the current direction such the current direction will not be reconsidered
                direction = (direction + rand_num) % NUM_DIRECTIONS;
                p = point_by_direction(cur_x, cur_y, direction);
                counter--;
            }
            if (counter != 0) { //If the trainer has been wandering for too long, don't move
                new_x = p->x;
                new_y = p->y;
            }
            trainer_distance->trainer->direction = direction;
            free(p);
            break;
        case SENTRY:
            exit(1); //This shouldn't happen
            break;
        case EXPLORER:
            counter = 10;
            direction = trainer_distance->trainer->direction;
            p = point_by_direction(cur_x, cur_y, direction);
            while (counter > 0 && (map->tile_weight_maps[EXPLORER][p->y][p->x] == INT_MAX || trainer_map->trainers[p->y][p->x] != NULL)) { //If the tile is impassable or occupied, or the trainer has been wandering for too long,
                int rand_num = rand() % (NUM_DIRECTIONS - 1) + 1; //Generate a random number to add to the current direction such the current direction will not be reconsidered
                direction = (direction + rand_num) % NUM_DIRECTIONS;
                p = point_by_direction(cur_x, cur_y, direction);
            }
            if (counter != 0) { //If the trainer has been wandering for too long, don't move
                new_x = p->x;
                new_y = p->y;
            }
            trainer_distance->trainer->direction = direction;
            free(p);
            break;
        case NUM_TRAINERS:
            exit(1); //This shouldn't happen
            break;
    }

    trainer_distance->trainer->x = new_x;
    trainer_distance->trainer->y = new_y;

    if (cur_x != new_x || cur_y != new_y) {
        trainer_map->trainers[cur_y][cur_x] = NULL;
        trainer_map->trainers[new_y][new_x] = trainer_distance->trainer;
    }

    movement_cost = map->tile_weight_maps[trainer_distance->trainer->type][new_y][new_x];
    trainer_distance->distance += movement_cost;

    push(movement_queue, trainer_distance);
}

Trainer* next_movement(GameMap* map, TrainerMap* trainer_map) {
    TrainerDistance* trainer_distance = (TrainerDistance*) pop(movement_queue);

    move_trainer(trainer_distance, map, trainer_map);

    return trainer_distance->trainer;
}

void clear_movement_queue() {
    while (movement_queue->size > 0) {
        free(pop(movement_queue));
    }
}

void free_movement_queue() {
    free_priority_queue(movement_queue);
}