CC = gcc
CFLAGS = -g -W -Wall -std=c99 $(shell pkg-config --cflags gtk+-2.0)
LDFLAGS = $(shell pkg-config --libs gtk+-2.0)
OBJ = main.o

.PHONY: clean

latexila: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o latexila

main.o: main.c

clean:
	rm -f $(OBJ) latexila
