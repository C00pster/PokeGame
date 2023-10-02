#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

#include "map/game_map.h"
#include "map/tile.h"
#include "map/world.h"
#include "character.h"
#include "config.h"

void print_map(GameMap* game_map) {
    int i, j;

    clear();

    for (j = 0; j < Y_WIDTH; j++) {
        for (i = 0; i < X_WIDTH; i++) {
            printw("%c", get_tile_char(game_map->tiles[j][i]));
        }
        printw("\n");
    }
    refresh();
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x = 0, y = 0;
    int i;
    char input;
    int num_trianers = 10;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--num-trainers") == 0) {
            if (i+1 >= argc) {
                printf("Error: --num-trainers requires an argument\n");
                return 1;
            } else {
                num_trianers = atoi(argv[i+1]);
                if (num_trianers < 0) {
                    printf("Error: --num-trainers requires a positive integer argument\n");
                    return 1;
                }
            }
        }
    }

    World* world = create_world();
    if (!world) {
        printw("Error allocating memory for world\n");
        refresh();
        endwin();
        return 1;
    }
    
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    print_map(world->maps[INDEX(y)][INDEX(x)]); //Prints the map

    do {
        printw("You are at (%d,%d)", x, y);
        input = getch();

        switch (input) {
            case 'n':
                if (INDEX(y+1) >= SIZE) {
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                    printw("You can't go that way\n");
                    refresh();
                    break;
                } else {
                    y++;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                }
                break;
            case 's':
                if (INDEX(y-1) < 0) {
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                    printw("You can't go that way\n");
                    refresh();
                    break;
                } else {
                    y--;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                }
                break;
            case 'e':
                if (INDEX(x+1) >= SIZE) {
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                    printw("You can't go that way\n");
                    refresh();
                    break;
                } else {
                    x++;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                }
                break;
            case 'w':
                if (INDEX(x-1) < 0) {
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                    printw("You can't go that way\n");
                    refresh();
                    break;
                } else {
                    x--;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
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
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                    print_map(world->maps[INDEX(y)][INDEX(x)]);
                }
                break;
            case 'q':
                break;
            default:
                printf("Invalid command\n");
                break;
            clear();
        }
    } while (input != 'q');

    //Memory cleanup
    free_world(world);

    endwin();
    return 0;
}