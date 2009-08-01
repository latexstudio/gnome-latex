CC = gcc
CFLAGS = -g -W -Wall -std=c99 $(shell pkg-config --cflags gtk+-2.0) -DGTK_DISABLE_DEPRECATED=1
LDFLAGS = $(shell pkg-config --libs gtk+-2.0)
OBJ = main.o callbacks.o error.o

.PHONY: clean

latexila: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o latexila

main.o: main.c main.h callbacks.h error.h

callbacks.o: callbacks.c callbacks.h main.h error.h

error.o: error.c error.h

clean:
	rm -f $(OBJ) latexila
