CC = g++
CFLAGS = -c -std=c++11

all : os

os :	System.o cpu.o main.o
		$(CC) System.o cpu.o main.o -o Simulate

System.o :	System.cpp
		$(CC) $(CFLAGS) System.cpp

cpu.o :	cpu.cpp
		$(CC) $(CFLAGS) cpu.cpp

main.o :	main.cpp
		$(CC) $(CFLAGS) main.cpp

clean :	rm -rf *.o