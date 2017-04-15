
#include "ClientServer.h"

void quit(int result);
void client_sigint_handler(int sig);
void client_sigfpe_handler(int sig);
void client_sigusr3_handler(int sig);
void client_sigusr2_handler(int sig);
void client_sigusr1_handler(int sig);
void client_sigquit_handler(int sig);

void initSignals();
void appendLog(char *sLogFileName, int iServerID, char *sFifo);
void writeResultsToLog(char *sLogFileName, struct result *res);
void writeResultsToConsole(struct result *res);

static int serverpid;
static pid_t sspid;
static int request = 1;
static char sLogFileName[MAX];
static char sMainFifoName[MAX];
static char sServerFifoName[MAX];
static int handlerflag = 0;

int main(int argc, char **argv){


	int sfiford;
	int sfifowr;
	int mfiford;
	int mfifowr;
	double x;

	int opID;
	double param;
	int i;
	int iWaitTime = 5;
	int iParams;
	int iLookedUp;
	struct result res;
	FILE *fPids;
	pid_t clientpid;

	initSignals();
	if(argc > 16){
		fprintf(stderr, "You can enter maximum 16 command-line arguments");
		exit(1);
	}

	if(argc < 6){
		fprintf(stderr, "You cannot enter less than 6 command-line arguments");
		exit(1);
	}

	fPids = fopen(PIDS, "a");
	fprintf(fPids, "%d\n", getpid());
	fclose(fPids);

	sprintf(sMainFifoName, "%s", &argv[1][1]);
	mkfifo(sMainFifoName, 0666);
	iWaitTime = atoi(&argv[2][1]);
	opID = atoi(&argv[3][1]);
	iLookedUp = 4;

	switch(opID){
		case 1: res.opId = 1;
				break;
		case 2: res.opId = 2;
				break;
		case 3: res.opId = 3;
				break;
		case 4: res.opId = 4;
				break;
		default:
				fprintf(stderr, "Usage: You cannot enter less than 1 and grater that 4 as operation.\n");
				exit(1);
	}
	iParams = getParamNum(opID);
	clientpid = getpid();

	mfifowr = open(sMainFifoName, O_WRONLY);
    if (write(mfifowr, &clientpid, sizeof(clientpid)) < 0 || write(mfifowr, &opID, sizeof(opID)) < 0 || write(mfifowr, &iWaitTime, sizeof(iWaitTime)) < 0){
        perror("write op");
        quit(EXIT_FAILURE);
    }
    close(mfifowr);

    mfiford = open(sMainFifoName, O_RDONLY);
    if(read(mfiford, &sspid, sizeof(sspid)) < 0 ||read(mfiford, &serverpid, sizeof(serverpid)) < 0 || read(mfiford, sServerFifoName, sizeof(sServerFifoName)) < 0){
    	perror("read client fifo");
        quit(EXIT_FAILURE);
    }
    close(mfiford);

    snprintf(sLogFileName, MAX, LOG_FILE_NAME, (long)serverpid);   
    fprintf(stdout, "client got pid and sServerFifoName name from main fifo: %d, %s\n", serverpid, sServerFifoName);

    mkfifo(sServerFifoName, 0666);

    while(iLookedUp + iParams <= argc){
    	sfifowr = open(sServerFifoName, O_WRONLY);
    	fprintf(stdout, "Sending parameters to math-server ");
    	for(i = 0; i < iParams; ++i){
    		param = (double) atoi(&argv[iLookedUp++][1]);
	    	if(write(sfifowr, &param, sizeof(param)) < 0){
	    		perror("write client fifo");
	       		quit(EXIT_FAILURE);
	    	}
	    	fprintf(stdout, "%f ", param);
	    }
	    fprintf(stdout, "\n");
	    close(sfifowr);

	    sfiford = open(sServerFifoName, O_RDONLY);
    	if(read(sfiford, &res, sizeof(res)) < 0){
    		perror("read server fifo");
    		quit(EXIT_FAILURE);
    	}
    	close(sfiford);

    	if(handlerflag != 1){
    		appendLog(sLogFileName, serverpid, sServerFifoName);
    		writeResultsToLog(sLogFileName, &res);
    		writeResultsToConsole(&res);
    	}
    	else
    		handlerflag = 0;
    	
    }
    sfifowr = open(sServerFifoName, O_WRONLY);
    x = -9999.0;
    write(sfifowr, &x, sizeof(x));
    close(sfifowr);
    unlink(sServerFifoName);
	return 0;
}

void appendLog(char *sLogFileName, int iServerID, char *sFifo){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");

	fprintf(fLog, "--------------------------\n");
	fprintf(fLog, "Reques ID: %d\n", request++);
	fprintf(fLog, "Connected math server: %d, via FIFO: %s\n", iServerID, sFifo);
	fclose(fLog);
}

