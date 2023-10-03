#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

#include "data_structures/queue.h"
#include "map/world.h"
#include "map/game_map.h"
#include "map/tile.h"
#include "map/path.h"
#include "dijkstra.h"
#include "config.h"

#define ARR_MAX 800

Tile*** allocate_map() {
    __int8_t i;
    Tile*** map = (Tile***)malloc(Y_WIDTH * sizeof(Tile**));
    if (!map) {
        printf("Error allocating memory for map\n");
        return NULL;
    }
    for (i =0; i < Y_WIDTH; i++) {
        map[i] = malloc (X_WIDTH * sizeof(Tile*));
        if (!map[i]) {
            printf("Error allocating memory for map\n");
            return NULL;
        }
        memset(map[i], '\0', X_WIDTH * sizeof(Tile*));
    }

    return map;
}

Tile*** generate_regions(TileType* starter_tile_types, __uint8_t start_tile_type_length) {
    __int8_t i,j;
    Tile*** map = allocate_map();
    Queue* q = create_queue(ARR_MAX);

    //Generates random starter points
    for (i = 0; i < start_tile_type_length; i++) {
        Point* new_point = generate_point(1, X_WIDTH-2, 1, Y_WIDTH-2);
        map[new_point->y][new_point->x] = create_tile(starter_tile_types[i], new_point->x, new_point->y); //starter_char_vals[i];
        enqueue(q, new_point);
    }

    //Sets boulder border
    for (i = 0; i < X_WIDTH; i++) {
        map[0][i] = create_tile(BOULDER, i, 0);
        map[Y_WIDTH-1][i] = create_tile(BOULDER, i, Y_WIDTH-1);
    }
    for (i = 1; i < Y_WIDTH - 1; i++) {
        map[i][0] = create_tile(BOULDER, 0, i);
        map[i][X_WIDTH-1] = create_tile(BOULDER, X_WIDTH-1, i);
    }

    //Fills map in from starter points
    while (q->size > 0) {
        Point* p = dequeue(q);
        Tile* t = map[p->y][p->x];

        for (j = -1; j <= 1; j++) {
            for (i = -2; i <= 2; i++) {
                Point* new_point = create_point(p->x + i, p->y + j);
                if (new_point->y > 0 && new_point->y < Y_WIDTH-1 && new_point->x > 0 && new_point->x < X_WIDTH - 1) {
                    if (!map[new_point->y][new_point->x]) {
                        map[new_point->y][new_point->x] = create_tile(t->type, new_point->x, new_point->y);
                        enqueue(q, new_point);
                    }
                }
            }
        }
        free(p);
    }

    destroy_queue(q);
    return map;
}

GameMap* generate_terrain(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    TileType starter_tile_types[7] = {TGRASS, SGRASS, FOREST, TGRASS, SGRASS, MOUNTAIN, TGRASS};
    __uint8_t start_tile_type_length = 7;

    GameMap* m = (GameMap*) malloc(sizeof(GameMap));
    if (!m) {
        printf("Error allocating memory for GameMap\n");
        return NULL;
    }
    m->tiles = generate_regions(starter_tile_types, start_tile_type_length);
    if (!m->tiles) {
        printf("Error generating regions\n");
        return NULL;
    }
    generate_path(m, top_path, bottom_path, left_path, right_path, distance);

    return m;
}

Point* add_pc(World* world, int x, int y) {
    GameMap* m = world->maps[INDEX(y)][INDEX(x)];
    int x_coor, y_coor;

    if (rand() % 2 == 1) {
        y_coor = m->horizontal_path_row;
        x_coor = rand() % (X_WIDTH - 6) + 3;
    } else {
        y_coor = rand() % (Y_WIDTH - 6) + 3;
        x_coor = m->vertical_path_col;
    }

    m->tiles[y_coor][x_coor]->trainer = PC;

    Point* p = create_point(x_coor, y_coor);

    return p;
}

/*
Creates a map if it doesn't exist. Adds trainers to created map
*/
void generate_map(World* world, int x, int y) {
    if (x != 0 || y != 0) {
        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
        int distance = sqrt(pow(x, 2) + pow(y, 2));
        world->maps[INDEX(y)][INDEX(x)] = generate_terrain(p->top, p->bottom, p->left, p->right, distance);
        free(p);
    } else {
        world->maps[INDEX(y)][INDEX(x)] = generate_terrain(X_WIDTH/2, X_WIDTH/2, Y_WIDTH/2, Y_WIDTH/2, 0);
    }
    Point* pc = add_pc(world, x, y);

    generate_distance_maps(world, x, y, pc->x, pc->y);

    free(pc);
}

void print_map(GameMap* game_map) {
    int i, j;

    clear();

    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            printw("%c", get_tile_char(game_map->tiles[j][i]));
        }
        printw("\n");
    }
    refresh();
}

void free_map(GameMap* game_map) {
    int i, j;
    if (!game_map) return;
    if (!game_map->tiles) {
        for (j = 0; j < Y_WIDTH; j++) {
            for (i = 0; i < X_WIDTH; i++) {
                free_tile(game_map->tiles[j][i]);
            }
        }
        free(game_map->tiles);
    }
    free(game_map);
}