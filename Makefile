CC = gcc
CFLAGS = -ggdb -Wall -Werror -lm -Iinclude
SRCDIR = src
BINDIR = bin
TARGET = run
SUBDIRS = . data_structures map
SOURCES = $(foreach dir, $(SUBDIRS), $(wildcard $(SRCDIR)/$(dir)/*.c))
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)	#create the directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

tgz: clean
	tar cvzf Assignment1_Cooper-McKee.tgz --exclude=.vscode --exclude=.gitignore *
	
clean:
	rm -rf $(BINDIR) *.tgz run