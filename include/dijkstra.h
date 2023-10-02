#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "data_structures/priority_queue.h"
#include "map/map_generation.h"
#include "character.h"
#include "limits.h"

typedef struct {
    unsigned int row;
    unsigned int col;
    unsigned int distance;
    unsigned int last_weight;
} Node;

Node* create_node(int row, int col, unsigned int distance, unsigned int last_weight);
unsigned int** generate_distance_map(Map* map, int x, int y, TrainerType type);

#endif // DIJKSTRA_H