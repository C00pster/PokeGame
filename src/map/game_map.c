#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

#include "data_structures/queue.h"
#include "map/game_map.h"
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

unsigned int*** allocate_tile_weight_maps() {
    int i, j;
    unsigned int*** tile_weight_maps = (unsigned int***) malloc(NUM_TRAINERS * sizeof(unsigned int**));
    if (!tile_weight_maps) {
        printf("Error allocating memory for tile_weight_maps\n");
        return NULL;
    }
    for (i = 0; i < NUM_TRAINERS; i++) {
        tile_weight_maps[i] = (unsigned int**) malloc(Y_WIDTH * sizeof(unsigned int*));
        if (!tile_weight_maps[i]) {
            printf("Error allocating memory for tile_weight_maps\n");
            return NULL;
        }
        for (j = 0; j < Y_WIDTH; j++) {
            tile_weight_maps[i][j] = (unsigned int*) malloc(X_WIDTH * sizeof(unsigned int));
            if (!tile_weight_maps[i][j]) {
                printf("Error allocating memory for tile_weight_maps\n");
                return NULL;
            }
        }
    }

    return tile_weight_maps;
}

GameMap* allocate_game_map() {
    GameMap* game_map = (GameMap*) malloc(sizeof(GameMap));
    game_map->tiles = allocate_map();
    game_map->tile_weight_maps = allocate_tile_weight_maps();

    return game_map;
}

GameMap* generate_regions(GameMap* game_map, TileType* starter_tile_types, __uint8_t start_tile_type_length) {
    __int8_t i,j;
    Queue* q = create_queue(ARR_MAX);

    //Generates random starter points
    for (i = 0; i < start_tile_type_length; i++) {
        Point* new_point = generate_point(1, X_WIDTH-2, 1, Y_WIDTH-2);
        game_map->tiles[new_point->y][new_point->x] = create_tile(starter_tile_types[i], new_point->x, new_point->y); //starter_char_vals[i];
        enqueue(q, new_point);
    }

    //Sets boulder border
    for (i = 0; i < X_WIDTH; i++) {
        game_map->tiles[0][i] = create_tile(BOULDER, i, 0);
        game_map->tiles[Y_WIDTH-1][i] = create_tile(BOULDER, i, Y_WIDTH-1);
    }
    for (i = 1; i < Y_WIDTH - 1; i++) {
        game_map->tiles[i][0] = create_tile(BOULDER, 0, i);
        game_map->tiles[i][X_WIDTH-1] = create_tile(BOULDER, X_WIDTH-1, i);
    }

    //Fills map in from starter points
    while (q->size > 0) {
        Point* p = dequeue(q);
        Tile* t = game_map->tiles[p->y][p->x];

        for (j = -1; j <= 1; j++) {
            for (i = -2; i <= 2; i++) {
                Point* new_point = create_point(p->x + i, p->y + j);
                if (new_point->y > 0 && new_point->y < Y_WIDTH-1 && new_point->x > 0 && new_point->x < X_WIDTH - 1) {
                    if (!game_map->tiles[new_point->y][new_point->x]) {
                        game_map->tiles[new_point->y][new_point->x] = create_tile(t->type, new_point->x, new_point->y);
                        enqueue(q, new_point);
                    }
                }
            }
        }
        free(p);
    }

    destroy_queue(q);
    return game_map;
}

GameMap* generate_game_map(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    TileType starter_tile_types[7] = {TGRASS, SGRASS, FOREST, TGRASS, SGRASS, MOUNTAIN, TGRASS};
    __uint8_t start_tile_type_length = 7;

    GameMap* m = allocate_game_map();
    generate_regions(m, starter_tile_types, start_tile_type_length);
    if (!m->tiles) {
        printf("Error generating regions\n");
        return NULL;
    }
    generate_path(m, top_path, bottom_path, left_path, right_path, distance);

    return m;
}

void free_map(GameMap* game_map) {
    int i, j;
    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            free_tile(game_map->tiles[j][i]);
        }
        free(game_map->tiles[j]);
    }
    free(game_map->tiles);
    for (j = PC; j < NUM_TRAINERS; j++) {
        for (i = 0; i < Y_WIDTH; i++) {
            free(game_map->tile_weight_maps[j][i]);
        }
    }
    free(game_map);
}