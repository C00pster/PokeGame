#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

#include "config.h"
#include "map/movement.h"
#include "data_structures/priority_queue.h"
#include "data_structures/point.h"

PriorityQueue* movement_queue;

int compare_trainer_distance(void* a, void* b) {
    TrainerDistance* t_a = (TrainerDistance*) a;
    TrainerDistance* t_b = (TrainerDistance*) b;
    if(t_a->distance == t_b->distance) return 0;
    return t_a->distance < t_b->distance ? -1 : 1;
}

void init_movement_queue() {
    movement_queue = create_priority_queue(num_trainers, compare_trainer_distance);
}

void add_trainer_to_movement_queue(Trainer* trainer, unsigned long distance) {
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
            exit(-1); //This shouldn't happen
    }
    return NULL;
}

void move_trainer(TrainerDistance* trainer_distance, GameMap* map, TrainerMap* trainer_map, int pc_x, int pc_y) {
    int cur_x, cur_y, new_x, new_y, counter;
    unsigned long movement_cost;
    cur_x = new_x = trainer_distance->trainer->x;
    cur_y = new_y = trainer_distance->trainer->y;
    if (!movement_queue) printf("movement_queue is null\n");
    Point* p;
    Direction direction;

    switch(trainer_distance->trainer->type) {
        case PC:
            exit(1); //This shouldn't happen
            break;
        case HIKER:
        case RIVAL:
            int min = UINT_MAX;
            int i, j;

            for (j = -1; j <= 1; j++) {
                for (i = -1; i <= 1; i++) {
                    if (i == 0 && j == 0) continue; //Don't calculate the current tile
                    if (cur_x + i < 1 || cur_x + i >= X_WIDTH - 1 || cur_y + j < 1 || cur_y + j >= Y_WIDTH - 1) continue; //Don't calculate tiles outside of the map
                    if (trainer_map->trainers[cur_y + j][cur_x + i] != NULL || (cur_y + j == pc_y && cur_x + i == pc_x)) continue; //Don't allow movement into other trainers
                    if (map->tile_weight_maps[trainer_distance->trainer->type][cur_y + j][cur_x + i] == UINT_MAX) continue; //Don't allow movement into impassable terrain
                    if (trainer_map->distance_maps[trainer_distance->trainer->type][cur_y + j][cur_x + i] < min) { //If the distance to the tile is less than the current minimum, set the new minimum and the new x and y coordinates
                        min = trainer_map->distance_maps[trainer_distance->trainer->type][cur_y+j][cur_x+i];
                        new_x = cur_x+i;
                        new_y = cur_y+j;
                    }
                }
            }

            break;
        case SWIMMER:
            exit(-1); //This shouldn't happen
            break;
        case PACER:
            direction = trainer_distance->trainer->direction;
            p = point_by_direction(cur_x, cur_y, direction);
            if (map->tile_weight_maps[trainer_distance->trainer->type][p->y][p->x] == UINT_MAX || trainer_map->trainers[p->y][p->x] != NULL) { //If the tile is impassable or occupied, turn around
                direction = (direction + 4) % NUM_DIRECTIONS;
                free(p);
                p = point_by_direction(cur_x, cur_y, direction);
            }
            if (map->tile_weight_maps[trainer_distance->trainer->type][p->y][p->x] != UINT_MAX && trainer_map->trainers[p->y][p->x] == NULL && !(p->y == pc_y && p->x == pc_x)) { //If the tile is passable and unoccupied, move forward
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
            while (counter > 0 && (map->tiles[p->y][p->x]->type != map->tiles[cur_y][cur_x]->type || trainer_map->trainers[p->y][p->x] != NULL)  && !(p->y == pc_y && p->x == pc_x)) { //If the tile is impassable or occupied, or the trainer has been wandering for too long, 
                int rand_num = rand() % (NUM_DIRECTIONS - 1) + 1; //Generate a random number to add to the current direction such the current direction will not be reconsidered
                direction = (direction + rand_num) % NUM_DIRECTIONS;
                free(p);
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
            exit(-1); //This shouldn't happen
            break;
        case EXPLORER:
            counter = 10;
            direction = trainer_distance->trainer->direction;
            p = point_by_direction(cur_x, cur_y, direction);
            while (counter > 0 && (map->tile_weight_maps[EXPLORER][p->y][p->x] == UINT_MAX || trainer_map->trainers[p->y][p->x] != NULL) && !(p->y == pc_y && p->x == pc_x)) { //If the tile is impassable or occupied, or the trainer has been wandering for too long,
                int rand_num = rand() % (NUM_DIRECTIONS - 1) + 1; //Generate a random number to add to the current direction such the current direction will not be reconsidered
                direction = (direction + rand_num) % NUM_DIRECTIONS;
                free(p);
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
            exit(-1); //This shouldn't happen
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

int move_pc(TrainerDistance* trainer_distance, GameMap* map, TrainerMap* trainer_map, Direction direction) {
    if (trainer_distance->trainer->type != PC) exit(1); //This shouldn't happen
    switch (direction) { //Inverting due to the way the map is printed
        case NORTH:
            direction = SOUTH;
            break;
        case NORTHEAST:
            direction = SOUTHEAST;
            break;
        case EAST:
        case WEST:
            break;
        case SOUTHEAST:
            direction = NORTHEAST;
            break;
        case SOUTH:
            direction = NORTH;
            break;
        case SOUTHWEST:
            direction = NORTHWEST;
            break;
        case NORTHWEST:
            direction = SOUTHWEST;
            break; 
        case NUM_DIRECTIONS:
            trainer_distance->distance += 10;
            push(movement_queue, trainer_distance);
            return 1;
    }
    Point* p = point_by_direction(trainer_distance->trainer->x, trainer_distance->trainer->y, direction);
    if (p->x < 1 || p->x >= X_WIDTH - 1 || p->y < 1 || p->y >= Y_WIDTH - 1) {
        mvprintw(0, 0, "Out of bounds");
        free(p);
        return 0;
    }
    if (map->tile_weight_maps[PC][p->y][p->x] == UINT_MAX) {
        mvprintw(0, 0, "Impassable terrain");
        free(p);
        return 0;
    }
    if (trainer_map->trainers[p->y][p->x] != NULL && trainer_map->trainers[p->y][p->x]->is_defeated == 0) {
        clear();
        mvprintw(0, 0, "You have encountered a trainer and won! Press ESC to exit");
        refresh();
        int input = 0;
        while (input != 27) {
            flushinp();
            input = getch();
            if (input == 27) {
                trainer_map->trainers[p->y][p->x]->is_defeated = 1;
            }
        }
    }
    trainer_distance->trainer->x = p->x;
    trainer_distance->trainer->y = p->y;
    trainer_distance->distance += map->tile_weight_maps[PC][p->y][p->x];
    free(p);
    push(movement_queue, trainer_distance);
    return 1;
}

int enter_building(GameMap* map, int x, int y) {
    if (map->tiles[y][x]->type == PMART || map->tiles[y][x]->type == PCNTR) {
        erase();
        mvprintw(0, 0, "You are in a placeholder building. Enter '<' to exit");
        refresh();
        int input = '>';
        while (input != '<') {
            flushinp();
            input = getch();
        }
        return 1;
    } return 0;
}

TrainerDistance* next_movement(GameMap* map, TrainerMap* trainer_map, int pc_x, int pc_y) {
    TrainerDistance* trainer_distance = (TrainerDistance*) pop(movement_queue);

    if (trainer_distance->trainer->type == PC || trainer_distance->trainer->is_defeated == 1) {
        return trainer_distance;
    }

    move_trainer(trainer_distance, map, trainer_map, pc_x, pc_y);

    return trainer_distance;
}

void clear_movement_queue() {
    while (movement_queue->size > 0) {
        free(pop(movement_queue));
    }
}

void free_movement_queue() {
    free_priority_queue(movement_queue);
}