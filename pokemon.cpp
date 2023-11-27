#include <cstdlib>
#include <algorithm>
#include <string>
#include <ncurses.h>
#include <vector>
#include <climits>
#include <cmath>
#include <iterator>

#include "pokemon.h"
#include "db_parse.h"
#include "poke327.h"
#include "io.h"

static bool operator<(const levelup_move &f, const levelup_move &s)
{
  return ((f.level < s.level) || ((f.level == s.level) && f.move < s.move));
}

static int pkmn_lvl()
{
    int md = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
            abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  int minl, maxl;

  if (md <= 200) {
    minl = 1;
    maxl = md / 2;
  } else {
    minl = (md - 200) / 2;
    maxl = 100;
  }
  if (minl < 1) {
    minl = 1;
  }
  if (minl > 100) {
    minl = 100;
  }
  if (maxl < 1) {
    maxl = 1;
  }
  if (maxl > 100) {
    maxl = 100;
  }

  return (rand() % (maxl - minl + 1)) + minl;
}

pokemon::pokemon() : pokemon(pkmn_lvl()) {}

pokemon::pokemon(int level) : level(level)
{
  pokemon_species_db *s;
  unsigned i, j;
  bool found;

  // Subtract 1 because array is 1-indexed
  pokemon_species_index = rand() % ((sizeof (species) /
                                     sizeof (species[0])) - 1);
  s = species + pokemon_species_index;
  
  if (!s->levelup_moves.size()) {
    // We have never generated a pokemon of this species before, so we
    // need to find it's level-up moveset and save it for next time.
    for (i = 1; i < (sizeof (pokemon_moves) /
                     sizeof (pokemon_moves[0])); i++) {
      if (s->id == pokemon_moves[i].pokemon_id &&
          pokemon_moves[i].pokemon_move_method_id == 1) {
        for (found = false, j = 0; !found && j < s->levelup_moves.size(); j++) {
          if (s->levelup_moves[j].move == pokemon_moves[i].move_id) {
            found = true;
          }
        }
        if (!found) {
          s->levelup_moves.push_back({ pokemon_moves[i].level,
                                       pokemon_moves[i].move_id });
        }
      }
    }

    // s->levelup_moves now contains all of the moves this species can learn
    // through leveling up.  Now we'll sort it by level to make that process
    // simpler.
    sort(s->levelup_moves.begin(), s->levelup_moves.end());

    // Also initialize base stats while we're here
    s->base_stat[0] = pokemon_stats[pokemon_species_index * 6 - 5].base_stat;
    s->base_stat[1] = pokemon_stats[pokemon_species_index * 6 - 4].base_stat;
    s->base_stat[2] = pokemon_stats[pokemon_species_index * 6 - 3].base_stat;
    s->base_stat[3] = pokemon_stats[pokemon_species_index * 6 - 2].base_stat;
    s->base_stat[4] = pokemon_stats[pokemon_species_index * 6 - 1].base_stat;
    s->base_stat[5] = pokemon_stats[pokemon_species_index * 6 - 0].base_stat;
  }

  // Get pokemon's move(s).
  for (i = 0;
       i < s->levelup_moves.size() && s->levelup_moves[i].level <= level;
       i++)
    ;

  // 0 is an invalid index, since the array is 1 indexed.
  move_index[0] = move_index[1] = move_index[2] = move_index[3] = 0;
  // I don't think 0 moves is possible, but account for it to be safe
  if (i) {
    move_index[0] = s->levelup_moves[rand() % i].move;
    if (i != 1) {
      do {
        j = rand() % i;
      } while (s->levelup_moves[j].move == move_index[0]);
      move_index[1] = s->levelup_moves[j].move;
    }
  }

  // Calculate IVs
  for (i = 0; i < 6; i++) {
    IV[i] = rand() & 0xf;
    effective_stat[i] = 5 + ((s->base_stat[i] + IV[i]) * 2 * level) / 100;
    if (i == 0) { // HP
      effective_stat[i] += 5 + level;
    }
  }

  shiny = (((rand() & 0x1fff) == 0x1fff) ? true : false);
  gender = ((rand() & 0x1) ? gender_female : gender_male);
}

const char *pokemon::get_species() const
{
  return species[pokemon_species_index].identifier;
}

int pokemon::get_level() const
{
  return level;
}

int pokemon::get_hp() const
{
  return effective_stat[stat_hp];
}

void pokemon::set_hp(int hp)
{
  effective_stat[stat_hp] = hp;
}

int pokemon::get_max_hp() const
{
  return std::floor(((species[pokemon_species_index].base_stat[stat_hp] + IV[stat_hp]) * 2 * level) / 100) + level + 10;
}

int pokemon::get_atk() const
{
  return effective_stat[stat_atk];
}

int pokemon::get_def() const
{
  return effective_stat[stat_def];
}

int pokemon::get_spatk() const
{
  return effective_stat[stat_spatk];
}

int pokemon::get_spdef() const
{
  return effective_stat[stat_spdef];
}

int pokemon::get_speed() const
{
  return effective_stat[stat_speed];
}

