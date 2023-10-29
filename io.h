#ifndef IO_H
# define IO_H

# include "config.h"
# include "character.h"

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(int16_t dest[2]);
void io_queue_message(const char *format, ...);
void io_battle(Character *aggressor, Character *defender);

#endif
