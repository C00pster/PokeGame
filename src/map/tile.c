#include <stdlib.h>
#include <ncurses.h>

#include "map/tile.h"
#include "trainer.h"

Tile* create_tile(TileType type, int x, int y) {
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->type = type;
    tile->x = x;
    tile->y = y;

    return tile;
}

char get_tile_char(Tile* tile) {
    switch(tile->type) {
        case BOULDER:
        case MOUNTAIN:
            return '%';
        case TREE:
        case FOREST:
            return '^';
        case PATH:
        case GATE:
            return '#';
        case PMART:
            return 'M';
        case PCNTR:
            return 'C';
        case TGRASS:
            return ':';
        case SGRASS:
            return '.';       
        case WATER:
            return '~';
        case NUM_TILES:
            exit(1);
    }
    return '!'; // This should never happen
}

void free_tile(Tile* tile) {
    free(tile);
}