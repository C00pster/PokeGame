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
    GATE
} TileType;

typedef struct {
    TileType type;
    __int8_t x, y;
    TrainerType trainer;
} Tile;

Tile* create_tile(TileType type, __int8_t x, __int8_t y);
char get_tile_char(Tile* tile);

#endif // TILE_H