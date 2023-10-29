#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>
# include "config.h"

#define DIJKSTRA_PATH_MAX (INT_MAX / 2)

class Character {
  public:
    character_type_t ctype;
    movement_type_t mtype;
    int16_t pos[2];
    int next_turn;
    int seq_num;
    Character( character_type_t ctype, movement_type_t mtype, int16_t pos[2], int next_turn, int seq_num, char symbol ){
      this->ctype = ctype;
      this->mtype = mtype;
      this->pos[0] = pos[0];
      this->pos[1] = pos[1];
      this->next_turn = next_turn;
      this->seq_num = seq_num;
      this->symbol = symbol;
    }
    Character(){}; // default constructor
    virtual ~Character(){};
    char symbol;
};

class NPC : public Character {
  public:
    int defeated;
    int16_t dir[2];
    NPC( character_type_t ctype, movement_type_t mtype, int16_t pos[2], int16_t dir[2], int next_turn, int seq_num, char symbol ) 
      : Character( ctype, mtype, pos, next_turn, seq_num, symbol ){
      this->defeated = 0;
      this->dir[0] = dir[0];
      this->dir[1] = dir[1];
    }
    NPC() : Character(){} //default constructor
    ~NPC(){};
};

class PC : public Character {
  public:
    PC( character_type_t ctype, movement_type_t mtype, int16_t pos[2], int next_turn, int seq_num, char symbol ) 
      : Character( ctype, mtype, pos, next_turn, seq_num, symbol ){}
    PC() : Character(){} //default constructor
    ~PC(){};
};

/* character is defined in poke327.h to allow an instance of character
 * in world without including character.h in poke327.h                 */

int32_t cmp_char_turns(const void *key, const void *with);
void delete_character(void *v);

class Map;
void pathfind(Map *m);

extern void (*move_func[num_movement_types])(Character *, int16_t[2]);

int pc_move(char);

#endif
