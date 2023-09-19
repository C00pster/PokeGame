#include "map_generation.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define X_WIDTH 80
#define Y_WIDTH 21
#define SIZE 401
#define OFFSET 200
#define INDEX(i) ((i) + OFFSET)

void print_map(char** map) {
    int i, j;
    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            printf("%c", map[j][i]);
        }
        printf("\n");
    }
}

typedef struct {
    __int8_t top, bottom, left, right;
} Path_Tracker;

Path_Tracker* get_paths(Map*** map, int x, int y) {
    Path_Tracker* path_tracker = (Path_Tracker*)malloc(sizeof(Path_Tracker));

    if (x + 1 < SIZE) {
        if (map[y][x+1]) {
            path_tracker->right = map[y][x+1]->left_path;
        } else path_tracker->right = 0;
    } else path_tracker->right = -1;
    if (x - 1 > 0) {
        if (map[y][x-1]) {
            path_tracker->left = map[y][x-1]->right_path;
        } else path_tracker->left = 0;
    } else path_tracker->left = -1;
    if (y + 1 < SIZE) {
        if (map[y+1][x]) {
            path_tracker->bottom = map[y+1][x]->top_path;
        } else path_tracker->bottom = 0;
    } else path_tracker->bottom = -1;
    if (y - 1 > 0) {
        if (map[y-1][x]) {
            path_tracker->top = map[y-1][x]->bottom_path;
        } else path_tracker->top = 0;
    } else path_tracker->top = -1;

    return path_tracker;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x = 0, y = 0;
    int i, j, k;
    char input;

    Map ***world = (Map***)malloc(sizeof(Map***) * SIZE); //Creates an array of pointers to maps
    if (!world) {
        printf("Error allocating memory for world\n");
        return 1;
    }
    for(i = 0; i < SIZE; i++) {
        world[i] = (Map**)malloc(sizeof(Map*) * SIZE);
        if (!world[i]) {
            printf("Error allocating memory for world\n");
            return 1;
        }
        memset(world[i], '\0', sizeof(Map*) * SIZE);
    }

    world[INDEX(y)][INDEX(x)] = generate_map(0, 0, 0, 0, 0); //Generates map at the origin

    print_map(world[INDEX(y)][INDEX(x)]->map); //Prints the map

    do {
        printf("You are at (%d,%d)\tEnter a command: ", x, y);
        input = getc(stdin);
        printf("\n");

        switch (input) {
            case 'n':
                if (INDEX(y+1) >= SIZE) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    y++;
                    if (world[INDEX(y)][INDEX(x)] == NULL) {
                        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
                        int distance = sqrt(pow(x, 2) + pow(y, 2));
                        world[INDEX(y)][INDEX(x)] = generate_map(p->top, p->bottom, p->left, p->right, distance);
                        free(p);
                    }
                    print_map(world[INDEX(y)][INDEX(x)]->map);
                }
                break;
            
            case 's':
                if (INDEX(y-1) < 0) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    y--;
                    if (!world[INDEX(y)][INDEX(x)]) {
                        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
                        int distance = sqrt(pow(x, 2) + pow(y, 2));
                        world[INDEX(y)][INDEX(x)] = generate_map(p->top, p->bottom, p->left, p->right, distance);
                        free(p);
                    }
                    print_map(world[INDEX(y)][INDEX(x)]->map);
                }
                break;

            case 'e':
                if (INDEX(x+1) >= SIZE) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    x++;
                    if (!world[INDEX(y)][INDEX(x)]) {
                        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
                        int distance = sqrt(pow(x, 2) + pow(y, 2));
                        world[INDEX(y)][INDEX(x)] = generate_map(p->top, p->bottom, p->left, p->right, distance);
                        free(p);
                    }
                    print_map(world[INDEX(y)][INDEX(x)]->map);
                }
                break;

            case 'w':
                if (INDEX(x-1) < 0) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    x--;
                    if (!world[INDEX(y)][INDEX(x)]) {
                        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
                        int distance = sqrt(pow(x, 2) + pow(y, 2));
                        world[INDEX(y)][INDEX(x)] = generate_map(p->top, p->bottom, p->left, p->right, distance);
                        free(p);
                    }
                    print_map(world[INDEX(y)][INDEX(x)]->map);
                }
                break;

            case 'f':
                int x_fly, y_fly;
                scanf(" %d %d", &x_fly, &y_fly);
                if (INDEX(x_fly) < 0 || INDEX(x_fly) >= SIZE || INDEX(y_fly) < 0 || INDEX(y_fly) >= SIZE) {
                    printf("Invalid coordinates\n");
                    break;
                } else {
                    x = x_fly;
                    y = y_fly;
                    if (!world[INDEX(y)][INDEX(x)]) {
                        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
                        int distance = sqrt(pow(x, 2) + pow(y, 2));
                        world[INDEX(y)][INDEX(x)] = generate_map(p->top, p->bottom, p->left, p->right, distance);
                        free(p);
                    }
                    print_map(world[INDEX(y)][INDEX(x)]->map);
                }
                break;

            default:
                printf("Invalid command\n");
                break;
        }
        while (getc(stdin) != '\n'); //Clears stdin
    } while (input != 'q');

    //Memory cleanup
    if (world == NULL) return 0;
    for (i = 0; i < SIZE; i++) {
        if (world[i] == NULL) continue;
        for (j = 0; j < SIZE; j++) {
            if (world[i][j] == NULL) continue;
            for (k = 0; k < Y_WIDTH; k++) {
                if (world[i][j]->map[k] != NULL) free(world[i][j]->map[k]);
            }
        }
        free(world[i]);
    }
    free(world);

    return 0;
}