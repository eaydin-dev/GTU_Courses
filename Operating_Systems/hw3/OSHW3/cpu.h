#ifndef CPU_H

using namespace std;

#include <iostream>	// I/O
#include <stdlib.h>
#include <iomanip>
#include <vector>	// storing
/* parsing */
#include <string>


#include "CommonDefines.h"
#include "Debugger.h"
#include "Page.h"
#include "PageTableEntry.h"
#include "Process.h"

class CPU
{
public:
	CPU() : CPU(-1) {/* empty */ }
	CPU(int mode) : running(false)
	{

        int i;
		debugger.setDebugMode(mode);
        // initialize main memory
        for (i = 0; i < MAIN_MEMORY_SIZE; ++i){
            memory.push_back(-1);
        }

        running = true;
	}


	int tick();
	void run();

    void setMode(int m) { debugger.setDebugMode(m); }
    int getPAddr(int vAddr);
    void getOperands(instruction inst, int * op1, int * op2);
    int setMem(instruction * inst);
    void putMemory(int addr, int value, int * err);
    int getMemory(int addr, int * err);

	Process * getCurrentProcess() {
		return currentProcess;
	}

	void setCurrentProcess(Process * currentProcess) {
		CPU::currentProcess = currentProcess;
        //cout << "set curr process" << endl;
	}

    inline int getMissingPage() { return missingPage; }
	inline bool isHalted() { return !this->running; }

	//inline vector<mem> getMem() { return memory;  }
	//inline vector<instruction> getInstructions() { return instructions; }

	vector<int> memory;

private:
	string getDebugText(instruction inst, int mode);

	Debugger debugger;
	bool running;
    int missingPage;

private:
	Process * currentProcess = NULL;

};

#endif // !CPU_H
