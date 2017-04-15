//
// Created by EA on 24.03.2016.
//

#ifndef OS_COMMON_H
#define OS_COMMON_H

using namespace std;

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>

#define PROCESS_RETURN_REGISTER 4

enum Command { set, cpy, cpyi, add, addi, jif, hlt, subi, cpyi2 };
const char * const CommandName[] = { "SET", "CPY", "CPYI", "ADD", "ADDI", "JIF", "HLT", "SUBI", "CPYI2" };

enum SystemCall { prn, fork, exec };
const char * const SystemCallNames[] = {"PRN", "FORK", "EXEC"};

enum Status { READY, BLOCKED, RUNNING, HALT, NOT_ASSIGNED };
const char * const StatusStrings[] = { "READY", "BLOCKED", "RUNNING", "HALT", "NOT_ASSIGNED"};

class instruction
{
public:
    instruction() : instruction(set, -1) { /* empty */ }

    instruction(Command c, int op1 = 0, int op2 = 0) {
        command = c;
        OP1 = op1;
        OP2 = op2;
        this->type = 0;
        this->nullPtr = false;
    }

    instruction(SystemCall s, int op1){
        this->sysCall = s;
        this->OP1 = op1;
        this->type = 1;
        this->nullPtr = false;
    }


    string toString() {
        stringstream ss;
        string str;
        if (command == hlt)
            return CommandName[hlt];
        else{
            ss << CommandName[command] << " " << OP1 << " " << OP2;
            return ss.str();
        }
    }

    bool nullPtr;
    Command command;                  //like "ADD" ,"SET" ,"JIF" ...
    SystemCall sysCall;
    long int OP1;
    long int OP2;
    int type;		// 0 for ordinary command, 1 for syscall
};

class process
{
public:
    process(string filename, int pid, int ppid){
        this->filename = filename;
        this->pid = pid;
        this->ppid = ppid;

        this->base = -1;
        this->limit = -1;
        this->status = NOT_ASSIGNED;
        this->physicalAddress = -1;
        this->ticks = 0;
        this->startTime = 0;
    }

    process(const process & other){
        this->filename = other.filename;
        this->pid = other.pid;
        this->ppid = other.ppid;

        this->instructions = other.instructions;
        this->base = other.base;
        this->limit = other.limit;

        this->physicalAddress = other.physicalAddress;
        this->stackPtr = other.stackPtr;
        this->status = READY;
        this->ticks = 0;
        this->startTime = other.getStartTime();
    }

    inline string toString(){
        stringstream ss;
        ss << "*** ";
        ss << "Name: " << setw(10) << filename << " || ";
        ss << "Pid/Ppid: " << setw(3) << pid << "/" << ppid << " || ";
        ss << "Start: " << setw(5) << startTime << " || ";
        ss << "Ticks: " << setw(5) << ticks << " || ";
        ss << "Status: " << setw(10) << StatusStrings[status] << " || ";
        ss << "P. Addr: " << setw(8) << physicalAddress << " || ";
        ss << "***" << endl;

        return ss.str();
    }
    inline int getBase() 			{ return base; }
    inline void setBase(int base) 	{ this->base = base; }
    inline  int getLimit() 			{ return limit; }
    inline void setLimit(int limit) { this->limit = limit; }
    inline int getPhysicalAddress() { return physicalAddress; }
    inline void setPhysicalAddress(int addr) { this->physicalAddress = addr; }

    inline void setStatus(Status s) { this->status = s; }
    inline Status getStatus()		{ return status; }
    inline void incTick()           { ++ticks; }
    const string &getFilename() const { return filename; }

    inline void addInstruction(instruction i) { instructions.push_back(i); }
    inline vector<instruction> * getInstructions() { return &instructions; }

    int getPid() const { return pid; }
    void setPid(int pid) { process::pid = pid; }
    int getPpid() const { return ppid; }
    void setPpid(int ppid) { process::ppid = ppid; }

    int getStartTime() const { return startTime; }
    void setStartTime(int startTime) { this->startTime = startTime; }

    int getTicks() const {return ticks;}

private:
    vector<instruction> instructions;
    int stackPtr;
    bool running;

    string filename;
    int pid;
    int ppid;
    int startTime;
    int ticks;
    Status status;
    int physicalAddress;

    int base;
    int limit;
    /* data */
};

typedef map<int,process> prMap;

#endif //OS_COMMON_H
