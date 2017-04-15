
/**
 *	Main for MoreOrLess program.
 */


#include "MoreOrLess.h"

int main(int argc, char **argv)
{
	char cInp;			/* get input from stdin	*/
	char cTemp;			/* a temporary char that will help while determining which arrow you had pressed */
	int iShownLines = 5;/* how many lines is being showed */
    int iControl = 1;	/* loop control */
    int iWhere = 1;		/* which line we are */
	long int liNext = 0;		/* where should it start to print when you press down arrow */
    FILE *ifFile;
    
    if(3 != argc){
		printf("Missing arguments.\n");
		exit(1);
	}
    	 
    ifFile = fopen(argv[1], "r");  
    if(!ifFile){
		printf("Error while opening file.\n");
		exit(1);
	}
    
    /* get number of lines as integer	*/
    if(sscanf(argv[2], "%d", &iShownLines) == 0){
    	system("clear");
    	printf("Error\nYou've entered a non-numerical input as line number. You can enter a correct input know > ");
    	if(scanf("%d", &iShownLines) == 0){
    		printf("\n\nError\nYou've entered a non-numerical input again. Re-run the program with correct parameters please.\n\n");
    		exit(1);
    	}
    }
		
	/*	 print first page	*/
	system("clear");
	print(ifFile, iShownLines, &liNext); 
	
	/*	get inputs without enter  */
    skip_enter();
    while(iControl){
        iControl = !is_key_hitted();					/* check if is any key hitted	*/
        if (iControl != 0){ 							/* get input and analyze */   	
            cInp = getchar();
            if (LITTLE_Q  == cInp || CAPITAL_Q == cInp){	
	            system("clear");
                iControl = 0;							/* quit the loop and program */
            } 
            
            else if(ARROW == cInp){	/* arrows and esc starts with \033, ^[	*/
	            cInp = getchar();
				if('\133' == cInp){						/*	[  */				
					cTemp = getchar();					/*   A, B, C or D  */
					
					if(UP == cTemp){					
						system("clear");				/* clear the console */
						set_prev(ifFile, iWhere - 1);		/* seek the file specific position on file */
						print(ifFile, iShownLines, &liNext);
						--iWhere;						/* keep the line number */
						if(iWhere < 1)					/* make sure it is not out of range */
							iWhere = 1;
					}
					else if (DOWN == cTemp){			
						system("clear");				/* clear the console */
						fseek(ifFile, liNext, SEEK_SET);	/* seek to specific position on file */
						print(ifFile, iShownLines, &liNext);					
						++iWhere;						/* keep the line number	*/
					}				
					
					else if(RIGHT == cTemp){
						system("clear");				/* clear the console */
						print(ifFile, iShownLines, &liNext);		
						iWhere += iShownLines;			/* keep the line number	*/		
					}
					
					else if(LEFT == cTemp){
						system("clear");				/* clear the console */
						
						/* go to specific position of file */
						if(iWhere - iShownLines >= 0)
							set_prev(ifFile, iWhere - iShownLines);
						else
							set_prev(ifFile, 0);
							
						print(ifFile, iShownLines, &liNext);
						iWhere -= iShownLines;			/* keep the line number	*/	
						if(iWhere < 1)
							iWhere = 1;
					}
				}			
			}
					
			else if (ENTER == cInp){			
				system("clear");				/* clear the console */
				fseek(ifFile, liNext, SEEK_SET);	/* seek to specific position on file */
				print(ifFile, iShownLines, &liNext); 
				++iWhere;						/* keep the line number	*/
			}			
        }
    }
 	
 	fclose(ifFile);
    return 0;
}
