CC = gcc
CFLAGS = -ggdb -Wall -Werror -lm

all: run config.h

run: main.o map_generation.o data_structures/queue.o character.o tile.o data_structures/priority_queue.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

map_generation.o: map_generation.c map_generation.h
	$(CC) $(CFLAGS) -c $< -o $@

data_structures/queue.o: data_structures/queue.c data_structures/queue.h
	$(CC) $(CFLAGS) -c $< -o $@

character.o: character.c character.h
	$(CC) $(CFLAGS) -c $< -o $@

tile.o: tile.c tile.h
	$(CC) $(CFLAGS) -c $< -o $@

data_structures/priority_queue.o: data_structures/priority_queue.c data_structures/priority_queue.h
	$(CC) $(CFLAGS) -c $< -o $@

tgz: clean
	tar cvzf Assignment1_Cooper-McKee.tgz --exclude=.vscode --exclude=.gitignore *
	
clean:
	rm -f *.o map_generation *.tgz run