int pokemon::get_species_index() const
{
  return pokemon_species_index;
}

const char *pokemon::get_gender_string() const
{
  return gender == gender_female ? "female" : "male";
}

bool pokemon::is_shiny() const
{
  return shiny;
}

const char *pokemon::get_move(int i) const
{
  if (i < 4 && move_index[i]) {
    return moves[move_index[i]].identifier;
  } else {
    return "";
  }
}

int pokemon::get_move_index(int i) const {
  if (i < 4 && move_index[i]) {
    return move_index[i];
  } else {
    return -1;
  }
}

move_db get_move_at_move_index(int i) {
  return moves[i];
}

void do_pokemon_move(class pokemon *attacking_pokemon, move_db p_move, class pokemon *defending_pokemon, bool is_pc) {
  if (rand() % 100 < p_move.accuracy) {
    if (is_pc) {
      mvprintw(13, 4, "Your move hit! (press any key to continue)");
    } else mvprintw(13, 4, "The enemy's move hit! (press any key to continue)");
    getch();
    move(13, 0);
    clrtoeol();
    float critical;
    if (rand() % 256 < floor(attacking_pokemon->get_speed() / 2)) {
      critical = 1.5;
    } else {
      critical = 1.0;
    }
    float random = ((float) (rand() % 15 + 85)) / 100.0;
    float STAB = 1.0;
    std::vector<pokemon_types_db> p_types;
    std::copy_if(std::begin(pokemon_types), std::end(pokemon_types), std::back_inserter(p_types), [defending_pokemon](pokemon_types_db const& ptdb) { return ptdb.pokemon_id == defending_pokemon->get_species_index(); });
    for (size_t i = 0; i < p_types.size(); i++) {
      if (p_types[i].type_id == p_move.type_id) {
        STAB = 1.5;
      }
    }
    float type = 1.0;
    float x1 = ((2 * attacking_pokemon->get_level()) / 5) + 2;
    float x2 = p_move.power;
    float x3 = (float) attacking_pokemon->get_atk() / (float) defending_pokemon->get_def();
    float x4 = ((x1 * x2 * x3) / 50.0) + 2.0;
    int damage = x4 * critical * random * STAB * type;
    if (damage >= defending_pokemon->get_hp()) {
      defending_pokemon->set_hp(0);
    } else {
      defending_pokemon->set_hp(defending_pokemon->get_hp() - damage);
    }
  } else {
    mvprintw(13, 4, "Your move missed!");
    getch();
    move(13, 0);
    clrtoeol();
  }
  move(is_pc ? 8 : 5, 0);
  clrtoeol();
  mvprintw(is_pc ? 8 : 5, 4, "HP: %d", defending_pokemon->get_hp());
  refresh();
}

void pokemon_battle(class pokemon *pc_p, class pokemon *npc_p, bool skip_pc_move) {
  int num_npc_moves = 0;
  while (npc_p->get_move(num_npc_moves)[0] != '\0') {
    num_npc_moves++;
  }
  std::string s;
  if (!skip_pc_move) {
    std::string move1 = pc_p->get_move(0)[0] != '\0' ? "[1] " + std::string(pc_p->get_move(0)) : "";
    std::string move2 = pc_p->get_move(1)[0] != '\0' ? "[2] " + std::string(pc_p->get_move(1)) : "";
    std::string move3 = pc_p->get_move(2)[0] != '\0' ? "[3] " + std::string(pc_p->get_move(2)) : "";
    std::string move4 = pc_p->get_move(3)[0] != '\0' ? "[4] " + std::string(pc_p->get_move(3)) : "";
    move(11, 0);
    clrtoeol();
    mvprintw(11, 4, "%s  %s  %s  %s", move1.c_str(), move2.c_str(), move3.c_str(), move4.c_str());
    refresh();

    int input;
    int move_index = -1;
    while (move_index == -1) {
      input = getch();
      switch(input) {
        case '1':
          if (pc_p->get_move(0)[0] != '\0') {
            move_index = pc_p->get_move_index(0);
          }
          break;
        case '2':
          if (pc_p->get_move(1)[0] != '\0') {
            s = pc_p->get_move(1);
            move_index = pc_p->get_move_index(1);
          }
          break;
        case '3':
          if (pc_p->get_move(2)[0] != '\0') {
            s = pc_p->get_move(2);
            move_index = pc_p->get_move_index(2);
          }
          break;
        case '4':
          if (pc_p->get_move(3)[0] != '\0') {
            s = pc_p->get_move(3);
            move_index = pc_p->get_move_index(3);
          }
          break;
        default:
          break;
      }      
    }
    move_db pc_move = moves[move_index];
    move_db npc_move = moves[npc_p->get_move_index(rand() % num_npc_moves)];
    if (pc_move.priority > npc_move.priority) {
      do_pokemon_move(pc_p, pc_move, npc_p, true);
      if (npc_p->get_hp() > 0) {
        do_pokemon_move(npc_p, npc_move, pc_p, false);
      }
    } else if (npc_move.priority > pc_move.priority) {
      do_pokemon_move(npc_p, npc_move, pc_p, false);
      if (pc_p->get_hp() > 0) {
        do_pokemon_move(pc_p, pc_move, npc_p, true);
      }      
    } else if (pc_p->get_speed() > npc_p->get_speed()) {
      do_pokemon_move(pc_p, pc_move, npc_p, true);
      if (npc_p->get_hp() > 0) {
        do_pokemon_move(npc_p, npc_move, pc_p, false);
      }
    } else if (npc_p->get_speed() > pc_p->get_speed()){
      do_pokemon_move(npc_p, npc_move, pc_p, false);
      if (pc_p->get_hp() > 0) {
        do_pokemon_move(pc_p, pc_move, npc_p, true);
      }
    } else if (rand() % 2 == 0) {
      do_pokemon_move(pc_p, pc_move, npc_p, true);
      if (npc_p->get_hp() > 0) {
        do_pokemon_move(npc_p, npc_move, pc_p, false);
      }
    } else {
      do_pokemon_move(npc_p, npc_move, pc_p, false);
      if (pc_p->get_hp() > 0) {
        do_pokemon_move(pc_p, pc_move, npc_p, true);
      }
    }
  } else {
    move_db npc_move = moves[npc_p->get_move_index(rand() % num_npc_moves)];
    do_pokemon_move(npc_p, npc_move, pc_p, false);
  }
}

