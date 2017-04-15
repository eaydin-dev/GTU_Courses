//
// Created by EA on 30.04.2016.
//

#include "System.h"

int System::run() {
    int err;
    while(!cpu.isHalted()){
        err = cpu.tick();
        if (err == -2){   // page fault occured.
            handlePageFault();
        }
        else if (err == -1){
            cerr << "whhoooaaa!! i am done here!" << endl << endl;
            break;
        }
    }
}

int System::secondChanceFifo(){
    int pageToBeReplaced = pageQueue.front();
    Process * cp = cpu.getCurrentProcess();
    if (cp->pageTable[pageToBeReplaced].isReferenced()){
        cp->pageTable[pageToBeReplaced].setUnReferenced();
        pageQueue.pop();
        pageQueue.push(pageToBeReplaced);
        return secondChanceFifo();
    }
    else {
        pageQueue.pop();
        return pageToBeReplaced;
    }
}

void System::createRandomTestFile(int dataLen, string fileName){
    ofstream outfile;
    outfile.open(fileName);

    stringstream ss;
    int i;

    random_device rd; // obtain a random number from hardware
    mt19937 eng(rd()); // seed the generator
    uniform_int_distribution<> distr(RAND_LOWEST, RAND_HIGHEST); // define the range

    ss << "Begin Data Section" << endl;
    for (i = 0; i < dataLen; ++i){
        ss << i << " " << ((i < 60) ? 0 : distr(eng)) << endl; // generate number
    }

    ss << "End Data Section" << endl;
    ss << "Begin Instruction Section" << endl << endl << "# code here " << endl << endl;
    ss << "End Instruction Section" << endl;

    outfile << ss.str() ;
    outfile.close();
}


void System::handlePageFault() {

    int i,j, limit;

    if (mode == 3){
        cout << "\nPage Fault Occured" << endl;
        cout << "Page Table:\n" + cpu.getCurrentProcess()->getPageTableStr() << endl;

        cout << "Second Chance FIFO Queue: " << endl;
        for (i = 0; i < pageQueue.size(); ++i){
            j = pageQueue.front();
            cout << j << " ";
            pageQueue.pop();
            pageQueue.push(j);
        }
        cout << endl;
    }

    int pageToBeReplaced = secondChanceFifo();//cpu.getCurrentProcess()->pageTable[secondChanceFifo()].getFrameNo();
    int oldPageFrame = cpu.getCurrentProcess()->pageTable[pageToBeReplaced].getFrameNo();

    if (mode == 3){
        cout << "Page " << pageToBeReplaced << " is replacing with Page " << cpu.getMissingPage() << endl;
    }

    storeAndSetAbsentPage(pageToBeReplaced);    // first, set absent the page table entry that was referring to page we would
                                        // like to replace

    PageTableEntry * pageTableEntry = &cpu.getCurrentProcess()->pageTable[cpu.getMissingPage()];

    j = pageTableEntry->getDiskLoc(); // get disk reference
    i = PAGE_SIZE*oldPageFrame;
    limit = i + PAGE_SIZE;
    for ( ; i < limit; ++i){
        cpu.memory[i] = disk[j++];
    }

    // set this page's new frame, set it is present now, set it is not modified and not referenced.
    pageTableEntry->setFrameNo(oldPageFrame);
    pageTableEntry->setPresent();
    pageTableEntry->setUnModified();
    pageTableEntry->setUnReferenced();

    pageQueue.push(cpu.getMissingPage());   // add this new page to the queue to.
}

void System::storeAndSetAbsentPage(int pageNum){
    int pageTableSize = VIRTUAL_MEMORY_SIZE/PAGE_SIZE;
    int i, j, limit;
    //int pte = pageNum;

    cpu.getCurrentProcess()->pageTable[pageNum].setAbsent();     // say that this page is not in memory anymore
    //cerr << "is present? > " << cpu.getCurrentProcess()->pageTable[pte].entry[2] << endl;
    if (cpu.getCurrentProcess()->pageTable[pageNum].isModified()){
        i = cpu.getCurrentProcess()->pageTable[pageNum].getDiskLoc();
        limit = i + PAGE_SIZE;
        j = cpu.getCurrentProcess()->pageTable[pageNum].getFrameNo() * PAGE_SIZE;    // the location of the page in
        // memory
        for ( ; i < limit; ++i)
            disk[i] = cpu.memory[j++];
    }

}

Process System::createProcess(string f){
    Process p;
    parseFile(f, &p);

    int i, j = 0;
    int numOfPagesInMem = 0;// = MAIN_MEMORY_SIZE/PAGE_SIZE;

    cout << "page table size: " << p.pageTable.size() << ", disk size: " << disk.size() <<  endl;
    // set disk locations of each pages for this process (not designed for multi-process systems)
    for (i = 0; i < disk.size()-1 && j < VIRTUAL_MEMORY_SIZE/PAGE_SIZE; i += PAGE_SIZE){
        p.pageTable[j++].setDiskLoc(i);
        //cout << "page table setting: " << i << endl;
    }
    //cout << "j first: " << j << endl;
    j = 0;

    // get the data that fits memory
    for (i = 0; i < MAIN_MEMORY_SIZE && i < lastLoc; ++i){
        cpu.memory[i] = disk[i];
        //cout << "i: " << i << ", disk[i]: " << disk[i] << endl;

        if (j % PAGE_SIZE == 0)
            ++ numOfPagesInMem;
        ++j;
    }

    //cout << "numOfPagesInMem: " << numOfPagesInMem << endl;

    // assign these data's page table entries
    for (j = 0; j < numOfPagesInMem; ++j){
        if (j != 0)
            pageQueue.push(j);
        p.pageTable[j].setFrameNo(j);
        p.pageTable[j].setPresent();
    }

    // set other pages' page table entries as not absent
    for(i = j ; i < VIRTUAL_MEMORY_SIZE/PAGE_SIZE; i++){
        //p.pageTable[j++].setFrameNo(i);
        p.pageTable[i].setAbsent();
    }

    cout << "returning process" << endl;
    processes.push_back(p);
    cpu.setCurrentProcess(&processes[0]);   // not designed for multi-process

    return p;
}

void System::parseFile(string fileName, Process * p) {
    ifstream file(fileName.c_str());
    cout << "is open: " << (file.is_open() ? "true" : "false") << endl;
    string line = "";
    int index, value, op1, op2, i;
    Command command;

    string text;
    string commandStr;
    vector<string> temp;

    while (getline(file, line))
        temp.push_back(line);

    for (i = 0; i < temp.size(); ++i) {
        line = temp.at(i);
        line.erase(line.find_last_not_of("\n\r") + 1);
        istringstream iss(line);

        if (strcmp(line.c_str(), "Begin Data Section") == 0)
            continue;
        else if (strcmp(line.c_str(), "End Data Section") == 0)
            break;
        else {
            iss >> index >> value;
            this->disk[lastLoc++] = value;
        }
    }

    cout << "disk: " << disk.size() << ", i: " << i << endl;

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
                p->instructions.push_back(instruction(command, -1, -1));
            }

            else {
                iss >> op1 >> op2;
                p->instructions.push_back((instruction(command, op1, op2)));
            }

        }
    }

    /*ifstream file(fileName.c_str());
    string line;
    int index, value, op1, op2;
    Command command;
    string commandStr;

    cout << "parsing\n";
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
    }*/
    cout << "size: " << p->instructions.size() << endl;
    //this->stackPtr = this->memory[1];
}
