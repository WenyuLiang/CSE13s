SHELL := /bin/sh
CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: monte_carlo 

monte_carlo: monte_carlo.o
	$(CC) -o monte_carlo monte_carlo.o

monte_carlo.o: monte_carlo.c
	$(CC) $(CFLAGS) -c monte_carlo.c

clean:
	rm -f *.o 

format:
	clang-format -i -style=file *.[ch]
