#include <stdlib.h>

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
} Tile;

Tile* create_tile(TileType type, __int8_t x, __int8_t y);
char get_tile_char(TileType type);

#endif // TILE_H