#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>

#include "map/world.h"
#include "map/path.h"
#include "map/movement.h"
#include "dijkstra.h"
#include "config.h"

PathTracker* get_paths(World* world, int x, int y) {
    PathTracker* path_tracker = (PathTracker*) malloc(sizeof(PathTracker));

    if (x + 1 < SIZE) {
        if (world->maps[y][x+1]) {
            path_tracker->right = world->maps[y][x+1]->left_path;
        } else path_tracker->right = 0;
    } else path_tracker->right = -1;
    if (x - 1 > 0) {
        if (world->maps[y][x-1]) {
            path_tracker->left = world->maps[y][x-1]->right_path;
        } else path_tracker->left = 0;
    } else path_tracker->left = -1;
    if (y + 1 < SIZE) {
        if (world->maps[y+1][x]) {
            path_tracker->bottom = world->maps[y+1][x]->top_path;
        } else path_tracker->bottom = 0;
    } else path_tracker->bottom = -1;
    if (y - 1 > 0) {
        if (world->maps[y-1][x]) {
            path_tracker->top = world->maps[y-1][x]->bottom_path;
        } else path_tracker->top = 0;
    } else path_tracker->top = -1;

    return path_tracker;
}

//Adds the personal character to the world on a path
void add_pc(World* world, int x, int y) {
    int x_coor, y_coor;

    if (rand() % 2 == 1) {
        y_coor = world->maps[INDEX(y)][INDEX(x)]->horizontal_path_row;
        x_coor = rand() % (X_WIDTH - 6) + 3;
    } else {
        y_coor = rand() % (Y_WIDTH - 6) + 3;
        x_coor = world->maps[INDEX(y)][INDEX(x)]->vertical_path_col;
    }

    Trainer* pc = create_trainer(PC, x_coor, y_coor);

    world->pc = pc;

    add_trainer_to_movement_queue(pc, 0);

    generate_distance_maps(world, x, y, x_coor, y_coor);

    return;
}

void generate_weight_maps(GameMap* game_map) {
    int i;
    unsigned int tile_weights[NUM_TILES];
    for (i = PC; i < NUM_TRAINERS; i++) {
        get_trainer_weights(i, tile_weights);
        generate_weight_map(game_map, tile_weights, i);
    }
}


//Creates a map if it doesn't exist. Adds trainers to created map
void generate_map(World* world, int x, int y) {
    if (world->maps[INDEX(y)][INDEX(x)]);
    else if (x != 0 || y != 0) {
        PathTracker* p = get_paths(world, INDEX(x), INDEX(y));
        int distance = sqrt(pow(x, 2) + pow(y, 2));
        world->maps[INDEX(y)][INDEX(x)] = generate_game_map(p->top, p->bottom, p->left, p->right, distance);
        free(p);
    } else {
        world->maps[INDEX(y)][INDEX(x)] = generate_game_map(X_WIDTH/2, X_WIDTH/2, Y_WIDTH/2, Y_WIDTH/2, 0);
    }
    generate_weight_maps(world->maps[INDEX(y)][INDEX(x)]);
    add_trainers(world->maps[INDEX(y)][INDEX(x)], world->trainer_map, x, y);
    add_pc(world, x, y); //Also generates distance maps

    return;
}

int display_trainers(World* world, int world_x, int world_y, int pc_x, int pc_y) {
    clear();
    mvprintw(0, 0, "Trainers on this map:\n");
    refresh();
    char trainer_array[num_trainers][100];
    int counter = 0;
    for (int y = 0; y < Y_WIDTH; y++) {
        for (int x = 0; x < X_WIDTH; x++) {
            if (x == pc_x && y == pc_y) continue;
            if (world->trainer_map->trainers[y][x] != NULL) {
                int up_down_distance = world->trainer_map->trainers[y][x]->y - pc_y;
                char* up_down_str;
                if (up_down_distance >= 0) {
                    up_down_str = "south";
                } else {
                    up_down_str = "north";
                    up_down_distance *= -1;
                }
                int left_right_distance = world->trainer_map->trainers[y][x]->x - pc_x;
                char* left_right_str;
                if (left_right_distance >= 0) {
                    left_right_str = "east";
                } else {
                    left_right_str = "west";
                    left_right_distance *= -1;
                }
                sprintf(trainer_array[counter], "%c, %d %s and %d %s", get_trainer_char(world->trainer_map->trainers[y][x]->type), up_down_distance, up_down_str, left_right_distance, left_right_str);
                counter++;
            }
        }
    }

    counter = 0;
    while (counter < Y_WIDTH + 2 && counter < num_trainers){
        mvprintw(counter + 1, 0, "%s", trainer_array[counter]);
        counter++;
        refresh();
    }
    refresh();

    int input = 0;
    while (input != 27) {
        input = getch();
        if (input == KEY_UP) {
            if (counter <= Y_WIDTH + 2) {
                continue;
            } else {
                move(Y_WIDTH + 2, 0);
                deleteln();
                move(1,0);
                insertln();
                mvprintw(1, 0, "%s", trainer_array[counter - (Y_WIDTH + 3)]);
                counter--;
                refresh();
            }
        }
        if (input == KEY_DOWN) {
            if (counter == num_trainers) {
                continue;
            } else {
                move(1,0);
                deleteln();
                mvprintw(Y_WIDTH + 2, 0, "%s", trainer_array[counter]);
                counter++;
                refresh();
            }
        }
    }
    clear();
    return 0;
}

World* create_world() {
    int i;
    World* world = (World*) malloc(sizeof(World));
    if (!world) {
        printf("Error allocating memory for world\n");
        return NULL;
    }
    world->maps = (GameMap***) malloc(sizeof(GameMap***) * SIZE); //Creates an array of pointers to maps
    if (!world->maps) {
        printf("Error allocating memory for world\n");
        return NULL;
    }
    for(i = 0; i < SIZE; i++) {
        world->maps[i] = (GameMap**)malloc(sizeof(GameMap*) * SIZE);
        if (!world->maps[i]) {
            printf("Error allocating memory for world\n");
            return NULL;
        }
        memset(world->maps[i], '\0', sizeof(GameMap*) * SIZE);
    }

    world->trainer_map = allocate_trainer_map();

    generate_map(world, 0, 0);

    return world;
}

void print_map(World* world, int x, int y, int pc_x, int pc_y) {
    int i, j;

    move(1,0);

    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            if (i == pc_x && j == pc_y) {
                printw("%c", get_trainer_char(world->pc->type));
                continue;
            }
            if (world->trainer_map->trainers[j][i] != NULL) {
                printw("%c", get_trainer_char(world->trainer_map->trainers[j][i]->type));
                continue;
            } else printw("%c", get_tile_char(world->maps[y][x]->tiles[j][i]));
        }
        printw("\n");
    }
}

void free_world(World* world) {
    if (world) {
        int i, j;
        if (world->maps) {
            for(j = 0; j < SIZE; j++) {
                for(i = 0; i < SIZE; i++) {
                    if (world->maps[j][i]) free_map(world->maps[j][i]);
                }
                free(world->maps[j]);
            }
            free(world->maps);
        }
        free(world);
    }
}