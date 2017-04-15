
#include "cpu.h"

int main(int argc, char * argv[]) {

	int mode = -5;
	string fileName;

	if (argc == 2)
		fileName = argv[1];
	else if (argc == 4) {
		fileName = argv[1];
		if (strcmp(argv[2], "-D") == 0 || strcmp(argv[2], "-d") == 0)
			mode = atoi(argv[3]);
	}
	else {
		cerr << "usage: Simulate <filename> -D 0" << endl;
		exit(1);
	}

	//cout << endl << fileName << ", " << argv[2] << ", " << mode << endl << endl;

	CPU cpu(fileName, mode);
	cpu.run();
	//cout << cpu.getMemoryContent();

	/*vector<mem> memory = cpu.getMem();
	int sizeMem = memory.size();

	for (i = 0; i < sizeMem; ++i) {
		cout << memory.at(i).index << " " << memory.at(i).value << endl;
	}*/

	/*vector<instruction> instructions = cpu.getInstructions();
	int sizeInst = instructions.size();

	for (i = 0; i < sizeInst; ++i) {
		cout << instructions[i].index << " " << instructions[i].command << " " << instructions[i].A << " " << instructions[i].B << endl;
	}*/

	//system("pause");
	return 0;
}
