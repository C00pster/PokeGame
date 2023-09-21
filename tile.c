#include "tile.h"
#include <stdlib.h>

Tile* create_tile(TileType type, __int8_t x_coor, __int8_t y_coor) {
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->type = type;
    tile->x = x_coor;
    tile->y = y_coor;

    return tile;
}

char get_tile_char(TileType type) {
    char result;
    switch(type) {
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
    }
    return result;
}