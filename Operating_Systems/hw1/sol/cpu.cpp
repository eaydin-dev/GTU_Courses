
#include "cpu.h"

void CPU::tick() {
	instruction inst = instructions[memory[0]];
	//PC = memory[0];
	switch (inst.command) {
	case set:	memory[inst.OP2] = inst.OP1;							break;
	case cpy:	memory[inst.OP2] = memory[inst.OP1];					break;
	case cpyi:	memory[inst.OP2] = memory[memory[inst.OP1]];			break;
	case cpyi2:	memory[memory[inst.OP2]] = memory[inst.OP1];			break;
	case add:	memory[inst.OP1] += inst.OP2;							break;
	case addi:	memory[inst.OP2] += memory[inst.OP1];					break;
	case subi:	memory[inst.OP2] = memory[inst.OP1] - memory[inst.OP2];	break;
	case jif:	if (memory[inst.OP1] <= 0) memory[0] = inst.OP2; else memory[0]++;	break;
	case hlt:	running = false;										break;
	default:															break;
	}

	if (inst.command != jif && inst.command != hlt && inst.OP2 != 0)
		memory[0]++;

	debugger.debug(getDebugText(inst, debugger.getMode()), isHalted());
}

void CPU::run() {
	running = true;
	while (!isHalted()) {
		tick();
	}
}

void CPU::parseFile(string fileName) {

	ifstream file(fileName.c_str());
	string line;
	int index, value, op1, op2;
	Command command;
	string commandStr;

	while (getline(file, line)) {
		if (line == "" || line.at(0) == '#')
			continue;

		istringstream iss(line);
		//cout << line << endl;
		if (line == "Begin Data Section") {	// read data
			while (getline(file, line)) {
				if (line == "" || line.at(0) == '#')
					continue;
				istringstream iss(line);
				if (line != "End Data Section") {
					iss >> index >> value;
					//cout << index << " " << value << endl;
					this->memory.push_back(value);
				}
				else break;
			}
		}

		else if (line == "Begin Instruction Section") {
			while (getline(file, line)) {
				if (line == "" || line.at(0) == '#')
					continue;
				istringstream iss(line);
				if (line != "End Instruction Section") {
					iss >> index >> commandStr;
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

					if (command == hlt)
						this->instructions.push_back(instruction(command, -1, -1));
					else {
						iss >> op1 >> op2;
						this->instructions.push_back(instruction(command, op1, op2));
					}
				}
				else break;
			}
		}
	}

	this->stackPtr = this->memory[1];
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
		ss << "|" << setw(4) << i << " - " << setw(4) << memory[i] << "| ";

		if (i % 10 == 0)  ss << "\n";
		//else            ss << ", ";
	}

	ss << endl << "\t\t\t ------ END OF MEMORY ------ \t\t" << endl;
	return ss.str();
}
