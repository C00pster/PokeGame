#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "dijkstra.h"
#include "data_structures/priority_queue.h"

Node* create_node(int row, int col, int distance) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->row = row;
    node->col = col;
    node->distance = distance;
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

void dijkstra(int startCol, int startRow, int** distances, int weights[Y_WIDTH][X_WIDTH]){
    int i, j, x, y;
    bool visited[Y_WIDTH][X_WIDTH] = {false};
    distances[startRow][startCol] = weights[startRow][startCol];

    PriorityQueue* pq = create_priority_queue(X_WIDTH * Y_WIDTH, compare);
    Node* startNode = create_node(startRow, startCol, distances[startRow][startCol]);
    push(pq, startNode);

    while (pq->size > 0) {
        Node* current = (Node*) pop(pq);
        x = current->row;
        y = current->col;

        if (visited[y][x]) continue;
        visited[y][x] = true;

        for (j = -1; j <= 1; j++) {
            for (i = -1; i <= 1; i++) {
                int newX = x + i;
                int newY = y + j;
                if (isValid(newX, newY) && !visited[newY][newX]) {
                    int newDistance = current->distance + weights[newY][newX];
                    if (newDistance < distances[newY][newX]) {
                        distances[newY][newX] = newDistance;
                        Node* newNode = create_node(newX, newY, newDistance);
                        push(pq, newNode);
                    }
                }
            }
        }
        free(current);
    }

    free_priority_queue(pq);
}

int** generate_distance_map(Map* map, int x, int y, TrainerType type) {
    int i, j;
    int* tile_weights = get_trainer_weights(type);
    int** distances = malloc(sizeof(int*) * Y_WIDTH);
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