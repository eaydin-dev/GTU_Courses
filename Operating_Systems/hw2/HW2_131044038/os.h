//
// Created by EA on 24.03.2016.
//

#ifndef OS_OS_H
#define OS_OS_H

#include "common.h"
#include "cpu.h"

#include <iostream>	// I/O
#include <stdlib.h>
#include <iomanip>
#include <vector>	// storing
/* parsing */
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <queue>	// round robin
#include <map>

#define MEM_SIZE 2000

using namespace std;

class os
{
public:
	os() : os("", -1) {/* empty */ }
	os(string file) : os(file, -1) {/* empty */ }
	os(string file, int mode)
	{
		cout << "if you want to show the results step by step, enter 1 (recommended)" << endl <<
				"(if you not, enter anything) ";
		
		int temp;
		cin >> temp;
		
		if (temp == 1)
			outputVal = 1;
		else 
			outputVal = 0;
			
		this->contextSwitchHappened = false;
		this->processNum = 0;
		this->done = false;
		myCpu.setDebugMode(mode);
		dbgr.setDebugMode(mode, outputVal);

		// creating the first process of the cpu.
		// the last parameter 0 means that this process' parent is cpu itself.
		//process p1(file, ++processNum, 0);
		process p1 = getProgram(file, NULL);
		p1.setPid(++processNum);
		p1.setPpid(0);
		p1.setStatus(READY);
		p1.setStartTime(0);
		cout << " ";
		processTable.insert(pair<int, process>(p1.getPid(), p1));
		order.push_back(p1.getPid());
		//processTable.push_back(p1);
		currentProcess = scheduler();

		/*cout << currentProcess->getPid() << ", " << currentProcess->getPpid()
				<< ", " << currentProcess->getBase() << ", " <<
				currentProcess->getLimit() << endl;*/
	}

	void run();
	process getProgram(string filename, vector<int> * execVec);


private:
	//void restoreMem(vector<int> mem);
	process * scheduler();
	int sysCallTick();
	int sysFork(process * p);
	int sysExec(process * p, int addr);
	cpu myCpu;
	map<process *, instruction> blockedProcesses;
	vector<int> order;
	int exitVal = 0;
	process * currentProcess;// = process("null", -1, -1);
	//vector<process> processTable;
	prMap processTable;
	//vector<instruction> syscalls;
	vector<int> mainMemory;
	int processNum;
	bool done;
	bool contextSwitchHappened;
	debugger dbgr;
	int tickCount;
	int outputVal;
};



#endif //OS_OS_H
