#include "terrain_generation.h"
#include <stdio.h>
#include <time.h>

#define x_width 80
#define y_width 21

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int i, j;

    char** map = generate_terrain();

    //Prints map
    for (j = 0; j < y_width; j++) {
        for (i = 0; i < x_width; i++) {
            printf("%c", map[j][i]);
        }
        printf("\n");
    }

    return 0;
}