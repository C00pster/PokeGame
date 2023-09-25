#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "data_structures/priority_queue.h"
#include "map/map_generation.h"
#include "character.h"
#include "limits.h"

typedef struct {
    int row;
    int col;
    int distance;
} Node;

Node* create_node(int row, int col, int distance);
int** generate_distance_map(Map* map, int x, int y, TrainerType type);

#endif // DIJKSTRA_H