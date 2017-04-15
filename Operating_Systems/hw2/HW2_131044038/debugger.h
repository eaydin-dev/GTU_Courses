//
// Created by EA on 24.03.2016.
//

#ifndef OS_DEBUGGER_H
#define OS_DEBUGGER_H


#include <iostream>
#include <iomanip>
#include <string>
#include "common.h"

using namespace std;

class debugger
{
public:
	debugger() : debugger(-1) {}
	debugger(int m) { mode = m; }

	inline void setDebugMode(int m, int out) { mode = m; outputVal = out; }
	inline int getMode() { return mode; }
	inline void debug(string msg, bool isHalted = false, prMap * processTable = NULL) {
		if ((mode == 1) || (mode == 0 && isHalted)){
			cout << msg;
			if (outputVal == 1){
				cout << endl << "press enter";
				getchar();
				cout << endl;	
			}
		}
		else if (mode == 3 && !isHalted) {
			stringstream ss;
			map<int, process>::iterator it = processTable->begin();
			ss << endl << "-------------------------------------------------" << endl;
			while(it != processTable->end()){
				process * p = &(*it).second;
				ss << p->toString();
				++it;
			}
			ss << endl << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			cout << ss.str();
			
			if (outputVal == 1){
				cout << endl << "press enter";
				getchar();
				cout << endl;	
			}
		}
		else if (mode == 5){
			cout << msg;
			cout << endl << "press enter";
			getchar();
			cout << endl;
		}
	}

private:
	int mode;
	int outputVal;
};

#endif //OS_DEBUGGER_H
