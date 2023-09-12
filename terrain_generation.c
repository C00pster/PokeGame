#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"

#define ARR_MAX 800
#define X_WIDTH 80
#define Y_WIDTH 21

char** generate_regions(char* starter_char_vals, __uint8_t start_char_arr_length) {
    __int8_t i,j;
    //TODO - creates memory leak
    char** map = (char**)malloc(Y_WIDTH * sizeof(char*));
    if (!map) {
        printf("Error allocating memory for map\n");
        return NULL;
    }
    for (i = 0; i < Y_WIDTH; i++) {
        map[i] = malloc (X_WIDTH * sizeof(char));
        if (!map[i]) {
            printf("Error allocating memory for map\n");
            return NULL;
        }
    }
    Queue* q = create_queue(ARR_MAX);

    //Generates random starter points
    for (i = 0; i < start_char_arr_length; i++) {
        Point* new_point = generate_point(1, X_WIDTH-2, 1, Y_WIDTH-2);
        map[new_point->y][new_point->x] = starter_char_vals[i];
        enqueue(q, new_point);
    }

    //Sets mountain border
    for (i = 0; i < X_WIDTH; i++) {
        map[0][i] = '%';
        map[Y_WIDTH-1][i] = '%';
    }
    for (i = 1; i < Y_WIDTH - 1; i++) {
        map[i][0] = '%';
        map[i][X_WIDTH-1] = '%';
    }

    //Fills map in from starter points
    while (q->size > 0) {
        Point* p = dequeue(q);
        char c = map[p->y][p->x];

        for (j = -1; j < 2; j++) {
            for (i = -1; i < 2; i++) {
                Point* new_point = create_point(p->x + i, p->y + j);
                if (new_point->y > 0 && new_point->y < Y_WIDTH-1 && new_point->x > 0 && new_point->x < X_WIDTH - 1) {
                    if (map[new_point->y][new_point->x] == 0) {
                        map[new_point->y][new_point->x] = c;
                        enqueue(q, new_point);
                    }
                }
            }
        }
    }

    return map;
}

void generate_path(char** map) {
     __int8_t left_road, top_road, bottom_road, right_road, vertical_path_col, horizontal_path_row, bottom_greater, left_greater;
    __int8_t pokimart_val, pokicenter_val, i, j;

    //Sets the entrance coordinates, vertical, and horizontal path cols/rows
    left_road = 5 + rand() % (Y_WIDTH - 10);
    right_road = 5 + rand() % (Y_WIDTH - 10);
    top_road = 5 + rand() % (X_WIDTH - 10);
    bottom_road = 5 + rand() % (X_WIDTH - 10);
    if (left_road ==  right_road) {
        horizontal_path_row = left_road;
        left_greater = 0;
    } else if (left_road > right_road) {
        horizontal_path_row = right_road + rand() % (left_road - right_road + 1);
        left_greater = 1;
    } else {
        horizontal_path_row = left_road + rand() % (right_road - left_road + 1);
        left_greater = -1;
    }
    if (top_road == bottom_road) {
        vertical_path_col = top_road;
        bottom_greater = 0;
    } else if (top_road > bottom_road) {
        vertical_path_col = bottom_road + rand() % (top_road - bottom_road + 1);
        bottom_greater = -1;
    } else {
        vertical_path_col = top_road + rand() % (bottom_road - top_road + 1);
        bottom_greater = 1;
    }

    //Creates a 3 tile long path out from each entrance
    for (i = 0; i < 3; i++) {
        map[i][bottom_road] = '#';
        map[Y_WIDTH - i - 1][top_road] = '#';
        map[left_road][i] = '#';
        map[right_road][X_WIDTH - i - 1] = '#';
    }

    //Creates a path from the path entrance column or row to the vertical or horizontal cross
    if (left_greater == 1) {
        while (left_road != horizontal_path_row) {
            map[left_road][3] = '#';
            left_road--;
        }
        while (right_road != horizontal_path_row) {
            map[right_road][X_WIDTH-4] = '#';
            right_road++;
        }
    } else if (left_greater == -1) {
        while (left_road != horizontal_path_row) {
            map[left_road][3] = '#';
            left_road++;
        }
        while (right_road != horizontal_path_row) {
            map[right_road][X_WIDTH-4] = '#';
            right_road--;
        }
    }
    if (bottom_greater == 1) {
        while (bottom_road != vertical_path_col) {
            map[3][bottom_road] = '#';
            bottom_road--;
        }
        while (top_road != vertical_path_col) {
            map[Y_WIDTH-4][top_road] = '#';
            top_road++;
        }
    } else if (bottom_greater == -1) {
        while (bottom_road != vertical_path_col) {
            map[3][bottom_road] = '#';
            bottom_road++;
        }
        while (top_road != vertical_path_col) {
            map[Y_WIDTH-4][top_road] = '#';
            top_road--;
        }
    }

    //Creates perpendicular paths across the map to connect all entrances
    for (i = 3; i < X_WIDTH - 3; i++) {
        map[horizontal_path_row][i] = '#';
    }
    for (i = 3; i < Y_WIDTH - 3; i++) {
        map[i][vertical_path_col] = '#';
    }

    //Picks spot for pokimart and center
    pokicenter_val = horizontal_path_row;
    pokimart_val = vertical_path_col;
    while (pokicenter_val - horizontal_path_row < 1 && pokicenter_val - horizontal_path_row > -3) {
        pokicenter_val = 4 + rand() % (Y_WIDTH - 4 - 6 + 1);
    }
    while (abs(pokimart_val - vertical_path_col) < 2) {
        pokimart_val = 4 + rand() % (X_WIDTH - 4 - 6 + 1);
    }

    i = horizontal_path_row < (Y_WIDTH / 2) ? 1 : -1;
    j = vertical_path_col < (X_WIDTH / 2) ? 1 : -1;

    map[horizontal_path_row + (1 * i)][pokimart_val] = 'M';
    map[horizontal_path_row + (1 * i)][pokimart_val + 1] = 'M';
    map[horizontal_path_row + (2 * i)][pokimart_val] = 'M';
    map[horizontal_path_row + (2 * i)][pokimart_val + 1] = 'M';
    

    map[pokicenter_val][vertical_path_col + (1 * j)] = 'C';
    map[pokicenter_val][vertical_path_col + (2 * j)] = 'C';
    map[pokicenter_val + 1][vertical_path_col + (1 * j)] = 'C';
    map[pokicenter_val + 1][vertical_path_col + (2 * j)] = 'C';

    return;
}

char** generate_terrain() {
    char starter_char_vals[6] = {':',':','^','%','.','.'};
    __uint8_t start_char_arr_length = 6;

    char** map = generate_regions(starter_char_vals, start_char_arr_length);
    generate_path(map);

    return map;
}