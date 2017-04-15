#include <iostream>
#include <cstring>
#include "os.h"

using namespace std;

int main(int argc, char * argv[]) {

    int mode = 3;
    string fileName; // = "/home/ea/Desktop/OS/examples/test.txt";

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

    cout << endl << fileName << ", " << argv[2] << ", " << mode << endl << endl;

    os os1(fileName, mode);
    os1.run();

    return 0;
}
