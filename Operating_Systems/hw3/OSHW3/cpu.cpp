
#include "cpu.h"

int CPU::tick() {

    int err = 0;
    int temp, temp2;

    temp = getMemory(0, &err);  // get PC
    if (err < 0)
        return err;

    instruction inst = currentProcess->instructions[temp];   // get next instruction
	switch (inst.command) {
        case set:	putMemory(inst.OP2, inst.OP1, &err);
            break;
        case cpy:
            temp = getMemory(inst.OP1, &err);
            if (err < 0)
                break;
            putMemory(inst.OP2, temp, &err);
            //memory[inst.OP2] = memory[inst.OP1];
            break;
        case cpyi:
            temp = getMemory(inst.OP1, &err);
            if (err >= 0)
                temp = getMemory(temp, &err);
            else
                break;
            if (err < 0)
                break;
            putMemory(inst.OP2, temp, &err);
            //memory[inst.OP2] = memory[memory[inst.OP1]];
            break;
        case cpyi2:
            temp = getMemory(inst.OP2, &err);
            if (err >= 0){
                temp2 = getMemory(inst.OP1, &err);
                if (err < 0)
                    break;

                putMemory(temp, temp2, &err);
            }
            //memory[memory[inst.OP2]] = memory[inst.OP1];
            break;
        case add:
            temp = getMemory(inst.OP2, &err);
            if (err >= 0)
                putMemory(inst.OP2, temp + inst.OP1, &err);
            break;
        case addi:
            temp = getMemory(inst.OP2, &err);
            if (err >= 0){
                temp2 = getMemory(inst.OP1, &err);
                if (err < 0)
                    break;
                putMemory(inst.OP2, temp + temp2, &err);
            }
            //memory[inst.OP2] += memory[inst.OP1];
            break;
        case subi:
            temp = getMemory(inst.OP1, &err);
            if (err >= 0){
                temp2 = getMemory(inst.OP2, &err);
                if (err < 0)
                    break;
                putMemory(inst.OP2, temp - temp2, &err);
            }
            //memory[inst.OP2] = memory[inst.OP1] - memory[inst.OP2];
            break;
        case jif:
            temp = getMemory(inst.OP1, &err);
            if (err >= 0){
                if (temp <= 0){     // set PC.
                    putMemory(0, inst.OP2, &err);
                    if (err < 0)
                        break;
                }
                else{   // increment PC
                    temp = getMemory(0, &err); // get PC's current value first
                    if (err >= 0){
                        // this operation might be successful of fail.
                        // it is handled out of this switch case.
                        putMemory(0, temp + 1, &err);
                    }
                }
            }
            break;
        case hlt:
            running = false;
            break;
        default:
            break;
	}

	if (err >= 0 && inst.command != jif && inst.command != hlt && inst.OP2 != 0){
        temp = getMemory(0, &err); // get PC's current value first
        if (err >= 0){
            putMemory(0, temp + 1, &err);
        }
    }

    if (debugger.getMode() != 3){
        debugger.debug(getDebugText(inst, debugger.getMode()) + (debugger.getMode() != 0 ? "\nPage Table:\n" + currentProcess->getPageTableStr() : ""), isHalted());
       // if (debugger.getMode() != 0)
         //   debugger.debug("\nPage Table:\n" + currentProcess->getPageTableStr(), isHalted());
    }

    return err; // a successful operation or a page fault...
}

void CPU::getOperands(instruction inst, int * op1, int * op2){
    //if (inst.command == )
}

int CPU::setMem(instruction * inst){
    int addr;
    if ((addr = getPAddr(inst->OP2)) < 0)
        return addr;

    memory[addr] = inst->OP1;
}

void CPU::putMemory(int addr, int value, int * err){
    int pAddr;
    if ((pAddr = getPAddr(addr)) < 0){
        if (debugger.getMode() == 3)
            cerr << "pf in putMemory, caused by: " << addr << endl;
        *err = pAddr;
    }
    else{   // make the desired operation and clear err flag.
        *err = 0;
        currentProcess->pageTable[addr / PAGE_SIZE].setModified(); // we put something to the mem, make it modified.
        memory[pAddr] = value;
    }
}

int CPU::getMemory(int addr, int * err){
    int pAddr;
    if ((pAddr = getPAddr(addr)) < 0){
        if (debugger.getMode() == 3)
            cerr << "pf in getMemory, caused by: " << addr << endl;
        *err = pAddr;   // set err.
        return -1;
    }
    else{
        *err = 0;   // clear err flag.
        currentProcess->pageTable[addr / PAGE_SIZE].setReferenced(); // we got something to the mem, make it referenced.
        return memory[pAddr];
    }
}

int CPU::getPAddr(int vAddr) {

    int page = vAddr / PAGE_SIZE;
    int offset = vAddr % PAGE_SIZE;

    if (page > currentProcess->pageTableSize){
        cout << "segmentation fault\n";
        return -1;
    }
    else if (!currentProcess->pageTable[page].isPresent()){ // page is not in RAM
        missingPage = page;
        return -2;
    }

    return
            currentProcess->pageTable[page].getFrameNo() * PAGE_SIZE + offset;
}

void CPU::run() {
	running = true;
	while (!isHalted()) {
		tick();
	}
}



string CPU::getDebugText(instruction inst, int mode) {
	stringstream ss;

	if (mode != 0) {
		ss << "NEXT COMMAND: " << CommandName[inst.command];
		if (inst.command == hlt) ss << endl;
		else ss << " " << inst.OP1 << " " << inst.OP2 << endl;

		ss << "MEMORY AFTER COMMAND: " << endl;
	}

	ss << endl << "PC: " << memory[0] << endl;

	for (unsigned int i = 1; i < memory.size(); ++i) {
		ss << "[" << i << " - " << memory[i] << "] ";

		if (i % 10 == 0)  ss << "\n";
		//else            ss << ", ";
	}

	ss << endl << "\t\t\t ------ END OF MEMORY ------ \t\t" << endl;
	return ss.str();
}
