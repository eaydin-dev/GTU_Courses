
#include "ClientServer.h"

static char sMainFifoName[MAX];
static int childpid;
static pid_t clientpid;
static int handlerflag = 0;
static int request = 1;
static int iServerDead = 0;
static int iCreatePipeFlag = 0;

int calculate(double *paramArr, struct result *res);
void sync_sigfpe_handler(int sig);
void sync_sigint_handler(int sig);
void sync_sigusr1_handler(int sig);
void sync_sigusr2_handler(int sig);
void sync_sigusr3_handler(int sig);
void sync_sigusr4_handler(int sig);
void sync_sigquit_handler(int sig);

void initSignals();
void writeResult(struct result *res);
void appendLog(int opID, int mserverpid);

void quit(int result) {
	unlink(sMainFifoName);
	exit(result);
}

int main(int argc, char **argv){


	int mfifowr;
	int mfiford;
	int sfifowr;
	int sfiford;
	int pipeArr[2];
	char serverFifoName[MAX];
	int iWaitTime;
	int opID;
	int iParams;
	double paramArr[4];
	struct result result_s;
	int i;
	int flag;
	int err = 0;
	pid_t thispid = getpid();
	
	if(argc != 2){
		fprintf(stderr, "Usage: %s -<mainFiFoName>\n", argv[0]);
		exit(1);
	}

	initSignals();
	sprintf(sMainFifoName, "%s", &argv[1][1]);
	mkfifo(sMainFifoName, 0666);

	if(pipe(pipeArr) < 0){
		perror("pipe");
		quit(EXIT_FAILURE);
	}

	while(1){	
		mfiford = open(sMainFifoName, O_RDONLY);
	    if(read(mfiford, &clientpid, sizeof(clientpid)) < 0 || read(mfiford, &opID, sizeof(opID)) < 0 || read(mfiford, &iWaitTime, sizeof(iWaitTime)) < 0){
	    	perror("read");
	        quit(EXIT_FAILURE);
	    }
	    close(mfiford);

	    result_s.opId = opID;
	    fprintf(stdout, "ss got opId: %d\n", opID);
	    
	    printf("lol\n");
	    childpid = fork();
	    if(childpid != -1){
	    	if(childpid == 0){
	    		signal(SIGINT, NULL);
	    		printf("childpid: %d\n", getpid());
	    		if(iCreatePipeFlag != 1){
	    			appendLog(result_s.opId, (int)getpid());

		    		snprintf(serverFifoName, MAX, CLIENT_FIFO, (long)getpid());

		    		if(mkfifo(serverFifoName, 0666) < 0){
		    			perror("mknod synchornyzation");
		    			quit(EXIT_FAILURE);
		    		}

		    		if(write(pipeArr[1], serverFifoName, sizeof(serverFifoName)) < 0){
		    			perror("write pipe");
		    			quit(EXIT_FAILURE);
		    		}
		    		iParams = getParamNum(opID);
		    	}

	    		flag = 1;
	    		while(1){
	    			sfiford = open(serverFifoName, O_RDONLY);

		    		for(i = 0; i < iParams; ++i){
		    			if(read(sfiford, &paramArr[i], sizeof(paramArr[i])) < 0){
		    				perror("read server fifo");
		    				quit(EXIT_FAILURE);
		    			}
		    			if(paramArr[i] == -9999){
		    				flag = -1;
		    				break;
		    			}
		    		}
		    		close(sfiford);
		    		if(flag == -1)
		    			break;

		    		sleep(iWaitTime);

		    		err = calculate(paramArr, &result_s);
		    		if(err == -3){
		    			kill(clientpid, SIGUSR1);
		    			kill(getppid(), SIGUSR1);
		    			handlerflag = 1;
		    		}
		    		else if(err == -1){
		    			kill(clientpid, SIGFPE);
		    			kill(getppid(), SIGFPE);
		    			handlerflag = 1;
		    		}
		    		else if(err == -4){
		    			kill(clientpid, SIGUSR2);
		    			kill(getppid(), SIGUSR2);
		    			handlerflag = 1;
		    		}
		    		else if(err == -2){
		    			kill(clientpid, SIGUSR3);
		    			kill(getppid(), SIGUSR3);
		    			handlerflag = 1;
		    		}

		    		sfifowr = open(serverFifoName, O_WRONLY);
		    		if(write(sfifowr, &result_s, sizeof(result_s)) < 0){
		    			perror("xxx write server fifo");
		    			quit(EXIT_FAILURE);
		    		}
		    		close(sfifowr);

		    		if(handlerflag != 1)
		    			writeResult(&result_s);
		    		
		    		handlerflag = 0;
		    	}

		    	request = 1;
	    		exit(0);
	    	}
	    	else{	/*  parent */
	    		if(read(pipeArr[0], serverFifoName, sizeof(serverFifoName)) < 0){
	    			perror("read from pipe");
	    			quit(EXIT_FAILURE);
	    		}

	    		mfifowr = open(sMainFifoName, O_WRONLY);

	    		if(write(mfifowr, &thispid, sizeof(thispid)) < 0 || write(mfifowr, &childpid, sizeof(childpid)) < 0 || write(mfifowr, serverFifoName, sizeof(serverFifoName)) < 0){
	    			perror("write main fifo");
	    			quit(EXIT_FAILURE);
	    		}
	    		close(mfifowr);
	    		/*** TTTTTTTTTTTTTTTo CCCCCLient	*/		
	    		wait(NULL);
	    	}
	    }
	}
	return 0;
}

