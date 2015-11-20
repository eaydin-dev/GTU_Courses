CC=gcc
CFLAGS=-c -Wall -ansi -pedantic-errors

all: CountWords

CountWords: HW2.o
	$(CC) HW2.o -o HW2

HW2.o: HW2.c
	$(CC) $(CFLAGS) HW2.c

clean:
	rm *o MoreOrLess