void capture_pokemon(class pokemon *encountered_pokemon) {
  if (world.pc.buddy[5] == NULL) {
    bool pokemon_added = false;
    int i = 1;
    while (!pokemon_added) {
      if (world.pc.buddy[i] == NULL) {
        world.pc.buddy[i] = encountered_pokemon;
        pokemon_added = true;
      } else {
        i++;
      }
    }
  }
}

void open_bag(class pokemon *pc_p, class pokemon *encountered_pokemon) {
  bool exit = false;
  while (!exit) {
    clear();
    mvprintw(4, 4, "Potions: %d", world.pc.bag.num_potions);
    mvprintw(5, 4, "Revives: %d", world.pc.bag.num_revives);
    mvprintw(6, 4, "Pokeballs: %d", world.pc.bag.num_pokeballs);
    if (encountered_pokemon != NULL) {
      mvprintw(8, 4, "You've encountered %s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d",
                  encountered_pokemon->is_shiny() ? "*" : "", encountered_pokemon->get_species(),
                  encountered_pokemon->is_shiny() ? "*" : "", encountered_pokemon->get_hp(), encountered_pokemon->get_atk(),
                  encountered_pokemon->get_def(), encountered_pokemon->get_spatk(), encountered_pokemon->get_spdef(),
                  encountered_pokemon->get_speed());
      mvprintw(10, 4, "[1] Use Potion\t [2] Use Revive\t [3] Use Pokeball\t [4] Exit");
    } else {
      mvprintw(7, 4, "Pokemon: %s\t HP: %d/%d", pc_p->get_species(), pc_p->get_hp(), pc_p->get_max_hp());
      mvprintw(9, 4, "[1] Use Potion\t [2] Use Revive\t [3] Use Pokeball\t [4] Exit");
    }
    refresh();
    int input = getch();
    switch(input) {
      case '1':
        if (world.pc.bag.num_potions > 0 && pc_p->get_hp() < pc_p->get_max_hp() && 
            encountered_pokemon == NULL && pc_p->get_hp() > 0) {
          world.pc.bag.num_potions--;
          pc_p->set_hp(pc_p->get_hp() + 20);
          if (pc_p->get_hp() > pc_p->get_max_hp()) {
            pc_p->set_hp(pc_p->get_max_hp());
          }
        } else {
          mvprintw(13, 4, "You can't use a potion on this pokemon right now! (press any key to continue)");
          getch();
        }
        break;
      case '2':
        if (world.pc.bag.num_revives > 0 && pc_p->get_hp() == 0 && encountered_pokemon == NULL) {
          world.pc.bag.num_revives--;
          pc_p->set_hp(pc_p->get_max_hp() / 2);
        } else {
          mvprintw(13, 4, "You can't use a revive on this pokemon right now! (press any key to continue)");
          getch();
        }
        break;
      case '3':
        if (encountered_pokemon != NULL && world.pc.bag.num_pokeballs > 0) {
          world.pc.bag.num_pokeballs--;
          if (world.pc.buddy[5] != NULL) {
            io_queue_message("The pokemon fled!");
          } else {
            capture_pokemon(encountered_pokemon);
            io_queue_message("You captured the pokemon!");
          }
          exit = true;
        } else {
          mvprintw(13, 4, "You can't use a pokeball right now! (press any key to continue)");
          input = getch();
        }
        break;
      case '4':
        exit = true;
        break;
      default:
        break;
    }
  }
}

void replenish_bag() {
  world.pc.bag.num_potions = 10;
  world.pc.bag.num_revives = 10;
  world.pc.bag.num_pokeballs = 10;
}

void heal_all_pokemon() {
  for (int i = 0; i < 6; i++) {
    if (world.pc.buddy[i] != NULL) {
      world.pc.buddy[i]->set_hp(world.pc.buddy[i]->get_max_hp());
    }
  }
}