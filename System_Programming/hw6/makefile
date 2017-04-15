all:
	gcc -c -pedantic-errors -Wall NewWordCount.c main.c
	gcc -o NewWordCount NewWordCount.o main.o -pthread
	rm -f *.o