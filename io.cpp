#include <unistd.h>
#include <ncurses.h>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <string>
#include <queue>
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
  mvprintw(0, 0, "Aww, how'd you get so strong?  You and your pokemon must share a special bond!");
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

std::string num_str(int num) {
  return num == INT_MAX ? "" : std::to_string(num);
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

int io_parse_pokemon() {
  using namespace std;
  typedef struct {
    int id;
    string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
  } pokemon;

  string first_line;
  queue<pokemon*> pokemon_list;

  ifstream file;
  find_file("pokemon.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon* p = new pokemon;
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
    p->base_experience = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // order
    p->order = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // is_default
    p->is_default = token.empty() ? INT_MAX : stoi(token);

    pokemon_list.push(p);
  }

  cout << first_line << endl;
  while (pokemon_list.size() > 0) {
    pokemon *p = pokemon_list.front();
    pokemon_list.pop();
    cout << num_str(p->id) << "," << p->identifier << "," << num_str(p->species_id) << "," << 
      num_str(p->height) << "," << num_str(p->weight) << "," << num_str(p->base_experience) << "," << 
      num_str(p->order) << "," << num_str(p->is_default) << endl;
    delete p;
  }
  return 1;
}

int io_parse_moves() {
  using namespace std;
  typedef struct {
    int id;
    string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
  } move;

  string first_line;
  queue<move*> move_list;

  ifstream file;
  find_file("moves.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    move* m = new move;
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

    move_list.push(m);
  }

  cout << first_line << endl;
  while (move_list.size() > 0) {
    move *m = move_list.front();
    move_list.pop();
    cout << num_str(m->id) << "," << m->identifier << "," << num_str(m->generation_id) << "," << 
      num_str(m->type_id) << "," << num_str(m->power) << "," << num_str(m->pp) << "," << 
      num_str(m->accuracy) << "," << num_str(m->priority) << "," << num_str(m->target_id) << "," << 
      num_str(m->damage_class_id) << "," << num_str(m->effect_id) << "," << num_str(m->effect_chance) << 
      "," << num_str(m->contest_type_id) << "," << num_str(m->contest_effect_id) << "," << 
      num_str(m->super_contest_effect_id) << endl;
    delete m;
  }
  return 1;
}

int io_parse_pokemon_moves() {
  using namespace std;
  typedef struct {
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
  } pokemon_move;

  string first_line;
  queue<pokemon_move*> pokemon_move_list;

  ifstream file;
  find_file("pokemon_moves.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_move* pm = new pokemon_move;
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

    pokemon_move_list.push(pm);
  }

  cout << first_line << endl;
  while (pokemon_move_list.size() > 0) {
    pokemon_move *pm = pokemon_move_list.front();
    pokemon_move_list.pop();
    cout << num_str(pm->pokemon_id) << "," << num_str(pm->version_group_id) << "," << num_str(pm->move_id) << 
      "," << num_str(pm->pokemon_move_method_id) << "," << num_str(pm->level) << "," << num_str(pm->order) << 
      endl;
    delete pm;
  }
  return 1;
}

int io_parse_pokemon_species() {
  using namespace std;
  typedef struct {
    int id;
    string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
  } pokemon_species;

  string first_line;
  queue<pokemon_species*> pokemon_species_list;

  ifstream file;
  find_file("pokemon_species.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_species *ps = new pokemon_species;
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

    pokemon_species_list.push(ps);
  }

  cout << first_line << endl;
  while (pokemon_species_list.size() > 0) {
    pokemon_species *ps = pokemon_species_list.front();
    pokemon_species_list.pop();
    cout << num_str(ps->id) << "," << ps->identifier << "," << num_str(ps->generation_id) << "," << 
      num_str(ps->evolves_from_species_id) << "," << num_str(ps->evolution_chain_id) << "," << 
      num_str(ps->color_id) << "," << num_str(ps->shape_id) << "," << num_str(ps->habitat_id) << "," << 
      num_str(ps->gender_rate) << "," << num_str(ps->base_happiness) << "," << num_str(ps->is_baby) << 
      "," << num_str(ps->hatch_counter) << "," << num_str(ps->has_gender_differences) << "," << 
      num_str(ps->growth_rate_id) << "," << num_str(ps->forms_switchable) << "," << 
      num_str(ps->is_legendary) << "," << num_str(ps->is_mythical) << "," << num_str(ps->order) << "," <<
      num_str(ps->conquest_order) << endl;
    delete ps;
  }
  return 1;
}

int io_parse_experience() {
  using namespace std;
  typedef struct {
    int growth_rate_id;
    int level;
    int experience;
  } experience;
  
  string first_line;
  queue<experience*> experience_list;

  ifstream file;
  find_file("experience.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    experience* e = new experience;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // growth_rate_id
    e->growth_rate_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // level
    e->level = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // experience
    e->experience = token.empty() ? INT_MAX : stoi(token);

    experience_list.push(e);
  }

  cout << first_line << endl;
  while (experience_list.size() > 0) {
    experience *e = experience_list.front();
    experience_list.pop();
    cout << num_str(e->growth_rate_id) << "," << num_str(e->level) << "," << num_str(e->experience) << endl;
    delete e;
  }
  return 1;
}

int io_parse_type_names() {
  using namespace std;
  typedef struct {
    int type_id;
    int local_language_id;
    string name;
  } type_name;

  string first_line;
  queue<type_name*> type_name_list;

  ifstream file;
  find_file("type_names.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    type_name* tn = new type_name;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // type_id
    tn->type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // local_language_id
    tn->local_language_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // name
    tn->name = token;

    type_name_list.push(tn);
  }

  cout << first_line << endl;
  while (type_name_list.size() > 0) {
    type_name *tn = type_name_list.front();
    type_name_list.pop();
    cout << num_str(tn->type_id) << "," << num_str(tn->local_language_id) << "," << tn->name << endl;
    delete tn;
  }
  return 1;
}

int io_parse_pokemon_stats() {
  using namespace std;
  typedef struct {
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
  } pokemon_stat;

  string first_line;
  queue<pokemon_stat*> pokemon_stat_list;

  ifstream file;
  find_file("pokemon_stats.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_stat* ps = new pokemon_stat;
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

    pokemon_stat_list.push(ps);
  }

  cout << first_line << endl;
  while (pokemon_stat_list.size() > 0) {
    pokemon_stat *ps = pokemon_stat_list.front();
    pokemon_stat_list.pop();
    cout << num_str(ps->pokemon_id) << "," << num_str(ps->stat_id) << "," << num_str(ps->base_stat) << "," << 
      num_str(ps->effort) << endl;
  }
  return 1;
}

int io_parse_stats() {
  using namespace std;
  typedef struct {
    int id;
    int damage_class_id;
    string identifier;
    int is_battle_only;
    int game_index;
  } stat;

  string first_line;
  queue<stat*> stat_list;

  ifstream file;
  find_file("stats.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    stat* s = new stat;
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

    stat_list.push(s);
  }

  cout << first_line << endl;
  while (stat_list.size() > 0) {
    stat *s = stat_list.front();
    stat_list.pop();
    cout << num_str(s->id) << "," << num_str(s->damage_class_id) << "," << s->identifier << "," << 
      num_str(s->is_battle_only) << "," << num_str(s->game_index) << endl;
  }
  return 1;
}

int io_parse_pokemon_types() {
  using namespace std;
  typedef struct {
    int pokemon_id;
    int type_id;
    int slot;
  } pokemon_type;

  string first_line;
  queue<pokemon_type*> pokemon_type_list;

  ifstream file;
  find_file("pokemon_types.csv", &file);
  string line;

  if(!file.is_open()) {
    cerr << "Error opening file" << endl;
    exit(1);
  }

  getline(file, first_line);

  while (getline(file, line)) {
    pokemon_type* pt = new pokemon_type;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); // pokemon_id
    pt->pokemon_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // type_id
    pt->type_id = token.empty() ? INT_MAX : stoi(token);
    getline(ss, token, ','); // slot
    pt->slot = token.empty() ? INT_MAX : stoi(token);

    pokemon_type_list.push(pt);
  }

  cout << first_line << endl;
  while (pokemon_type_list.size() > 0) {
    pokemon_type *pt = pokemon_type_list.front();
    pokemon_type_list.pop();
    cout << num_str(pt->pokemon_id) << "," << num_str(pt->type_id) << "," << num_str(pt->slot) << endl;
  }
  return 1;
}

int io_parse_csv(csv_files file) {
  switch (file) {
    case pokemon:
      return io_parse_pokemon();
    case moves:
      return io_parse_moves();
    case pokemon_moves:
      return io_parse_pokemon_moves();
    case pokemon_species:
      return io_parse_pokemon_species();
    case experience:
      return io_parse_experience();
    case type_names:
      return io_parse_type_names();
    case pokemon_stats:
      return io_parse_pokemon_stats();
    case stats:
      return io_parse_stats();
    case pokemon_types:
      return io_parse_pokemon_types();
  }
  return 0;
}