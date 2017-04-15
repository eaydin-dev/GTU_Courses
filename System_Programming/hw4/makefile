all:
	gcc -c -pedantic-errors WordCount.c main.c
	gcc -o WordCount WordCount.o main.o -pthread
	rm -f *.o