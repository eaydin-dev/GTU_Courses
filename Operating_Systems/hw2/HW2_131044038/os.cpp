//
// Created by EA on 24.03.2016.
//

#include <iterator>
#include "os.h"

process * os::scheduler() {
    exitVal = 0;
    int i = 0;
    while(!done){
        if (!order.empty() && i < order.size()){
            i = i % order.size();
            process * p = &processTable.at(order[i]);
            if (p->getStatus() == READY){
                // context switch
                vector<int>::iterator b = mainMemory.begin()
                                          + p->getBase();
                vector<int>::iterator e = mainMemory.begin()
                                          + p->getLimit();

                //vector<int> subMem(b, e);
                myCpu.setContext(p->getPid(), *(p->getInstructions()), b, e);

                p->setStatus(RUNNING);
                order.erase(order.begin() + i);

                if (dbgr.getMode() == 2 && processNum != 1){
                    cout << endl << "-------------------------------------------------" << endl;
                    cout << "Context Switching" << endl;
                    cout << currentProcess->getFilename() << "(" << currentProcess->getPid() << ") to ";
                    cout << p->getFilename() << "("<< p->getPid() << ")" << endl;
                    cout << "CPU Tick Number: " << myCpu.getTickNum() << endl;
                    cout << endl << "-------------------------------------------------" << endl;
                    if (outputVal == 1){
						cout << endl << "press enter";
						getchar();
						cout << endl;	
					}
                }

                return p;
            }
            if (i == order.size()-1){
                if (sysCallTick() == 0) i = 0;
            }
            else i++;
        }
        else if (blockedProcesses.empty()) return NULL;
        else { if (sysCallTick() == 0) i = 0; }

        //++i;
    }
    return NULL;
}

void os::run() {
    instruction sys;
    sys.nullPtr = true;
    string debugText;
    //vector<instruction> sysCallVec;

    while (!done) {
        tickCount = myCpu.tick(&sys, &debugText); // execute next instruction
        currentProcess->incTick();

        //cout << "$$$ " << currentProcess->getStartTime() << endl;

        if (!sys.nullPtr) {     // process has a syscall. we shall
            // block that process and execute syscall.
            blockedProcesses.insert(pair<process *, instruction>
                                            (currentProcess, sys));
            sys.nullPtr = true;
            currentProcess->setStatus(BLOCKED);
            //order.push_back(currentProcess->getPid());
        }

        if (myCpu.isHalted()){ // if process is halted, delete it from process table.
            processTable.erase(currentProcess->getPid());
            myCpu.refresh();
            currentProcess = scheduler();
            if (currentProcess == NULL)
                done = true;
        }

        if (!done && currentProcess != NULL && (tickCount % 5 == 0 ||
                                                currentProcess->getStatus() == BLOCKED)) {
            //currentProcess->setStatus(READY);
            if (currentProcess->getStatus() == RUNNING){
                order.push_back(currentProcess->getPid());
                currentProcess->setStatus(READY);
            }

            //order.push_back(currentProcess->getPid());
            currentProcess = scheduler();
            if (currentProcess == NULL)
                done = true;
        }

        dbgr.debug(debugText, done, &processTable);
        sysCallTick();  // if there is a system call waiting, handle it.
    }
}

int os::sysCallTick() {
    if (!blockedProcesses.empty()){
        map<process *, instruction>::iterator it = blockedProcesses.begin();
        pair<process *, instruction> pair1 = *it;
        instruction sys = pair1.second;
        process * p = pair1.first;

        switch (sys.sysCall) {
            case prn:
                if (sys.OP1 < 0 ||
                    sys.OP1 > p->getLimit() - p->getBase()) {
                    cerr << "segmentation fault" << endl;
                    exit(-1);
                }

                cout << endl << "syscall: "
                << mainMemory[sys.OP1 + p->getBase()] << ", " << "pid: " << p->getPid()
                << endl;
                break;

            case fork:
                sysFork(p);
                break;

            case exec:
                sysExec(p, sys.OP1);
                break;
        }

        p->setStatus(READY);
        blockedProcesses.erase(p);
        order.push_back(p->getPid());

        return 0;
    }

    return 1;
}

