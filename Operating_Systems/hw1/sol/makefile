CC = g++
CFLAGS = -c -std=c++11


all :	cpu

cpu :	cpu.o main.o
		$(CC) cpu.o main.o -o Simulate

cpu.o :	cpu.cpp
		$(CC) $(CFLAGS) cpu.cpp

main.o :	main.cpp
		$(CC) $(CFLAGS) main.cpp

clean :
		rm *.o *~ p1
