CC = gcc
CFLAGS = -Wall -std=c99 $(shell pkg-config --cflags gtk+-2.0 gtksourceview-2.0) -DGTK_DISABLE_DEPRECATED=1
LDFLAGS = $(shell pkg-config --libs gtk+-2.0 gtksourceview-2.0)
OBJ = main.o callbacks.o print.o

.PHONY: clean

latexila: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o latexila

main.o: main.c main.h callbacks.h print.h

callbacks.o: callbacks.c callbacks.h main.h print.h

print.o: print.c print.h main.h

clean:
	rm -f $(OBJ) latexila
