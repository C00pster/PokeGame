#ifndef POKE327_H
# define POKE327_H

# include <cstdlib>
# include <cassert>
# include <vector>
# include <algorithm>
# include <cmath>

# include "heap.h"
# include "character.h"
# include "pair.h"
# include "data.h"

#define malloc(size) ({                 \
  char *_tmp;                           \
  assert(_tmp = (char *) malloc(size)); \
  _tmp;                                 \
})

/* Returns true if random float in [0,1] is less than *
 * numerator/denominator.  Uses only integer math.    */
# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))

/* Returns random integer in [min, max]. */
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

# define UNUSED(f) ((void) f)

#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define WORLD_SIZE         401

#define MIN_TRAINERS     7
#define ADD_TRAINER_PROB 60

#define MOUNTAIN_SYMBOL       '%'
#define BOULDER_SYMBOL        '0'
#define TREE_SYMBOL           '4'
#define FOREST_SYMBOL         '^'
#define GATE_SYMBOL           '#'
#define PATH_SYMBOL           '#'
#define BAILEY_SYMBOL         '#'
#define POKEMART_SYMBOL       'M'
#define POKEMON_CENTER_SYMBOL 'C'
#define TALL_GRASS_SYMBOL     ':'
#define SHORT_GRASS_SYMBOL    '.'
#define WATER_SYMBOL          '~'
#define ERROR_SYMBOL          '&'

#define PC_SYMBOL       '@'
#define HIKER_SYMBOL    'h'
#define RIVAL_SYMBOL    'r'
#define EXPLORER_SYMBOL 'e'
#define SENTRY_SYMBOL   's'
#define PACER_SYMBOL    'p'
#define SWIMMER_SYMBOL  'm'
#define WANDERER_SYMBOL 'w'

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass,
  ter_clearing,
  ter_mountain,
  ter_forest,
  ter_water,
  ter_gate,
  ter_bailey,
  num_terrain_types,
  ter_debug
} terrain_type_t;

extern int32_t move_cost[num_character_types][num_terrain_types];

class map {
 public:
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  character *cmap[MAP_Y][MAP_X];
  heap_t turn;
  int32_t num_trainers;
  int8_t n, s, e, w;
};

class world {
 public:
  map *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  map *cur_map;
  /* Please distance maps in world, not map, since *
   * we only need one pair at any given time.      */
  int hiker_dist[MAP_Y][MAP_X];
  int rival_dist[MAP_Y][MAP_X];
  class pc pc;
  int quit;
  int add_trainer_prob;
  int char_seq_num;
  class data data;
};

/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is a global.     */
extern class world world;

extern pair_t all_dirs[8];

#define rand_dir(dir) {     \
  int _i = rand() & 0x7;    \
  dir[0] = all_dirs[_i][0]; \
  dir[1] = all_dirs[_i][1]; \
}

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

int new_map(int teleport);
void pathfind(map *m);

class pokemon {
  public:
    std::string name;
    int height;
    int weight;
    int experience;
    int level;
    int hp;
    int hp_IV;
    int base_hp;
    int attack;
    int attack_IV;
    int base_attack;
    int defense;
    int defense_IV;
    int base_defense;
    int speed;
    int speed_IV;
    int base_speed;
    int special_attack;
    int special_attack_IV;
    int base_special_attack;
    int special_defense;
    int special_defense_IV;
    int base_special_defense;
    std::string gender;
    bool is_shiny;
    std::vector<class move*> moves;
    pokemon(int index, int lvl) {
        pokemon_t *p = world.data.pokemon->at(index);
        name = p->identifier;
        height = p->height;
        weight = p->weight;
        experience = p->experience;

        std::vector<pokemon_move_t*> destination;

        int moveset = -1;
        do {
            auto temp_it = std::find_if(world.data.pokemon_moves->begin(), world.data.pokemon_moves->end(), [p, lvl](pokemon_move_t *pm) {
                return pm->pokemon_id == p->species_id && pm->level <= lvl && pm->pokemon_move_method_id == 1;
            });
            if (temp_it == world.data.pokemon_moves->end()) {
                lvl++;
            } else {
                moveset = (*temp_it)->version_group_id;
            }
        } while (moveset == -1);
        std::copy_if(world.data.pokemon_moves->begin(), world.data.pokemon_moves->end(), std::back_inserter(destination), [p, lvl, moveset](pokemon_move_t *pm) {
            return pm->pokemon_id == p->species_id && pm->level <= lvl && pm->pokemon_move_method_id == 1 && pm->version_group_id == moveset;
        });;

        if (destination.size() == 1) {
            auto it = std::find_if(world.data.moves->begin(), world.data.moves->end(), [destination](move_t *m) {
                return m->id == destination[0]->move_id;
            });
            if (it != world.data.moves->end()) {
                moves.push_back(new class move(*it));
            }
        } else {
            int index_1 = rand() % destination.size();
            int index_2 = rand() % destination.size();
            while (index_1 == index_2) { index_2 = rand() % destination.size(); }
            auto it = std::find_if(world.data.moves->begin(), world.data.moves->end(), [destination, index_1](move_t *m) {
                return m->id == destination[index_1]->move_id;
            });
            if (it != world.data.moves->end()) {
                moves.push_back(new class move(*it));
            }
            auto it2 = std::find_if(world.data.moves->begin(), world.data.moves->end(), [destination, index_2](move_t *m) {
                return m->id == destination[index_2]->move_id;
            });
            if (it2 != world.data.moves->end()) {
                moves.push_back(new class move(*it2));
            }
        }

        this->level = lvl;

        destination.clear();

        std::vector<pokemon_stat_t*> ps_destination;
        auto is_correct_id = [p](pokemon_stat_t *ps) {
            return ps->pokemon_id == p->id;
        };

        std::copy_if(world.data.pokemon_stats->begin(), world.data.pokemon_stats->end(), std::back_inserter(ps_destination), is_correct_id);
        for (pokemon_stat_t *ps : ps_destination) {
            switch (ps->stat_id) {
                case 1:
                    base_hp = ps->base_stat;
                    break;
                case 2:
                    base_attack = ps->base_stat;
                    break;
                case 3:
                    base_defense = ps->base_stat;
                    break;
                case 4:
                    base_speed = ps->base_stat;
                    break;
                case 5:
                    base_special_attack = ps->base_stat;
                    break;
                case 6:
                    base_special_defense = ps->base_stat;
                    break;
            }
        }

        hp_IV = rand() % 16;
        attack_IV = rand() % 16;
        defense_IV = rand() % 16;
        special_attack_IV = rand() % 16;
        special_defense_IV = rand() % 16;
        speed_IV = rand() % 16;

        hp = floor((((base_hp + hp_IV) * 2) * lvl) / 100) + lvl + 10;
        attack = floor((((base_attack + hp_IV) * 2) * lvl) / 100) + 5;
        defense = floor((((base_defense + hp_IV) * 2) * lvl) / 100) + 5;
        speed = floor((((base_speed + hp_IV) * 2) * lvl) / 100) + 5;
        special_attack = floor((((base_special_attack + hp_IV) * 2) * lvl) / 100) + 5;
        special_defense = floor((((base_special_defense + hp_IV) * 2) * lvl) / 100) + 5;
        gender = rand() % 2 == 1 ? "male" : "female";
        is_shiny = rand() % 8192 == 1;
    }
};

class pokemon* get_random_pokemon(int level);

#endif
