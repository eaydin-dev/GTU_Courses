#include <iostream>
#include <vector>
#include <string>

//you may need to add more functions

using std;

class mmry
{
public:
	mmry(int i,int v){
		index=i;
		value=v;
	}
	~mmry();
	int index;
	int value;
	
};


class instruction
{
public:
	instruction(int i,string &s,int a=0,int b=0){
		index=i;
		command=s;
		A=a;
		B=b;
	}
	~instruction();
	int index;
	string command;                  //like "ADD" ,"SET" ...
	int A;
	int B;

	
};




class Simulate
{
public:
	Simulate(string &filename,int md){
		parseFile(filename);
		mode=md;

	}
	~Simulate();
	vector<mmry> memory;
	vector<instruction> instructions;
	int mode;
	void parseFile(string &filename){
		//initialize memory and instructions after parsing file


	}

	void printMemory(){
		for (int i = 0; i < memory.size(); ++i)
		{
			cout<<memory[i].index<<"  "<<memory[i].value<<" , ";
			if(i%10==0){
				cout<<"\n";
			}
		}


	}


	void cpuRun(){


	}







	
};



int main(int argc, char *argv[]){

// here you should get the command line arguments and pass to the sim(string filename, int mode)

string filename();	

Simulate sim();

sim.cpuRun();


}


