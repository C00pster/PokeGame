CC = gcc
CFLAGS = -ggdb -Wall -Werror -lm -Iinclude
SRCDIR = src
BINDIR = bin
TARGET = run
SOURCES = $(wildcard $(SRCDIR)/*.c $(SRCDIR)/map/*.c $(SRCDIR)/data_structures/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)


all: $(TARGET)

$(TARGET): $(OBJECTS) #main.o character.o map/map_generation.o map/tile.o data_structures/priority_queue.o data_structures/queue.o
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)	#create the directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

# main.o: main.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# character.o: character.c character.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# map/map_generation.o: map/map_generation.c map/map_generation.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# map/tile.o: map/tile.c map/tile.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# data_structures/queue.o: data_structures/queue.c data_structures/queue.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# data_structures/priority_queue.o: data_structures/priority_queue.c data_structures/priority_queue.h
# 	$(CC) $(CFLAGS) -c $< -o $@

tgz: clean
	tar cvzf Assignment1_Cooper-McKee.tgz --exclude=.vscode --exclude=.gitignore *
	
clean:
	rm -rf $(BINDIR) *.tgz run