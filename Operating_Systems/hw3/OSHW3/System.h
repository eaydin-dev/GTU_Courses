//
// Created by EA on 30.04.2016.
//

#ifndef OS3_SYSTEM_H
#define OS3_SYSTEM_H

#include "cpu.h"
#include "Process.h"
#include <random>
#include <queue>          // std::queue

using namespace std;

class System {
public:
    System(){
        int i;
        lastLoc = 0;
        for (i = 0; i < DISK_SIZE; ++i){
            disk.push_back(-1);
        }
    }

    int run();
    void handlePageFault();
    Process createProcess(string f);
    void setMode(int m) { mode = m; cpu.setMode(m); }

    void createRandomTestFile(int dataLen, string fileName);

    int secondChanceFifo();

private:
    void parseFile(string fileName, Process * p);
    void storeAndSetAbsentPage(int pageNum);

    queue<int> pageQueue;//MAIN_MEMORY_SIZE/PAGE_SIZE, -1);
    vector<Process> processes;
    CPU cpu;
    vector<int> disk;
    int mode;
    int lastLoc;
    int counter;
};


#endif //OS3_SYSTEM_H
