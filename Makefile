CC = gcc
CFLAGS = -ggdb -Wall -Werror -lm

all: run

run: main.o map_generation.o queue.o
	$(CC) $(CFLAGS) main.o map_generation.o queue.o -o run

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

map_generation.o: map_generation.c map_generation.h
	$(CC) $(CFLAGS) -c map_generation.c -o map_generation.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

tgz: clean
	tar cvzf Assignment1_Cooper-McKee.tgz --exclude=.vscode --exclude=.gitignore *

clean:
	rm -f *.o map_generation *.tgz run