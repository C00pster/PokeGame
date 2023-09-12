#include "terrain_generation.h"
#include <stdio.h>
#include <time.h>

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

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x, y = 0;

    char ****world = (char****)malloc(sizeof(char***) * SIZE); //Creates an array of pointers to 2d arrays of characters
    if (!world) {
        printf("Error allocating memory for world\n");
        return 1;
    }

    world[INDEX(y)][INDEX(x)] = generate_terrain(); //Generates terrain at the origin

    print_map(world[INDEX(y)][INDEX(x)]); //Prints the map

    return 0;
}