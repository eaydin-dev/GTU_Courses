CC = g++
CFLAGS = -c -std=c++11


all :	Sim

Sim :	os.o cpu.o main.o
		$(CC) os.o cpu.o main.o -o Simulate

os.o :	os.cpp
		$(CC) $(CFLAGS) os.cpp
		
cpu.o :	cpu.cpp
		$(CC) $(CFLAGS) cpu.cpp

main.o :	main.cpp
		$(CC) $(CFLAGS) main.cpp

clean :
		rm *.o
