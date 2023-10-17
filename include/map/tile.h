#ifndef TILE_H
#define TILE_H

#include <stdint.h>

//Represents the different types of tiles
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

//Contains the type of tile and the x and y coordinates of the tile in the GameMap (game_map.h)
typedef struct {
    TileType type;
    int x, y;
} Tile;

Tile* create_tile(TileType type, int x, int y);
void free_tile(Tile* tile);
char get_tile_char(Tile* tile);

#endif // TILE_H