void writeResult(struct result *res){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");

	fprintf(fLog, "Status: Success, Result(s): ");

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
	fclose(fLog);
}

void appendLog(int opID, int mserverpid){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");

	fprintf(fLog, "--------------------------\n");
	fprintf(fLog, "Client: %d, Operation ID: %d\n", clientpid, opID);
	fprintf(fLog, "Math Server: %d\n", mserverpid);
	fclose(fLog);
} 

int calculate(double *paramArr, struct result *res){
	switch(res->opId){
		case 1:
				if(op1_f(paramArr[0], paramArr[1], paramArr[2], &(res->res1)) == -1)
					return -1;
				break;

		case 2:
				if(op2_f(paramArr[0], paramArr[1], &(res->res1)) == -1)
					return -2;
				break;

		case 3:
				if(op3_f(paramArr[0], paramArr[1], paramArr[2], &(res->res1), &(res->res2)) == -1)
					return -3;
				break;
		case 4:
				if(op4_f(paramArr[0], paramArr[1], paramArr[2], paramArr[3], &(res->res1), &(res->res2), &(res->res3), &(res->res4)) == -1)
					return -4;
				break;
	}

	return 0;
}

void initSignals(){
	if (signal(SIGINT, sync_sigint_handler) == SIG_ERR) {
        printf("SIGINT install error\n");
        quit(EXIT_FAILURE);
    }
    if (signal(SIGFPE, sync_sigfpe_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    } 
    if (signal(SIGUSR1, sync_sigusr1_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    }
    if (signal(SIGUSR2, sync_sigusr2_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    }
    if (signal(SIGUSR3, sync_sigusr3_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    }
    if (signal(SIGUSR4, sync_sigusr4_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, sync_sigquit_handler) == SIG_ERR) {
        printf("SIGFPE install error\n");
        quit(EXIT_FAILURE);
    }
}

void sync_sigquit_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Unexpected termination.\n");
	fclose(fLog);

	quit(EXIT_FAILURE);
}

void sync_sigusr4_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Client got <CTRL-C> signal.\n");
	fclose(fLog);

	handlerflag = 1; 
}

void sync_sigusr1_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Delta is negative.\n");
	fclose(fLog);

    fprintf(stdout, "\nss got a SIGUSR1 signal\n");
    signal(sig, sync_sigusr1_handler);
    handlerflag = 1;
}

void sync_sigusr2_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Inverse of function is undefined.\n");
	fclose(fLog);

    fprintf(stdout, "\nss got a SIGUSR2 signal\n");
    signal(sig, sync_sigusr2_handler);
    handlerflag = 1;
    /*exit(1);*/
}

void sync_sigusr3_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Sum of parameters is negative.\n");
	fclose(fLog);

    fprintf(stdout, "\nss got a SIGUSR3 signal\n");
    signal(sig, sync_sigusr3_handler);
    handlerflag = 1;
}

void sync_sigint_handler(int sig){
	FILE *fPids, *fLog;
	int pid;

	fPids = fopen(PIDS, "r");
	if(fPids != NULL){
		while(fscanf(fPids, "%d", &pid) != EOF)
			kill(pid, SIGINT);
		fclose(fPids);
	}
	
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "\n-------------------\n<CTRL-C> signal has arrived. All processes are terminated.\n-------------------");
	fclose(fLog);

	remove(PIDS);
	kill(childpid, SIGINT);

    fprintf(stdout, "\nss got a SIGINT signal\n");
    signal(sig, sync_sigint_handler);
    quit(EXIT_FAILURE);
}

void sync_sigfpe_handler(int sig){
	FILE *fLog;
	fLog = fopen(SSLOG, "a");
	fprintf(fLog, "Status: Failed. Division by zero.\n");
	fclose(fLog);

	signal(sig, SIG_IGN);
    fprintf(stdout, "\nss got a SIGFPE signal\n");
    kill(clientpid, SIGFPE);
    signal(sig, sync_sigfpe_handler);
    handlerflag = 1;
}