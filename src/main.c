#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#include "map/world.h"
#include "map/movement.h"
#include "config.h"
#include "dijkstra.h"

int num_trainers = 10;

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int x = 0, y = 0;
    int i;
    int input;

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
    //nodelay(stdscr, TRUE); //Set getch() to be non-blocking
    curs_set(0); //Set the cursor to be invisible
    keypad(stdscr, TRUE);

    TrainerDistance* movement_trainer = NULL;
    int has_moved = 1;
    int quit_game = 0;

    do {
        while (has_moved == 1) {
            movement_trainer = next_movement(world->maps[INDEX(y)][INDEX(x)], world->trainer_map, world->pc->x, world->pc->y);
            if (movement_trainer->trainer->type == PC) has_moved = 0;
        }

        print_map(world, INDEX(x), INDEX(y), world->pc->x, world->pc->y);
        mvprintw(Y_WIDTH + 1, 0, "You are at (%d,%d)", x, y);
        move(0,0);
        clrtoeol();
        refresh();

        while (quit_game != 1 && has_moved == 0) {
            flushinp();
            usleep(100);
            input = getch();
            switch (input) {
                case '7':
                case 'y':
                    //move up and to the left one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, NORTHWEST) == 1) has_moved = 1;
                    break;
                case '8':
                case 'k':
                    //move up one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, NORTH) == 1) has_moved = 1;
                    break;
                case '9':
                case 'u':
                    //move up and to the right one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, NORTHEAST) == 1) has_moved = 1;
                    break;
                case '6':
                case 'l':
                    //move right one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, EAST) == 1) has_moved = 1;
                    break;
                case '3':
                case 'n':
                    //move down and to the right one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, SOUTHEAST) == 1) has_moved = 1;
                    break;
                case '2':
                case 'j':
                    //move down one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, SOUTH) == 1) has_moved = 1;
                    break;
                case '1':
                case 'b':
                    //move down and to the left one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, SOUTHWEST) == 1) has_moved = 1;
                    break;
                case '4':
                case 'h':
                    //move left one cell
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, WEST) == 1) has_moved = 1;
                    break;
                case '>':
                    if (enter_building(world->maps[INDEX(y)][INDEX(x)], movement_trainer->trainer->x, movement_trainer->trainer->y) == 1) has_moved = 0;
                    else mvprintw(0, 0, "No building here");
                    break;
                case '<':
                    //Exit pokemart or pokecenter
                    mvprintw(0, 0, "Not currently in a building");
                    break;
                case '5':
                case ' ':
                    //rest for a turn
                    if (move_pc(movement_trainer, world->maps[INDEX(y)][INDEX(x)], world->trainer_map, NUM_DIRECTIONS) == 1) has_moved = 1;
                    break;
                case 't':
                    display_trainers(world, INDEX(x), INDEX(y), movement_trainer->trainer->x, movement_trainer->trainer->y);
                    //display list of trainers on map
                    break;
                case 27: //escape
                    //exit trainer list
                    break;
                case 'q':
                    quit_game = 1;
                    break;
                default:
                    mvprintw(0, 0, "Invalid input");
                    break;
            }
            if (has_moved == 0) {
                generate_distance_maps(world, x, y, movement_trainer->trainer->x, movement_trainer->trainer->y);
                print_map(world, INDEX(x), INDEX(y), world->pc->x, world->pc->y);
                mvprintw(Y_WIDTH + 1, 0, "You are at (%d,%d)", x, y);
            } else {
                movement_trainer = NULL;
            }
            move(0,0);
            clrtoeol();
            refresh();
        }
    } while (quit_game != 1);

    //Memory cleanup
    free_world(world);
    free_movement_queue();

    endwin();
    return 0;
}