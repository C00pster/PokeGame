#ifndef IO_H
# define IO_H

class character;
typedef int16_t pair_t[2];

typedef enum{
    pokemon,
    moves,
    pokemon_moves,
    pokemon_species,
    experience,
    type_names,
    pokemon_stats,
    stats,
    pokemon_types
} csv_files;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(character *aggressor, character *defender);
void io_get_first_pokemon(class pc *player);

void io_load_data();

#endif
