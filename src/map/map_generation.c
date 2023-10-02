#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "data_structures/queue.h"
#include "map/map_generation.h"
#include "map/tile.h"
#include "dijkstra.h"
#include "config.h"

#define ARR_MAX 800

Tile*** allocate_map() {
    __int8_t i;
    Tile*** map = (Tile***)malloc(Y_WIDTH * sizeof(Tile**));
    if (!map) {
        printf("Error allocating memory for map\n");
        return NULL;
    }
    for (i =0; i < Y_WIDTH; i++) {
        map[i] = malloc (X_WIDTH * sizeof(Tile*));
        if (!map[i]) {
            printf("Error allocating memory for map\n");
            return NULL;
        }
        memset(map[i], '\0', X_WIDTH * sizeof(Tile*));
    }

    return map;
}

Tile*** generate_regions(TileType* starter_tile_types, __uint8_t start_tile_type_length) {
    __int8_t i,j;
    Tile*** map = allocate_map();
    Queue* q = create_queue(ARR_MAX);

    //Generates random starter points
    for (i = 0; i < start_tile_type_length; i++) {
        Point* new_point = generate_point(1, X_WIDTH-2, 1, Y_WIDTH-2);
        map[new_point->y][new_point->x] = create_tile(starter_tile_types[i], new_point->x, new_point->y); //starter_char_vals[i];
        enqueue(q, new_point);
    }

    //Sets boulder border
    for (i = 0; i < X_WIDTH; i++) {
        map[0][i] = create_tile(BOULDER, i, 0);
        map[Y_WIDTH-1][i] = create_tile(BOULDER, i, Y_WIDTH-1);
    }
    for (i = 1; i < Y_WIDTH - 1; i++) {
        map[i][0] = create_tile(BOULDER, 0, i);
        map[i][X_WIDTH-1] = create_tile(BOULDER, X_WIDTH-1, i);
    }

    //Fills map in from starter points
    while (q->size > 0) {
        Point* p = dequeue(q);
        Tile* t = map[p->y][p->x];

        for (j = -1; j <= 1; j++) {
            for (i = -2; i <= 2; i++) {
                Point* new_point = create_point(p->x + i, p->y + j);
                if (new_point->y > 0 && new_point->y < Y_WIDTH-1 && new_point->x > 0 && new_point->x < X_WIDTH - 1) {
                    if (!map[new_point->y][new_point->x]) {
                        map[new_point->y][new_point->x] = create_tile(t->type, new_point->x, new_point->y);
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
    m->horizontal_path_row = horizontal_path_row;
    m->vertical_path_col = vertical_path_col;

    //Creates a 3 tile long path out from each entrance
    if (bottom_path != -1) m->map[0][bottom_path] = create_tile(GATE, bottom_path, 0);
    if (top_path != -1) m->map[Y_WIDTH - 1][top_path] = create_tile(GATE, top_path, Y_WIDTH - 1);
    if (left_path != -1) m->map[left_path][0] = create_tile(GATE, 0, left_path);
    if (right_path != -1) m->map[right_path][X_WIDTH - 1] = create_tile(GATE, X_WIDTH - 1, right_path);
    for (i = 1; i < 3; i++) {
        if (bottom_path != -1) m->map[i][bottom_path] = create_tile(PATH, bottom_path, i);
        if (top_path != -1) m->map[Y_WIDTH - i - 1][top_path] = create_tile(PATH, top_path, Y_WIDTH - i - 1);
        if (left_path != -1) m->map[left_path][i] = create_tile(PATH, i, left_path);
        if (right_path != -1) m->map[right_path][X_WIDTH - i - 1] = create_tile(PATH, X_WIDTH - i - 1, right_path);
    }

    //Creates a path from the path entrance column or row to the vertical or horizontal cross
    if (left_greater == 1) {
        while (left_path != horizontal_path_row) {
            m->map[left_path][3] = create_tile(PATH, 3, left_path);
            left_path--;
        }
        while (right_path != horizontal_path_row) {
            m->map[right_path][X_WIDTH-4] = create_tile(PATH, X_WIDTH-4, right_path);
            right_path++;
        }
    } else if (left_greater == -1) {
        while (left_path != horizontal_path_row) {
            m->map[left_path][3] = create_tile(PATH, 3, left_path);
            left_path++;
        }
        while (right_path != horizontal_path_row) {
            m->map[right_path][X_WIDTH-4] = create_tile(PATH, X_WIDTH-4, right_path);
            right_path--;
        }
    }
    if (bottom_greater == 1) {
        while (bottom_path != vertical_path_col) {
            m->map[3][bottom_path] = create_tile(PATH, bottom_path, 3);
            bottom_path--;
        }
        while (top_path != vertical_path_col) {
            m->map[Y_WIDTH-4][top_path] = create_tile(PATH, top_path, Y_WIDTH-4);
            top_path++;
        }
    } else if (bottom_greater == -1) {
        while (bottom_path != vertical_path_col) {
            m->map[3][bottom_path] = create_tile(PATH, bottom_path, 3);
            bottom_path++;
        }
        while (top_path != vertical_path_col) {
            m->map[Y_WIDTH-4][top_path] = create_tile(PATH, top_path, Y_WIDTH-4);
            top_path--;
        }
    }

    //Creates perpendicular paths across the map to connect all entrances
    i = left_path == -1 ? vertical_path_col : 3; //If there is not a left path, start at the vertical path col
    j = right_path == -1 ? vertical_path_col : X_WIDTH - 3; //If there is not a right path, end at the vertical path col
    for (; i < j; i++) {
        m->map[horizontal_path_row][i] = create_tile(PATH, i, horizontal_path_row);
    }
    i = bottom_path == -1 ? horizontal_path_row : 3; //If there is not a bottom path, start at the horizontal path row
    j = top_path == -1 ? horizontal_path_row : Y_WIDTH - 3; //If there is not a top path, end at the horizontal path row
    for (; i < j; i++) {
        m->map[i][vertical_path_col] = create_tile(PATH, vertical_path_col, i);
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

        m->map[pokicenter_val][vertical_path_col + (1 * i)] = create_tile(PCNTR, vertical_path_col + (1 * i), pokicenter_val);
        m->map[pokicenter_val][vertical_path_col + (2 * i)] = create_tile(PCNTR, vertical_path_col + (2 * i), pokicenter_val);
        m->map[pokicenter_val + 1][vertical_path_col + (1 * i)] = create_tile(PCNTR, vertical_path_col + (1 * i), pokicenter_val + 1);
        m->map[pokicenter_val + 1][vertical_path_col + (2 * i)] = create_tile(PCNTR, vertical_path_col + (2 * i), pokicenter_val + 1);
    }
    if (rand() % 100 < generation_chance) {
        pokimart_val = vertical_path_col;
        while (abs(pokimart_val - vertical_path_col) < 2) {
            pokimart_val = 4 + rand() % (X_WIDTH - 4 - 6 + 1);
        }
        i = horizontal_path_row < (Y_WIDTH / 2) ? 1 : -1;

        m->map[horizontal_path_row + (1 * i)][pokimart_val] = create_tile(PMART, pokimart_val, horizontal_path_row + (1 * i));
        m->map[horizontal_path_row + (1 * i)][pokimart_val + 1] = create_tile(PMART, pokimart_val + 1, horizontal_path_row + (1 * i));
        m->map[horizontal_path_row + (2 * i)][pokimart_val] = create_tile(PMART, pokimart_val, horizontal_path_row + (2 * i));
        m->map[horizontal_path_row + (2 * i)][pokimart_val + 1] = create_tile(PMART, pokimart_val + 1, horizontal_path_row + (2 * i));
    }

    return;
}

Map* generate_terrain(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    TileType starter_tile_types[7] = {TGRASS, SGRASS, FOREST, TGRASS, SGRASS, MOUNTAIN, TGRASS};
    __uint8_t start_tile_type_length = 7;

    Map* m = (Map*)malloc(sizeof(Map));
    if (!m) {
        printf("Error allocating memory for map\n");
        return NULL;
    }
    m->map = generate_regions(starter_tile_types, start_tile_type_length);
    if (!m->map) {
        printf("Error generating regions\n");
        return NULL;
    }
    generate_path(m, top_path, bottom_path, left_path, right_path, distance);

    return m;
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

Point* add_pc(Map*** world, int x, int y) {
    Map* m = world[INDEX(y)][INDEX(x)];
    int x_coor, y_coor;

    if (rand() % 2 == 1) {
        y_coor = m->horizontal_path_row;
        x_coor = rand() % (X_WIDTH - 6) + 3;
    } else {
        y_coor = rand() % (Y_WIDTH - 6) + 3;
        x_coor = m->vertical_path_col;
    }

    m->map[y_coor][x_coor]->trainer = PC;

    Point* p = create_point(x_coor, y_coor);

    return p;
}

/*
Creates a map if it doesn't exist. Adds trainers to created map
*/
void generate_map(Map*** world, int x, int y) {
    if (x != 0 || y != 0) {
        Path_Tracker* p = get_paths(world, INDEX(x), INDEX(y));
        int distance = sqrt(pow(x, 2) + pow(y, 2));
        world[INDEX(y)][INDEX(x)] = generate_terrain(p->top, p->bottom, p->left, p->right, distance);
        free(p);
    } else {
        world[INDEX(y)][INDEX(x)] = generate_terrain(X_WIDTH/2, X_WIDTH/2, Y_WIDTH/2, Y_WIDTH/2, 0);
    }
    Point* p = add_pc(world, x, y);

    world[INDEX(y)][INDEX(x)]->rival_distance_map = generate_distance_map(world[INDEX(y)][INDEX(x)], p->x, p->y, RIVAL);
    world[INDEX(y)][INDEX(x)]->hiker_distance_map = generate_distance_map(world[INDEX(y)][INDEX(x)], p->x, p->y, HIKER);
}

Map*** create_world() {
    int i;
    Map ***world = (Map***)malloc(sizeof(Map***) * SIZE); //Creates an array of pointers to maps
    if (!world) {
        printf("Error allocating memory for world\n");
        return NULL;
    }
    for(i = 0; i < SIZE; i++) {
        world[i] = (Map**)malloc(sizeof(Map*) * SIZE);
        if (!world[i]) {
            printf("Error allocating memory for world\n");
            return NULL;
        }
        memset(world[i], '\0', sizeof(Map*) * SIZE);
    }

    int x = 0, y = 0;
    generate_map(world, x, y);

    return world;
}