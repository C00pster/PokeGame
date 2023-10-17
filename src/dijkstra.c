#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "dijkstra.h"
#include "config.h"

Node* create_node(int x, int y, unsigned int distance, unsigned int last_weight) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->row = y;
    node->col = x;
    node->distance = distance;
    node->last_weight = last_weight;
    return node;
}

bool is_valid(int x, int y) {
    return (x >= 0 && x < X_WIDTH && y >= 0 && y < Y_WIDTH);
}

int compare(void* a, void* b) {
    Node* nodeA = (Node*) a;
    Node* nodeB = (Node*) b;

    return nodeA->distance - nodeB->distance;
}

unsigned int safe_add(unsigned int a, unsigned int b) {
    if (b > 0 && a > UINT_MAX - b) {
        return UINT_MAX;
    } else {
        return a + b;
    }
}

void dijkstra(unsigned int startCol, unsigned int startRow, unsigned int** distances, int weights[Y_WIDTH][X_WIDTH]){
    int i, j;
    unsigned int x, y, last_weight;
    bool visited[Y_WIDTH][X_WIDTH] = {false};
    distances[startRow][startCol] = 0;

    PriorityQueue* pq = create_priority_queue(X_WIDTH * Y_WIDTH, compare);
    Node* startNode = create_node(startCol, startRow, 0, 10);
    push(pq, startNode);

    while (pq->size > 0) {
        Node* current = (Node*) pop(pq);
        y = current->row;
        x = current->col;

        if (visited[y][x]) continue;
        visited[y][x] = true;
        last_weight = current->last_weight;

        for (j = -1; j <= 1; j++) {
            for (i = -1; i <= 1; i++) {
                int newX = x + i;
                int newY = y + j;
                if (is_valid(newX, newY) && !visited[newY][newX]) {
                    int newDistance = safe_add(current->distance, last_weight);
                    if (newDistance < distances[newY][newX]) {
                        distances[newY][newX] = newDistance;
                        Node* newNode = create_node(newX, newY, newDistance, weights[newY][newX]);
                        push(pq, newNode);
                    }
                }
            }
        }
        free(current);
    }

    free_priority_queue(pq);
}

unsigned int** generate_distance_map(GameMap* game_map, int pc_x, int pc_y, TrainerType type) {
    int i, j;
    int tile_weights[NUM_TILES];
    get_trainer_weights(type, tile_weights);
    unsigned int** distances = malloc(sizeof(int*) * Y_WIDTH);
    for (i = 0; i < Y_WIDTH; i++) {
        distances[i] = malloc(sizeof(int) * X_WIDTH);
        for (j = 0; j < X_WIDTH; j++) {
            distances[i][j] = INT_MAX;
        }
    }

    int travel_weights[Y_WIDTH][X_WIDTH];

    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            travel_weights[j][i] = tile_weights[game_map->tiles[j][i]->type];
        }
    }

    dijkstra(pc_x, pc_y, distances, travel_weights);

    return distances;
}

void generate_distance_maps(World* world, int world_x, int world_y, int pc_x, int pc_y) {
    int i;
    world->trainer_map->distance_maps = malloc(sizeof(unsigned int**) * NUM_TRAINERS);
    for (i = 1; i < NUM_TRAINERS; i++) {
        world->trainer_map->distance_maps[i] = generate_distance_map(world->maps[INDEX(world_y)][INDEX(world_x)], pc_x, pc_y, i);
    }
}