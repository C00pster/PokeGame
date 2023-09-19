#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "queue.h"
#include "map_generation.h"

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
        memset(map[i], '\0', X_WIDTH);
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

        for (j = -1; j <= 1; j++) {
            for (i = -2; i <= 2; i++) {
                Point* new_point = create_point(p->x + i, p->y + j);
                if (new_point->y > 0 && new_point->y < Y_WIDTH-1 && new_point->x > 0 && new_point->x < X_WIDTH - 1) {
                    if (!map[new_point->y][new_point->x]) {
                        map[new_point->y][new_point->x] = c;
                        enqueue(q, new_point);
                    }
                }
            }
        }
    }

    return map;
}

void generate_path(Map* m, __int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    __int8_t vertical_path_col, horizontal_path_row, bottom_greater, left_greater;
    __int8_t pokimart_val, pokicenter_val, i, j;

    if (top_path == 0) {
        top_path = 5 + rand() % (X_WIDTH - 10);
    }
    if (bottom_path == 0) {
        bottom_path = 5 + rand() % (X_WIDTH - 10);
    }
    if (left_path == 0) {
        left_path = 5 + rand() % (Y_WIDTH - 10);
    }
    if (right_path == 0) {
        right_path = 5 + rand() % (Y_WIDTH - 10);
    }
    m->top_path = top_path;
    m->bottom_path = bottom_path;
    m->left_path = left_path;
    m->right_path = right_path;

    //Sets the entrance coordinates, vertical, and horizontal path cols/rows
    if (left_path == right_path) {
        horizontal_path_row = left_path;
        left_greater = 0;
    } else if (left_path == -1 || right_path == -1){
        horizontal_path_row = left_path == -1 ? right_path : left_path;
        left_greater = 0;
    } else if (left_path > right_path) {
        horizontal_path_row = right_path + rand() % (left_path - right_path + 1);
        left_greater = 1;
    } else {
        horizontal_path_row = left_path + rand() % (right_path - left_path + 1);
        left_greater = -1;
    }
    if (top_path == bottom_path) {
        vertical_path_col = top_path;
        bottom_greater = 0;
    } else if (bottom_path == -1 || top_path == -1) {
        vertical_path_col = bottom_path == -1 ? top_path : bottom_path;
        bottom_greater = 0;
    } else if (top_path > bottom_path) {
        vertical_path_col = bottom_path + rand() % (top_path - bottom_path + 1);
        bottom_greater = -1;
    } else {
        vertical_path_col = top_path + rand() % (bottom_path - top_path + 1);
        bottom_greater = 1;
    }

    //Creates a 3 tile long path out from each entrance
    for (i = 0; i < 3; i++) {
        if (bottom_path != -1) m->map[i][bottom_path] = '#';
        if (top_path != -1) m->map[Y_WIDTH - i - 1][top_path] = '#';
        if (left_path != -1) m->map[left_path][i] = '#';
        if (right_path != -1) m->map[right_path][X_WIDTH - i - 1] = '#';
    }

    //Creates a path from the path entrance column or row to the vertical or horizontal cross
    if (left_greater == 1) {
        while (left_path != horizontal_path_row) {
            m->map[left_path][3] = '#';
            left_path--;
        }
        while (right_path != horizontal_path_row) {
            m->map[right_path][X_WIDTH-4] = '#';
            right_path++;
        }
    } else if (left_greater == -1) {
        while (left_path != horizontal_path_row) {
            m->map[left_path][3] = '#';
            left_path++;
        }
        while (right_path != horizontal_path_row) {
            m->map[right_path][X_WIDTH-4] = '#';
            right_path--;
        }
    }
    if (bottom_greater == 1) {
        while (bottom_path != vertical_path_col) {
            m->map[3][bottom_path] = '#';
            bottom_path--;
        }
        while (top_path != vertical_path_col) {
            m->map[Y_WIDTH-4][top_path] = '#';
            top_path++;
        }
    } else if (bottom_greater == -1) {
        while (bottom_path != vertical_path_col) {
            m->map[3][bottom_path] = '#';
            bottom_path++;
        }
        while (top_path != vertical_path_col) {
            m->map[Y_WIDTH-4][top_path] = '#';
            top_path--;
        }
    }

    //Creates perpendicular paths across the map to connect all entrances
    i = left_path == -1 ? vertical_path_col : 3; //If there is not a left path, start at the vertical path col
    j = right_path == -1 ? vertical_path_col : X_WIDTH - 3; //If there is not a right path, end at the vertical path col
    for (; i < j; i++) {
        m->map[horizontal_path_row][i] = '#';
    }
    i = bottom_path == -1 ? horizontal_path_row : 3; //If there is not a bottom path, start at the horizontal path row
    j = top_path == -1 ? horizontal_path_row : Y_WIDTH - 3; //If there is not a top path, end at the horizontal path row
    for (; i < j; i++) {
        m->map[i][vertical_path_col] = '#';
    }

    //Picks spot for pokimart and center
    int generation_chance = 0;
    if (distance == 0) {
        generation_chance = 100;
    } else if (distance > 200) {
        generation_chance = 5;
    } else generation_chance = (((-45 * distance) / 200) + 50);
    if (rand() % 100 < generation_chance) {
        pokicenter_val = horizontal_path_row;
        while (pokicenter_val - horizontal_path_row < 1 && pokicenter_val - horizontal_path_row > -3) {
            pokicenter_val = 4 + rand() % (Y_WIDTH - 4 - 6 + 1);
        }
        i = vertical_path_col < (X_WIDTH / 2) ? 1 : -1;

        m->map[pokicenter_val][vertical_path_col + (1 * i)] = 'C';
        m->map[pokicenter_val][vertical_path_col + (2 * i)] = 'C';
        m->map[pokicenter_val + 1][vertical_path_col + (1 * i)] = 'C';
        m->map[pokicenter_val + 1][vertical_path_col + (2 * i)] = 'C';
    }
    if (rand() % 100 < generation_chance) {
        pokimart_val = vertical_path_col;
        while (abs(pokimart_val - vertical_path_col) < 2) {
            pokimart_val = 4 + rand() % (X_WIDTH - 4 - 6 + 1);
        }
        i = horizontal_path_row < (Y_WIDTH / 2) ? 1 : -1;

        m->map[horizontal_path_row + (1 * i)][pokimart_val] = 'M';
        m->map[horizontal_path_row + (1 * i)][pokimart_val + 1] = 'M';
        m->map[horizontal_path_row + (2 * i)][pokimart_val] = 'M';
        m->map[horizontal_path_row + (2 * i)][pokimart_val + 1] = 'M';
    }
    // pokicenter_val = horizontal_path_row;
    // pokimart_val = vertical_path_col;
    // while (pokicenter_val - horizontal_path_row < 1 && pokicenter_val - horizontal_path_row > -3) {
    //     pokicenter_val = 4 + rand() % (Y_WIDTH - 4 - 6 + 1);
    // }
    // while (abs(pokimart_val - vertical_path_col) < 2) {
    //     pokimart_val = 4 + rand() % (X_WIDTH - 4 - 6 + 1);
    // }

    // i = horizontal_path_row < (Y_WIDTH / 2) ? 1 : -1;
    // j = vertical_path_col < (X_WIDTH / 2) ? 1 : -1;

    // m->map[horizontal_path_row + (1 * i)][pokimart_val] = 'M';
    // m->map[horizontal_path_row + (1 * i)][pokimart_val + 1] = 'M';
    // m->map[horizontal_path_row + (2 * i)][pokimart_val] = 'M';
    // m->map[horizontal_path_row + (2 * i)][pokimart_val + 1] = 'M';
    

    // m->map[pokicenter_val][vertical_path_col + (1 * j)] = 'C';
    // m->map[pokicenter_val][vertical_path_col + (2 * j)] = 'C';
    // m->map[pokicenter_val + 1][vertical_path_col + (1 * j)] = 'C';
    // m->map[pokicenter_val + 1][vertical_path_col + (2 * j)] = 'C';

    return;
}

Map* generate_map(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    char starter_char_vals[7] = {':','.','^',':','.','%',':'};
    __uint8_t start_char_arr_length = 7;

    Map* m = (Map*)malloc(sizeof(Map));
    if (!m) {
        printf("Error allocating memory for map\n");
        return NULL;
    }
    m->map = generate_regions(starter_char_vals, start_char_arr_length);
    if (!m->map) {
        printf("Error generating regions\n");
        return NULL;
    }
    generate_path(m, top_path, bottom_path, left_path, right_path, distance);

    return m;
}