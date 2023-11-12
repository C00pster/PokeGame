# include <string>
# include <vector>

# ifndef DATA_H
# define DATA_H

typedef struct {
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int experience;
    int order;
    int is_default;
} pokemon_t;

typedef struct {
    int id;
    std::string identifier;
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
} move_t;

typedef struct {
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
} pokemon_move_t;

typedef struct {
    int id;
    std::string identifier;
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
} pokemon_species_t;

typedef struct {
    int growth_rate_id;
    int level;
    int experience;
} experience_t;

typedef struct {
    int type_id;
    int local_language_id;
    std::string name;
} type_name_t;

typedef struct {
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
} pokemon_stat_t;

typedef struct {
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;
} stat_t;

typedef struct {
    int pokemon_id;
    int type_id;
    int slot;
} pokemon_type_t;

class data {
  public:
    std::vector<pokemon_t*> *pokemon;
    std::vector<move_t*> *moves;
    std::vector<pokemon_move_t*> *pokemon_moves;
    std::vector<pokemon_species_t*> *pokemon_species;
    std::vector<experience_t*> *experience;
    std::vector<type_name_t*> *type_names;
    std::vector<pokemon_stat_t*> *pokemon_stats;
    std::vector<stat_t*> *stats;
    std::vector<pokemon_type_t*> *pokemon_types;
};

class move {
  public:
    std::string name;
    int power;
    int accuracy;
    int pp;
    int priority;
    int target;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
    move(move_t *m) {
        name = m->identifier;
        power = m->power;
        accuracy = m->accuracy;
        pp = m->pp;
        priority = m->priority;
        target_id = m->target_id;
        damage_class_id = m->damage_class_id;
        effect_id = m->effect_id;
        effect_chance = m->effect_chance;
        contest_type_id = m->contest_type_id;
        contest_effect_id = m->contest_effect_id;
        super_contest_effect_id = m->super_contest_effect_id;
    }
};

# endif