process os::getProgram(string filename, vector<int> * execVec) {
    process p(filename, -1, -1);

    p.setBase(mainMemory.size());
    p.setPhysicalAddress(p.getBase());

    ifstream file(p.getFilename().c_str());
    cout << "is open: " << (file.is_open() ? "true" : "false") << endl;
    string line = "";
    int index, value, op1, op2, i;
    Command command;
    SystemCall sysCall;
    string sysString;

    string text;
    string commandStr;
    vector<string> temp;

    while (getline(file, line))
        temp.push_back(line);

    for (i = 0; i < temp.size(); ++i) {
        if (i == MEM_SIZE){
            cerr << "memory is full" << endl;
            exit(1);
        }
        line = temp.at(i);
        line.erase(line.find_last_not_of("\n\r") + 1);
        istringstream iss(line);

        if (strcmp(line.c_str(), "Begin Data Section") == 0)
            continue;
        else if (strcmp(line.c_str(), "End Data Section") == 0)
            break;
        else {
            iss >> index >> value;
            execVec == NULL ? this->mainMemory.push_back(value) :
            execVec->push_back(value);
        }
    }

    //cout << "memsize: " << mainMemory.size() << ", i: " << i-1 << endl;
    p.setLimit(i-2);

    for (i++; i < temp.size(); ++i) {
        line = temp.at(i);
        line.erase(line.find_last_not_of("\n\r") + 1);
        istringstream iss(line);

        if (strcmp(line.c_str(), "Begin Instruction Section") == 0 ||
            strcmp(line.c_str(), "End Data Section") == 0)
            continue;
        else if (strcmp(line.c_str(), "End Instruction Section") == 0)
            break;

        else {
            iss >> index >> text;
            if (text == "CALL") {
                iss >> sysString;
                if (sysString == SystemCallNames[0]) sysCall = prn;
                else if (sysString == SystemCallNames[1]) sysCall = fork;
                else if (sysString == SystemCallNames[2]) sysCall = exec;
                else {
                    cerr << "wrong call" << endl;
                    exit(1);
                }

                if (sysCall != fork) {
                    iss >> op1;
                    p.addInstruction(instruction(sysCall, op1));
                }
                else p.addInstruction(instruction(sysCall, -1));

            }

            else {
                commandStr = text;
                if (commandStr == CommandName[0]) command = set;
                else if (commandStr == CommandName[1]) command = cpy;
                else if (commandStr == CommandName[2]) command = cpyi;
                else if (commandStr == CommandName[3]) command = add;
                else if (commandStr == CommandName[4]) command = addi;
                else if (commandStr == CommandName[5]) command = jif;
                else if (commandStr == CommandName[6]) command = hlt;
                else if (commandStr == CommandName[7]) command = subi;
                else if (commandStr == CommandName[8]) command = cpyi2;
                else {
                    cerr << "unknown command\n";
                    exit(1);
                }

                if (command == hlt) {
                    p.addInstruction(instruction(command, -1, -1));
                }

                else {
                    iss >> op1 >> op2;
                    p.addInstruction(instruction(command, op1, op2));
                }
            }
        }
    }

    //cout << "mem size: " << mainMemory.size() << ", inst: " <<
    // p.instructions.size() << endl;

    return p;
    //this->stackPtr = this->memory[1];
}

int os::sysFork(process * p) {
    int i = 0;
    process child(*p);
    child.setPid(++processNum);
    child.setPpid(p->getPid());
    child.setStartTime(tickCount);
    child.setBase(mainMemory.size());

    child.setPhysicalAddress(child.getBase());

    vector<int>::const_iterator it = mainMemory.begin() + p->getBase();
    vector<int>::const_iterator end = mainMemory.begin() + p->getLimit() + 1;

    if (mainMemory.size() + (end-it) == MEM_SIZE){
        cerr << "mem is full" << endl;
        *(mainMemory.begin() + p->getBase() + PROCESS_RETURN_REGISTER) = -1;
    }
    cout << "/////////// MEM ///////////" << endl;
    while(it != end){
        mainMemory.push_back(*it);
        ++it;
    }
    cout << "/////////// MEM ///////////" << endl;

    child.setLimit(mainMemory.size() - 1);
    processTable.insert(pair<int, process>(child.getPid(), child)); // add process to
    // process table

    order.push_back(child.getPid());
    // assign return values on success
    *(mainMemory.begin() + p->getBase() + PROCESS_RETURN_REGISTER) =
            child.getPid();
    *(mainMemory.begin() + child.getBase() + PROCESS_RETURN_REGISTER) = 0;

    // on failure ?
    return 0;
}

int os::sysExec(process *p, int addr) {
    string filename;
    int i;
    stringstream ss;

    vector<int> newMem;
    vector<int>::iterator memIt = mainMemory.begin();
    vector<int>::iterator pMem = mainMemory.begin() + p->getBase();
    vector<int>::iterator fileNameLoc = mainMemory.begin()+p->getBase()+ addr;

    if (fileNameLoc < (memIt + p->getBase()) || fileNameLoc > (memIt + p->getLimit())){
        cerr << "segmentation fault - exec" << endl;
        exit(-1);
    }

    for(i = 0; *(fileNameLoc+i) != 0; ++i){
        ss << (char) *(fileNameLoc+i);
    }

    filename = ss.str();
    cout << "file: " << filename << endl;
    process fresh = getProgram(filename, &newMem);

    if (newMem.size() > (p->getLimit() - p->getBase() + 1)){
        cerr << "not enough memory to overlay new program - exec" << endl;
        *(mainMemory.begin() + p->getBase() + PROCESS_RETURN_REGISTER) = -1;
        return 0;
    }

    for (i = 0; i < newMem.size(); ++i){
        *(pMem + i) = newMem.at(i);
    }

    p->getInstructions()->clear();
    vector<instruction> * insts = fresh.getInstructions();
    for (i = 0; i < insts->size(); ++ i)
        p->addInstruction(insts->at(i));

    *(mainMemory.begin() + p->getBase() + PROCESS_RETURN_REGISTER) = 0;
}





/*void os::restoreMem(vector<int> mem) {
    int i;
    int counter = 0;
    for (i = currentProcess.getBase(); i <= currentProcess.getLimit(); ++i) {
        mainMemory[i] = mem[counter++];
    }
}*/



