

#ifndef SERVERCLIENT
#define SERVERCLIENT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <sys/errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX 256
#define OP1_PARAMNUM 3
#define OP2_PARAMNUM 2
#define OP3_PARAMNUM 3
#define OP4_PARAMNUM 4

#define LOG_FILE_NAME "Log_%ld"
#define CLIENT_FIFO "cl_%ld"
#define SSLOG "SS Log"
#define PIDS "/tmp/pids.txt"
#define SIGUSR3 SIGWINCH
#define SIGUSR4 SIGURG

struct result
{
	int opId;
	double res1;
	double res2;
	double res3;
	double res4;
};

int getParamNum(int opId){
	switch(opId){
		case 1: return OP1_PARAMNUM;
				
		case 2: return OP2_PARAMNUM;
				
		case 3: return OP3_PARAMNUM;
				
		case 4: return OP4_PARAMNUM;
				
		default:
				fprintf(stderr, "Usage: You cannot enter less than 1 and grater that 4 as operation.\n");
				exit(1);
	}
}

int getResultNum(int opId){
	switch(opId){
		case 1:			
		case 2: return 1;
				
		case 3: return 2;
				
		case 4: return 4;
				
		default:
				fprintf(stderr, "Usage: You cannot enter less than 1 and grater that 4 as operation.\n");
				exit(1);
	}
}

int op1_f(double a, double b, double c, double *res){
	if(c == 0)
		return -1;
	*res = sqrt(a*a + b*b)/fabs(c);

	return 0;
}

int op2_f(double a, double b, double *res){
	if(a + b < 0)
		return -1;
	*res = sqrt(a + b);

	return 0;
}

int op3_f(double a, double b, double c, double *res1, double *res2){
	double determinant;

	determinant = b*b - 4*a*c;

	if(determinant < 0)
		return -1;

	else if(determinant > 0){
		*res1 = (-b + sqrt(determinant)) / 2*a;
		*res2 = (-b - sqrt(determinant)) / 2*a;
	}

	else{
		*res1 = -b / (2*a);
		*res2 = -b / (2*a);
	}

	return 0;
}

int op4_f(double a, double b, double c, double d, double *res_a, double *res_b, double *res_c, double *res_d){
	*res_a = -d;
	*res_b = b;
	*res_c = c;
	*res_d = -a;

	if(*res_c + *res_d == 0)
		return -1;

	return 0;
}

#endif