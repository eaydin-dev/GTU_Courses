CC=gcc
CFLAGS=-c -Wall -ansi -pedantic-errors

all: NewWordCount

NewWordCount: main.o NewWordCount.o
	$(CC) main.o NewWordCount.o -o NewWordCount

main.o: main.c
	$(CC) $(CFLAGS) main.c

NewWordCount.o: NewWordCount.c
	$(CC) $(CFLAGS) NewWordCount.c

clean:
	rm *.o NewWordCount