void writeResultsToLog(char *sLogFileName, struct result *res){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	fprintf(fLog, "Result(s): ");
	
	switch(res->opId){
		case 1:
		case 2:
				fprintf(fLog, "%f\n", res->res1);
				break;
		case 3:
				fprintf(fLog, "%f and %f\n", res->res1, res->res2);
				break;
		case 4:
				fprintf(fLog, "%dx%+d / %dx%+d\n", (int)res->res1, (int)res->res2, (int)res->res3, (int)res->res4);
				break;
	}
	
	fprintf(fLog, "Status: Success\n");
	fclose(fLog);
}

void writeResultsToConsole(struct result *res){

	fprintf(stdout, "Result(s): ");
	
	switch(res->opId){
		case 1:
		case 2:
				fprintf(stdout, "%f\n", res->res1);
				break;
		case 3:
				fprintf(stdout, "%f and %f\n", res->res1, res->res2);
				break;
		case 4:
				fprintf(stdout, "%dx%+d / %dx%+d\n", (int)res->res1, (int)res->res2, (int)res->res3, (int)res->res4);
				break;
	}
	
	fprintf(stdout, "\nStatus: Success\n");
}

void quit(int result) {
	unlink(sServerFifoName);
	exit(result);
}

void client_sigquit_handler(int sig){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Unexpected termination.\n");
	fclose(fLog);
	kill(serverpid, SIGINT);
	
	kill(sspid, SIGQUIT);
	remove(PIDS);
	
	signal(sig, SIG_IGN);
    fprintf(stdout, "client got a SIGQUIT signal\n");
    signal(sig, client_sigquit_handler);
    handlerflag = 1;

    quit(EXIT_FAILURE);
}

void client_sigusr1_handler(int sig){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Delta is negative.\n");
	fclose(fLog);
	/*kill(serverpid, SIGINT);*/
	
	remove(PIDS);
	signal(sig, SIG_IGN);
    fprintf(stdout, "client got a SIGUSR1 signal\n");
    signal(sig, client_sigusr1_handler);
    handlerflag = 1;

   /* quit(EXIT_FAILURE);*/
}

void client_sigusr3_handler(int sig){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Sum of parameters is negative.\n");
	fclose(fLog);

	/*kill(serverpid, SIGINT);*/
	
	remove(PIDS);
	signal(sig, SIG_IGN);
    fprintf(stdout, "client got a SIGUSR3 signal\n");
    signal(sig, client_sigusr3_handler);
    handlerflag = 1;

   /* quit(EXIT_FAILURE);*/
}


void client_sigint_handler(int sig){
	FILE *fLog;

	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Caused by SIGINT.\n");
	fclose(fLog);

	kill(sspid, SIGUSR4);
	kill(serverpid, SIGINT);
	
	remove(PIDS);

	signal(sig, SIG_IGN);
    fprintf(stdout, "client got a SIGINT signal\n");
    signal(sig, client_sigint_handler);

    quit(EXIT_FAILURE);
}

void client_sigfpe_handler(int sig){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Division by zero.\n");
	fclose(fLog);

	remove(PIDS);

	signal(sig, SIG_IGN);
    fprintf(stdout, "client got a SIGFPE signal\n");
    signal(sig, client_sigfpe_handler);
    handlerflag = 1;
   
   	/*quit(EXIT_FAILURE);*/
}

void client_sigusr2_handler(int sig){
	FILE *fLog;
	fLog = fopen(sLogFileName, "a");
	appendLog(sLogFileName, serverpid, sServerFifoName);
	fprintf(fLog, "Status: Failed. Inverse of function is undefined.\n");
	fclose(fLog);

	remove(PIDS);

    fprintf(stdout, "\nclient got a SIGUSR2 signal\n");
    signal(sig, client_sigusr2_handler);
    handlerflag = 1;
    
    /*quit(EXIT_FAILURE);*/
}

void initSignals(){
	if (signal(SIGINT, client_sigint_handler) == SIG_ERR) {
        printf("SIGINT install error\n");
        exit(1);
    }
    if (signal(SIGFPE, client_sigfpe_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        exit(1);
    }
    if (signal(SIGUSR1, client_sigusr1_handler) == SIG_ERR) {
        printf("SIGUSR1 install error\n");
        exit(1);
    }
    if (signal(SIGUSR2, client_sigusr2_handler) == SIG_ERR) {
        printf("SIGUSR2 install error\n");
        exit(1);
    }
    if (signal(SIGUSR3, client_sigusr3_handler) == SIG_ERR) {
        printf("SIGUSR3 install error\n");
        exit(1);
    }
    if (signal(SIGQUIT, client_sigquit_handler) == SIG_ERR) {
        printf("SIGUSR3 install error\n");
        exit(1);
    }
}