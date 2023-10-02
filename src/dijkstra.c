#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "dijkstra.h"
#include "data_structures/priority_queue.h"

Node* create_node(int x, int y, unsigned int distance, unsigned int last_weight) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->row = y;
    node->col = x;
    node->distance = distance;
    node->last_weight = last_weight;
    return node;
}

bool isValid(int x, int y) {
    return (x >= 0 && x < X_WIDTH && y >= 0 && y < Y_WIDTH);
}

int compare(void* a, void* b) {
    Node* nodeA = (Node*) a;
    Node* nodeB = (Node*) b;

    return nodeA->distance - nodeB->distance;
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
                if (isValid(newX, newY) && !visited[newY][newX]) {
                    int newDistance = current->distance + last_weight;
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

unsigned int** generate_distance_map(Map* map, int x, int y, TrainerType type) {
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
            travel_weights[j][i] = tile_weights[map->map[j][i]->type];
        }
    }

    dijkstra(x, y, distances, travel_weights);

    return distances;
}