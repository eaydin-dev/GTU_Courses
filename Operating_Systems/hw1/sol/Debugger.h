
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <string>

using namespace std;

class Debugger
{
public:
	Debugger() : Debugger(-1) {}
	Debugger(int m) { mode = m; }

	inline void setDebugMode(int m) { mode = m; }
	inline int getMode() { return mode; }
	inline void debug(string msg, bool isHalted = false) {
		if ((mode == 1) || (mode == 0 && isHalted))
			cout << msg;
		else if (mode == 2) {
			cout << msg;
			cout << endl << "Press enter to fetch next instruction ";
			getchar();	// wait for input key
			cout << endl;
		}
	}

private:
	int mode;
};

#endif
