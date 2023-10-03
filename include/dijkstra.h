#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "data_structures/priority_queue.h"
#include "map/game_map.h"
#include "character.h"
#include "limits.h"

typedef struct {
    unsigned int row;
    unsigned int col;
    unsigned int distance;
    unsigned int last_weight;
} Node;

Node* create_node(int row, int col, unsigned int distance, unsigned int last_weight);
void generate_distance_maps(World* world, int world_x, int world_y, int pc_x, int pc_y);

#endif // DIJKSTRA_H