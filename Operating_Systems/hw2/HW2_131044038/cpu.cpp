//
// Created by EA on 24.03.2016.
//

#include <iomanip>
#include <iostream>
#include "cpu.h"

void cpu::init(){
    mem0 = &(*this->b);
    begin = &(*this->b);
    end = &(*this->e);
    memSize = (int) (end - begin);
}

int cpu::tick(instruction * syscall, string * debug) {

    //cout << "mem0: " << *mem0 << ", " << *begin << ", " << *end << endl;

    /*
     *
            ((((inst.OP2 > memSize)) && inst.command != set) ||
             (((inst.OP2 < 0) || (inst.OP2 > memSize)) && inst.command != add)) &&
            inst.command != hlt && inst.type != 1
     */
    instruction inst = instructions[*mem0];
    Command c = inst.command;
    if (((inst.OP2 > memSize || inst.OP2 < 0) ||
        ((inst.OP1 > memSize || inst.OP1 < 0) && (c != set && c != add))) &&
            inst.type == 0 && c != hlt)
    {
        cerr << "segmentation fault - " << CommandName[c] << " " << inst.OP1
        << " " << inst.OP2 << endl;
        exit(-1);
    }
    //PC = memory[0];
    if (inst.type == 0) {    // ordinary command
        switch (inst.command) {
            case set:
                *(b + inst.OP2) = inst.OP1;
                break;
            case cpy:
                *(b + inst.OP2) = *(b + inst.OP1);
                break;
            case cpyi:
                *(b + inst.OP2) = *(b + *(b + inst.OP1));
                break;
            case cpyi2:
                *(b + *(b + inst.OP2)) = *(b + inst.OP1);
                break;
            case add:
                *(b + inst.OP2) += inst.OP1;
                break;
            case addi:
                *(b + inst.OP2) += *(b + inst.OP1);
                break;
            case subi:
                *(b + inst.OP2) = *(b + inst.OP1) - *(b + inst.OP2);
                break;
            case jif:
                if (*(b + inst.OP1) <= 0) *mem0 = inst.OP2;
                else (*mem0)++;
                break;
            case hlt:
                running = false;
                break;
            default:
                break;
        }
    }

    else if (inst.type == 1) {
        syscall->OP1 = inst.OP1;
        //syscall->OP2 = inst.OP2;
        //syscall->command = inst.command;
        syscall->nullPtr = false;
        syscall->sysCall = inst.sysCall;
        syscall->type = inst.type;
    }

    if (inst.type == 1 || (inst.command != jif && inst.command != hlt && inst.OP2 != 0))
        (*mem0)++;

    debug->assign(getDebugText(inst));

    return ++tickNum;
}

string cpu::getDebugText(instruction inst) {
    stringstream ss;

    if (debugMode != 0) {
        ss << "NEXT COMMAND: ";
        if (inst.type == 0){
            ss << CommandName[inst.command];
            if (inst.command == hlt) ss << endl;
            else ss << " " << inst.OP1 << " " << inst.OP2 << endl;
        }
        else {
            ss << SystemCallNames[inst.sysCall];
            if (inst.sysCall == fork) ss << endl;
            else ss << " " << inst.OP1 << endl;
        }

        ss << "MEMORY AFTER COMMAND: " << endl;
    }

    ss << endl << "PC: " << *mem0 << endl;

    for (unsigned int i = 1; i <= memSize; ++i) {
        ss << "[" << setw(3) << i << " - " << setw(2) << *(b+i) << "] ";

        if (i % 10 == 0) ss << endl;
        //else             ss << ", ";
    }

    ss << endl << "\t\t\t ------ END OF MEMORY ------ \t\t" << endl;
    return ss.str();
}
