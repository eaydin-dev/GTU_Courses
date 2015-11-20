
/**
 *	The source file of MoreOrLess program.
 */

#include "MoreOrLess.h"

void print(FILE *ifInput, int iShownLines, long int *pliNext){
	int iLooper = 0;
	char szLine[LINE_SIZE];
	
	/* this if statement is here to just keep lpNext which will be used in down arrow */
	if(fgets(szLine, sizeof(szLine), ifInput)){
		*pliNext = ftell(ifInput);
		printf("%s", szLine);				
	}
	
	/*	print rest of iShownLines	*/
	for(iLooper = 0; iLooper < iShownLines-1; ++iLooper)
		if(fgets(szLine, sizeof(szLine), ifInput)){
			printf("%s", szLine);					
		}
}


void set_prev(FILE *ifInput, int iWhere){
	int iLooper = 0;
	char szLine[LINE_SIZE];
	
	rewind(ifInput);	/* go to beginning of file */
	
	/* forward until arrive iWhere */
	for(iLooper = 0; iLooper < iWhere - 1; ++iLooper)
		fgets(szLine, sizeof(szLine), ifInput);
}

int is_key_hitted(){
    struct timeval tv;
    fd_set fds;
    
    /* set second and microsecond 0 to check input without timeout	*/
    tv.tv_sec = 0;	
    tv.tv_usec = 0;
        
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);	/* check only standart input */ 
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);	/* set output and exception NULL */
    return FD_ISSET(STDIN_FILENO, &fds);	/* returns a non-zero integer if there is input	*/
}

void skip_enter(){

    struct termios ttystate;
 
    /* get the terminal state */
    tcgetattr(STDIN_FILENO, &ttystate);
 
    /* close canonical mode */
    ttystate.c_lflag &= ~ICANON;
        
    /* wait for just one input to read */
    ttystate.c_cc[VMIN] = 1;

    /* set the terminal attributes */
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}


