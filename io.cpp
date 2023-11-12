#include <unistd.h>
#include <ncurses.h>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "io.h"
#include "character.h"
#include "poke327.h"

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_bailey:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, BAILEY_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] ==
             DIJKSTRA_PATH_MAX                                            ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

std::string num_str(int num) {
  return num == INT_MAX ? "" : std::to_string(num);
}

std::string pokemon_to_string (class pokemon *p) {
  std::string str;
  str += p->name;
  str += " Lvl:" + num_str(p->level);
  str += " HP:" + num_str(p->hp);
  str += " Atk:" + num_str(p->attack);
  str += " Def:" + num_str(p->defense);
  str += " SpAtk:" + num_str(p->special_attack);
  str += " SpDef:" + num_str(p->special_defense);
  str += " Spd:" + num_str(p->speed);
  str += " Moves:";
  for (class move* m : p->moves) {
    str += " " + m->name;
  }
  return str;
}

static void io_list_trainers_display(npc **c, uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = dynamic_cast<npc *> (world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "Welcome to the Pokemart.  Could I interest you in some Pokeballs?");
  refresh();
  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  How can Nurse Joy assist you?");
  refresh();
  getch();
}

void io_battle(character *aggressor, character *defender)
{
  npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);

  io_display();
  io_queue_message("A trainer challenges you to a battle! Pokemon:");
  for (class pokemon *p : n->pokemon_list) {
    io_queue_message(pokemon_to_string(p).c_str());
  }
  io_print_message_queue(0, 0);
  refresh();
  getch();

  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival) {
    n->mtype = move_wander;
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *> (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if ((dynamic_cast<npc *>
                (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]))) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_grass) {
    if (rand() % 100 < 10) {
      int distance = abs(world.cur_idx[dim_x] - 200) + abs(world.cur_idx[dim_y] - 200);
      int level;
      if (distance <= 200) {
        level = 1 + rand() % ((distance / 2) + 1);
      } else {
        level = ((distance - 200) / 2) + rand() % (101 - ((distance - 200) / 2));
      }
      class pokemon *p = get_random_pokemon(level);
      std::string pokemon_str = "A wild " + p->name + " appeared! (" + pokemon_to_string(p) + ")";
      mvprintw(0, 0, pokemon_str.c_str());
      refresh();
      getch();
      world.pc.pokemon_list.push_back(p);
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      DIJKSTRA_PATH_MAX) {
    return 1;
  }

  if (world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_gate &&
      dest[dim_y] != world.pc.pos[dim_y]                       &&
      dest[dim_x] != world.pc.pos[dim_x]) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;    
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

void find_file(const std::string& name, std::ifstream *file) {
  file->open("/share/cs327/pokedex/pokedex/data/csv/" + name);
  if (file->good()) return;
  file->open(std::string(getenv("HOME")) + "/.poke327/" + name);
  if (file->good()) return;
  else {
    std::cerr << "Error opening file" << std::endl;
    exit(1);
  }
}

std::vector<pokemon_t*>* io_parse_pokemon() {
  using namespace std;

  string first_line;
  auto pokemon_list = new vector<pokemon_t*>();

  ifstream file;
  find_file("pokemon.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_t* p = new pokemon_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // id
    p->id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // identifier
    p->identifier = token;
    getline(ss, token, ','); // species_id
    p->species_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // height
    p->height = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // weight
    p->weight = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // base_experience
    p->experience = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // order
    p->order = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // is_default
    p->is_default = token.empty() ? INT_MAX : stoi(token);

    pokemon_list->push_back(p);
  }

  return pokemon_list;
}

std::vector<move_t*>* io_parse_moves() {
  using namespace std;

  string first_line;
  auto move_list = new vector<move_t*>();

  ifstream file;
  find_file("moves.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    move_t* m = new move_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // id
    m->id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // identifier
    m->identifier = token;
    getline(ss, token, ','); // generation_id
    m->generation_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // type_id
    m->type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // power
    m->power = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // pp
    m->pp = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // accuracy
    m->accuracy = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // priority
    m->priority = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // target_id
    m->target_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // damage_class_id
    m->damage_class_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // effect_id
    m->effect_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // effect_chance
    m->effect_chance = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // contest_type_id
    m->contest_type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // contest_effect_id
    m->contest_effect_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // super_contest_effect_id
    m->super_contest_effect_id = token.empty() ? INT_MAX : stoi(token);

    move_list->push_back(m);
  }

  return move_list;
}

std::vector<pokemon_move_t*>* io_parse_pokemon_moves() {
  using namespace std;

  string first_line;
  auto pokemon_move_list = new vector<pokemon_move_t*>();

  ifstream file;
  find_file("pokemon_moves.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_move_t* pm = new pokemon_move_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // pokemon_id
    pm->pokemon_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // version_group_id
    pm->version_group_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // move_id
    pm->move_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // pokemon_move_method_id
    pm->pokemon_move_method_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // level
    pm->level = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // order
    pm->order = token.empty() ? INT_MAX : stoi(token);

    pokemon_move_list->push_back(pm);
  }

  return pokemon_move_list;
}

std::vector<pokemon_species_t*>* io_parse_pokemon_species() {
  using namespace std;

  string first_line;
  auto pokemon_species_list = new vector<pokemon_species_t*>();

  ifstream file;
  find_file("pokemon_species.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_species_t *ps = new pokemon_species_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // id
    ps->id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // identifier
    ps->identifier = token;
    getline(ss, token, ','); // generation_id
    ps->generation_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // evolves_from_species_id
    ps->evolves_from_species_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // evolution_chain_id
    ps->evolution_chain_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // color_id
    ps->color_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // shape_id
    ps->shape_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // habitat_id
    ps->habitat_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // gender_rate
    ps->gender_rate = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // capture_rate
    ps->capture_rate = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // base_happiness
    ps->base_happiness = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // is_baby
    ps->is_baby = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // hatch_counter
    ps->hatch_counter = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // has_gender_differences
    ps->has_gender_differences = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // growth_rate_id
    ps->growth_rate_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // forms_switchable
    ps->forms_switchable = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // is_legendary
    ps->is_legendary = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // is_mythical
    ps->is_mythical = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // order
    ps->order = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // conquest_order
    ps->conquest_order = token.empty() ? INT_MAX : stoi(token);

    pokemon_species_list->push_back(ps);
  }

  return pokemon_species_list;
}

std::vector<experience_t*>* io_parse_experience() {
  using namespace std;
  
  string first_line;
  auto experience_list = new vector<experience_t*>();

  ifstream file;
  find_file("experience.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    experience_t* e = new experience_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // growth_rate_id
    e->growth_rate_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // level
    e->level = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // experience
    e->experience = token.empty() ? INT_MAX : stoi(token);

    experience_list->push_back(e);
  }

  return experience_list;
}

std::vector<type_name_t*>* io_parse_type_names() {
  using namespace std;

  string first_line;
  auto type_name_list = new vector<type_name_t*>();

  ifstream file;
  find_file("type_names.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    type_name_t* tn = new type_name_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // type_id
    tn->type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // local_language_id
    tn->local_language_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // name
    tn->name = token;

    type_name_list->push_back(tn);
  }

  return type_name_list;
}

std::vector<pokemon_stat_t*>* io_parse_pokemon_stats() {
  using namespace std;

  string first_line;
  auto pokemon_stat_list = new vector<pokemon_stat_t*>();

  ifstream file;
  find_file("pokemon_stats.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_stat_t* ps = new pokemon_stat_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // pokemon_id
    ps->pokemon_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // stat_id
    ps->stat_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // base_stat
    ps->base_stat = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // effort
    ps->effort = token.empty() ? INT_MAX : stoi(token);

    pokemon_stat_list->push_back(ps);
  }

  return pokemon_stat_list;
}

std::vector<stat_t*>* io_parse_stats() {
  using namespace std;

  string first_line;
  auto stat_list = new vector<stat_t*>();

  ifstream file;
  find_file("stats.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    stat_t* s = new stat_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // id
    s->id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // damage_class_id
    s->damage_class_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // identifier
    s->identifier = token;
    getline(ss, token, ','); // is_battle_only
    s->is_battle_only = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // game_index
    s->game_index = token.empty() ? INT_MAX : stoi(token);

    stat_list->push_back(s);
  }

  return stat_list;
}

std::vector<pokemon_type_t*>* io_parse_pokemon_types() {
  using namespace std;

  string first_line;
  auto pokemon_type_list = new vector<pokemon_type_t*>();

  ifstream file;
  find_file("pokemon_types.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_type_t* pt = new pokemon_type_t;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // pokemon_id
    pt->pokemon_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // type_id
    pt->type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // slot
    pt->slot = token.empty() ? INT_MAX : stoi(token);

    pokemon_type_list->push_back(pt);
  }

  return pokemon_type_list;
}

void io_load_data() {
  world.data.pokemon = io_parse_pokemon();
  world.data.moves = io_parse_moves();
  world.data.pokemon_moves = io_parse_pokemon_moves();
  world.data.pokemon_species = io_parse_pokemon_species();
  world.data.experience = io_parse_experience();
  world.data.type_names = io_parse_type_names();
  world.data.pokemon_stats = io_parse_pokemon_stats();
  world.data.stats = io_parse_stats();
  world.data.pokemon_types = io_parse_pokemon_types();
}

void io_get_first_pokemon(class pc *player) {
  clear();

  mvprintw(0, 0, "Welcome to the world of Pokemon! Please choose a pokemon from the list below by entering 1, 2, or 3");
  class pokemon *p1 = get_random_pokemon(1);
  class pokemon *p2 = get_random_pokemon(1);
  class pokemon *p3 = get_random_pokemon(1);

  std::string p1_str = pokemon_to_string(p1);
  std::string p2_str = pokemon_to_string(p2);
  std::string p3_str = pokemon_to_string(p3);  

  mvprintw(1, 0, "1. %s", p1_str.c_str());
  mvprintw(2, 0, "2. %s", p2_str.c_str());
  mvprintw(3, 0, "3. %s", p3_str.c_str());
  mvprintw(4, 0, "Enter your choice: ");

  char input = getch();
  while (input != '1' && input != '2' && input != '3') {
    input = getch();
  }

  if (input == '1') {
    player->pokemon_list.push_back(p1);
  } else if (input == '2') {
    player->pokemon_list.push_back(p2);
  } else {
    player->pokemon_list.push_back(p3);
  }
}