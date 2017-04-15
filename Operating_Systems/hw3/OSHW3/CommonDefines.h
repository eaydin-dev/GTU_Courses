//
// Created by EA on 28.04.2016.
//

#ifndef OS3_COMMONDEFINES_H
#define OS3_COMMONDEFINES_H

using namespace std;

#define PAGE_TABLE_PRINT_NUM 50
#define DISK_SIZE 1048576
#define MAIN_MEMORY_SIZE 512
#define PAGE_SIZE 64
#define VIRTUAL_MEMORY_SIZE 32768

// for random nums
#define RAND_LOWEST 0
#define RAND_HIGHEST 500

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


#endif //OS3_COMMONDEFINES_H
