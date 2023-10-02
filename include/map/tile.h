#include <stdlib.h>
#include "../character.h"

#ifndef TILE_H
#define TILE_H

typedef enum {
    BOULDER,
    TREE,
    PATH,
    PMART,
    PCNTR,
    TGRASS,
    SGRASS,
    MOUNTAIN,
    FOREST,
    WATER,
    GATE,
    NUM_TILES
} TileType;

typedef struct {
    TileType type;
    __int8_t x, y;
    TrainerType trainer;
} Tile;

Tile* create_tile(TileType type, __int8_t x, __int8_t y);
void free_tile(Tile* tile);
char get_tile_char(Tile* tile);
int get_tile_weight(Tile* tile, TrainerType trainer);

#endif // TILE_H