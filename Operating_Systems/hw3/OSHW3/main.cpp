#include <string.h>
#include "System.h"

int main(int argc, char * argv[]) {

	//cout << "lol :(" << endl;

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

	System os;
    //os.createRandomTestFile(2000, "randProg.ea");
    os.setMode(mode);
	os.createProcess(fileName);
    os.run();

	//system("pause");
	return 0;
}
