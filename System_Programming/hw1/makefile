CC=gcc
CFLAGS=-c -Wall -ansi -pedantic-errors

all: MoreOrLess

MoreOrLess: main.o MoreOrLess.o
	$(CC) main.o MoreOrLess.o -o MoreOrLess

main.o: main.c
	$(CC) $(CFLAGS) main.c

MoreOrLess.o: MoreOrLess.c
	$(CC) $(CFLAGS) MoreOrLess.c

clean:
	rm *o MoreOrLess
