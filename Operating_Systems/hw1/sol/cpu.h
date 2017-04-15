#ifndef CPU_H

#include <iostream>	// I/O
#include <stdlib.h>
#include <iomanip>
#include <vector>	// storing

/* parsing */
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

#include "Debugger.h"

using namespace std;

enum Command { set, cpy, cpyi, add, addi, jif, hlt, subi, cpyi2 };
const char * const CommandName[] = { "SET", "CPY", "CPYI", "ADD", "ADDI", "JIF", "HLT", "SUBI", "CPYI2" };


class instruction
{
public:
	instruction(Command c, int op1 = 0, int op2 = 0) {
		command = c;
		OP1 = op1;
		OP2 = op2;
	}

	Command command;                  //like "ADD" ,"SET" ,"JIF" ...
	long int OP1;
	long int OP2;
};


class CPU
{
public:
	CPU() : CPU("", -1) {/* empty */ }
	CPU(string file) : CPU(file, -1) {/* empty */ }
	CPU(string file, int mode) : fileName(file), PC(0), stackPtr(0), running(false)
	{
		parseFile(file);
		debugger.setDebugMode(mode);
		//this->PC = &memory[0];
	}

	void parseFile(string fileName);
	inline void parseFile() { this->parseFile(this->fileName); }

	void tick();
	void run();

	//inline void setPC(int pc) { *PC = pc; }
	inline void setStackPtr(int sPtr) { this->stackPtr = sPtr; }

	inline bool isHalted() { return !this->running; }

	//inline vector<mem> getMem() { return memory;  }
	//inline vector<instruction> getInstructions() { return instructions; }

private:
	string getDebugText(instruction inst, int mode);
	vector<int> memory;
	vector<instruction> instructions;
	string fileName;
	Debugger debugger;
	int * PC;
	int stackPtr;
	bool running;
};

#endif // !CPU_H
