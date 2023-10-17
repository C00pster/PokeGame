#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "data_structures/priority_queue.h"
#include "map/world.h"

//Contains the x and y position of the node on the GameMap, the distance from the PC, and the weight of the last node (used for backtracking)
typedef struct {
    unsigned int row;
    unsigned int col;
    unsigned int distance;
    unsigned int last_weight;
} Node;

Node* create_node(int row, int col, unsigned int distance, unsigned int last_weight);
void generate_distance_maps(World* world, int world_x, int world_y, int pc_x, int pc_y);

#endif // DIJKSTRA_H