#include <stdio.h>
#include <string.h>

#include "map/world.h"
#include "map/game_map.h"
#include "config.h"

World* create_world() {
    int i;
    World* world = (World*)malloc(sizeof(World));
    if (!world) {
        printf("Error allocating memory for world\n");
        return NULL;
    }
    world->maps = (GameMap***)malloc(sizeof(GameMap***) * SIZE); //Creates an array of pointers to maps
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

    int x = 0, y = 0;
    generate_map(world, x, y);

    return world;
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