all:
	gcc -c -Wall Mine.c -D_SVID_SOURCE
	gcc -o Mine Mine.o -pthread
	gcc -c -Wall Miner.c -D_SVID_SOURCE
	gcc -o Miner Miner.o -pthread
	rm -f *.o
