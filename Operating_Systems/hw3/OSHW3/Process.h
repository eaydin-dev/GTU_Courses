//
// Created by EA on 30.04.2016.
//

#ifndef OS3_PROCESS_H
#define OS3_PROCESS_H

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

#include <vector>
#include "CommonDefines.h"
#include "PageTableEntry.h"

using namespace std;


class Process {
public:
    Process(){
        int i;
        pageTableSize = VIRTUAL_MEMORY_SIZE/PAGE_SIZE;
        numOfPagesInMem = MAIN_MEMORY_SIZE/PAGE_SIZE;
        // initialize virtual table
        for (i = 0; i < pageTableSize; ++i){
            pageTable.push_back(PageTableEntry());
        }
    }

    string getPageTableStr(){
        stringstream ss;
        ss << endl;
        ss << "---- page table ----" << endl;
        for (int i = 0; i < PAGE_TABLE_PRINT_NUM; ++i){
            ss << i << " - " << pageTable[i].entry[0] << ", "
            << pageTable[i].entry[1] << ", " << pageTable[i].entry[2] << ", "
            << pageTable[i].entry[3] << ", " << pageTable[i].entry[4] << endl;
        }
        ss << "----    end    ----" << endl;

        return ss.str();
    }
    vector<PageTableEntry> pageTable;
    vector<instruction> instructions;
    int pageTableSize;
    int numOfPagesInMem;

private:


};


#endif //OS3_PROCESS_H
