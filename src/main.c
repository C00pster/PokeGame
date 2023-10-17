#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#include "map/world.h"
#include "map/movement.h"
#include "config.h"

int num_trainers = 10;

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x = 0, y = 0;
    int i;
    char input;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--num-trainers") == 0) {
            if (i+1 >= argc) {
                printf("Error: --num-trainers requires an argument\n");
                return 1;
            } else {
                num_trainers = atoi(argv[i+1]);
                if (num_trainers < 0) {
                    printf("Error: --num-trainers requires a positive integer argument\n");
                    return 1;
                }
            }
        }
    }

    init_movement_queue();

    World* world = create_world();
    if (!world) {
        printw("Error allocating memory for world\n");
        refresh();
        endwin();
        return 1;
    }
    
    initscr();
    raw();
    noecho(); //Typed characters are not echoed
    nodelay(stdscr, TRUE); //Set getch() to be non-blocking
    curs_set(0); //Set the cursor to be invisible
    keypad(stdscr, TRUE);

    do {
        print_map(world, INDEX(x), INDEX(y));
        mvprintw(Y_WIDTH, 0, "You are at (%d,%d)", x, y);
        refresh();

        input = getch();
        if (input == ERR) { //No key was pressed
            next_movement(world->maps[INDEX(y)][INDEX(x)], world->trainer_map);
            usleep(25000);
            continue;
        }

        switch (input) {
            case 'w':
                if (INDEX(y+1) >= SIZE) {
                    mvprintw(Y_WIDTH + 2, 0, "You can't go that way");
                } else {
                    y++;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                }
                break;
            case 's':
                if (INDEX(y-1) < 0) {
                    mvprintw(Y_WIDTH + 1, 0, "You can't go that way");
                } else {
                    y--;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                }
                break;
            case 'd':
                if (INDEX(x+1) >= SIZE) {
                    mvprintw(Y_WIDTH + 1, 0, "You can't go that way");
                } else {
                    x++;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                }
                break;
            case 'a':
                if (INDEX(x-1) < 0) {
                    printw("You can't go that way\n");
                } else {
                    x--;
                    if (!world->maps[INDEX(y)][INDEX(x)]) generate_map(world, x, y);
                }
                break;
            // case ' ':
            //     movementTrainer = next_movement(world->maps[INDEX(y)][INDEX(x)], world->trainer_map);
            //     break;
            case 'q':
                break;
            default:
                mvprintw(Y_WIDTH + 1, 0, "Invalid input");
                break;
        }
    } while (input != 'q');

    //Memory cleanup
    free_world(world);
    free_movement_queue();

    endwin();
    return 0;
}