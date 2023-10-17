#include <stdlib.h>

#include "map/path.h"
#include "config.h"

void generate_path(GameMap* m, __int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path, int distance) {
    __int8_t vertical_path_col, horizontal_path_row, bottom_greater, left_greater;
    __int8_t pokimart_val, pokicenter_val, i, j;

    top_path = top_path == 0 ? 5 + rand() % (X_WIDTH - 10) : top_path;
    bottom_path = bottom_path == 0 ? 5 + rand() % (X_WIDTH - 10) : bottom_path;
    left_path = left_path == 0 ? 5 + rand() % (Y_WIDTH - 10) : left_path;
    right_path = right_path == 0 ? 5 + rand() % (Y_WIDTH - 10) : right_path;
    
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
    if (bottom_path != -1) m->tiles[0][bottom_path] = create_tile(GATE, bottom_path, 0);
    if (top_path != -1) m->tiles[Y_WIDTH - 1][top_path] = create_tile(GATE, top_path, Y_WIDTH - 1);
    if (left_path != -1) m->tiles[left_path][0] = create_tile(GATE, 0, left_path);
    if (right_path != -1) m->tiles[right_path][X_WIDTH - 1] = create_tile(GATE, X_WIDTH - 1, right_path);
    for (i = 1; i < 3; i++) {
        if (bottom_path != -1) m->tiles[i][bottom_path] = create_tile(PATH, bottom_path, i);
        if (top_path != -1) m->tiles[Y_WIDTH - i - 1][top_path] = create_tile(PATH, top_path, Y_WIDTH - i - 1);
        if (left_path != -1) m->tiles[left_path][i] = create_tile(PATH, i, left_path);
        if (right_path != -1) m->tiles[right_path][X_WIDTH - i - 1] = create_tile(PATH, X_WIDTH - i - 1, right_path);
    }

    //Creates a path from the path entrance column or row to the vertical or horizontal cross
    if (left_greater == 1) {
        while (left_path != horizontal_path_row) {
            m->tiles[left_path][3] = create_tile(PATH, 3, left_path);
            left_path--;
        }
        while (right_path != horizontal_path_row) {
            m->tiles[right_path][X_WIDTH-4] = create_tile(PATH, X_WIDTH-4, right_path);
            right_path++;
        }
    } else if (left_greater == -1) {
        while (left_path != horizontal_path_row) {
            m->tiles[left_path][3] = create_tile(PATH, 3, left_path);
            left_path++;
        }
        while (right_path != horizontal_path_row) {
            m->tiles[right_path][X_WIDTH-4] = create_tile(PATH, X_WIDTH-4, right_path);
            right_path--;
        }
    }
    if (bottom_greater == 1) {
        while (bottom_path != vertical_path_col) {
            m->tiles[3][bottom_path] = create_tile(PATH, bottom_path, 3);
            bottom_path--;
        }
        while (top_path != vertical_path_col) {
            m->tiles[Y_WIDTH-4][top_path] = create_tile(PATH, top_path, Y_WIDTH-4);
            top_path++;
        }
    } else if (bottom_greater == -1) {
        while (bottom_path != vertical_path_col) {
            m->tiles[3][bottom_path] = create_tile(PATH, bottom_path, 3);
            bottom_path++;
        }
        while (top_path != vertical_path_col) {
            m->tiles[Y_WIDTH-4][top_path] = create_tile(PATH, top_path, Y_WIDTH-4);
            top_path--;
        }
    }

    //Creates perpendicular paths across the map to connect all entrances
    i = left_path == -1 ? vertical_path_col : 3; //If there is not a left path, start at the vertical path col
    j = right_path == -1 ? vertical_path_col : X_WIDTH - 3; //If there is not a right path, end at the vertical path col
    for (; i < j; i++) {
        m->tiles[horizontal_path_row][i] = create_tile(PATH, i, horizontal_path_row);
    }
    i = bottom_path == -1 ? horizontal_path_row : 3; //If there is not a bottom path, start at the horizontal path row
    j = top_path == -1 ? horizontal_path_row : Y_WIDTH - 3; //If there is not a top path, end at the horizontal path row
    for (; i < j; i++) {
        m->tiles[i][vertical_path_col] = create_tile(PATH, vertical_path_col, i);
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

        m->tiles[pokicenter_val][vertical_path_col + (1 * i)] = create_tile(PCNTR, vertical_path_col + (1 * i), pokicenter_val);
        m->tiles[pokicenter_val][vertical_path_col + (2 * i)] = create_tile(PCNTR, vertical_path_col + (2 * i), pokicenter_val);
        m->tiles[pokicenter_val + 1][vertical_path_col + (1 * i)] = create_tile(PCNTR, vertical_path_col + (1 * i), pokicenter_val + 1);
        m->tiles[pokicenter_val + 1][vertical_path_col + (2 * i)] = create_tile(PCNTR, vertical_path_col + (2 * i), pokicenter_val + 1);
    }
    if (rand() % 100 < generation_chance) {
        pokimart_val = vertical_path_col;
        while (abs(pokimart_val - vertical_path_col) < 2) {
            pokimart_val = 4 + rand() % (X_WIDTH - 4 - 6 + 1);
        }
        i = horizontal_path_row < (Y_WIDTH / 2) ? 1 : -1;

        m->tiles[horizontal_path_row + (1 * i)][pokimart_val] = create_tile(PMART, pokimart_val, horizontal_path_row + (1 * i));
        m->tiles[horizontal_path_row + (1 * i)][pokimart_val + 1] = create_tile(PMART, pokimart_val + 1, horizontal_path_row + (1 * i));
        m->tiles[horizontal_path_row + (2 * i)][pokimart_val] = create_tile(PMART, pokimart_val, horizontal_path_row + (2 * i));
        m->tiles[horizontal_path_row + (2 * i)][pokimart_val + 1] = create_tile(PMART, pokimart_val + 1, horizontal_path_row + (2 * i));
    }

    return;
}