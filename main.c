#include "map_generation.h"
#include "tile.h"
#include "config.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

void print_map(Map*** world, int x, int y) {
    x += OFFSET;
    y += OFFSET;
    int i, j;
    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            printf("%c", get_tile_char(world[y][x]->map[j][i]->type));
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x = 0, y = 0;
    int i, j, k;
    char input;

    Map ***world = create_world();
    if (!world) {
        printf("Error allocating memory for world\n");
        return 1;
    }
    
    print_map(world, x, y); //Prints the map

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
                    if (!world[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world, x, y);
                }
                break;
            case 's':
                if (INDEX(y-1) < 0) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    y--;
                    if (!world[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world, x, y);
                }
                break;
            case 'e':
                if (INDEX(x+1) >= SIZE) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    x++;
                    if (!world[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world, x, y);
                }
                break;
            case 'w':
                if (INDEX(x-1) < 0) {
                    printf("You can't go that way\n");
                    break;
                } else {
                    x--;
                    if (!world[INDEX(y)][INDEX(x)]) {
                        generate_map(world, x, y);
                    }
                    print_map(world, x, y);
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
                        generate_map(world, x, y);
                    }
                    print_map(world, x, y);
                }
                break;
            case 'q':
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