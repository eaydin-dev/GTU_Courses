//
// Created by EA on 24.03.2016.
//

#ifndef OS_CPU_H
#define OS_CPU_H

#include <vector>
#include "common.h"
#include "debugger.h"

using namespace std;

class cpu
{
public:
    cpu(){
        refresh();
        tickNum = 0;
    }


    inline void refresh() { running = true; }
    int tick(instruction * syscall, string * debug);
    inline void setContext(int pid, vector<instruction> i,
                           vector<int>::iterator b, vector<int>::iterator e) {
        this->pid = pid;
        this->instructions = i;
        this->b = b;
        this->e = e;
        init();
        //memory = vector<int>(b, e);
    }

    inline bool isHalted() { return !this->running; }
    inline void setDebugMode(int m) { debugMode = m; }
    string getDebugText(instruction inst);
    inline int getTickNum() { return tickNum; }
    //inline vector<int> getMem() { return memory; }
private:
    int pid;

    void init();

    int * mem0;
    int * end;
    int memSize;
    int * begin;
    vector<instruction> instructions;
    vector<int>::iterator b;
    vector<int>::iterator e;
    //vector<int> memory;
    int debugMode;
    int tickNum;
    bool running;
};


#endif //OS_CPU_H
