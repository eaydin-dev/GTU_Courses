all:
	gcc -c -ansi -pedantic server.c -D_POSIX_C_SOURCE
	gcc -o server server.o -lm
	rm server.o
	gcc -c -ansi -pedantic client.c -D_POSIX_C_SOURCE
	gcc -o client client.o -lm
	rm client.o
