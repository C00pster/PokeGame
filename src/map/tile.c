#include <stdlib.h>

#include "map/tile.h"
#include "character.h"

Tile* create_tile(TileType type, __int8_t x, __int8_t y) {
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->type = type;
    tile->x = x;
    tile->y = y;
    tile->trainer = NUM_TRAINERS;

    return tile;
}

char get_tile_char(Tile* tile) {
    if (tile->trainer != NUM_TRAINERS) {
        return '@';
    }
    char result;
    switch(tile->type) {
        case BOULDER:
        case MOUNTAIN:
            result = '%';
            break;
        case TREE:
        case FOREST:
            result = '^';
            break;
        case PATH:
        case GATE:
            result = '#';
            break;
        case PMART:
            result = 'M';
            break;
        case PCNTR:
            result = 'C';
            break;
        case TGRASS:
            result = ':';
            break;
        case SGRASS:
            result = '.';     
            break;   
        case WATER:
            result = '~';
            break;
        case NUM_TILES:
            exit(1);
    }
    return result;
}

void free_tile(Tile* tile) {
    free(tile);
}