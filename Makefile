CC = gcc
CFLAGS = -ggdb -Wall -Werror

all: clean run

run: main.o terrain_generation.o queue.o
	$(CC) $(CFLAGS) main.o terrain_generation.o queue.o -o run

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

terrain_generation.o: terrain_generation.c terrain_generation.h
	$(CC) $(CFLAGS) -c terrain_generation.c -o terrain_generation.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

tgz: clean
	tar cvzf Assignment1_Cooper-McKee.tgz --exclude=.vscode *

clean:
	rm -f *.o terrain_generation *.tgz run