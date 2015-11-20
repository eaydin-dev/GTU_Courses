
#include "NewWordCount.h"

int main(int argc, char **argv) {

	int iNumFiles = 0;
	int iNumDirs = 0;
	int iNumWords = 0;

	pid_t pid;				/* fork return				*/
	char sWordFile[MAX];	/* temporary word file path */
	DIR *pDir;				/* root directory 			*/
	int iStatus; 			/* exit status of child 	*/
	int pipeArr[2];			/* pipe 					*/

	if(argc != 2){
		fprintf(stderr, "Usage: %s [a_path]\n\n", argv[0]);
		exit(1);
	}
	else if((pDir = opendir(argv[1])) == NULL){
		fprintf(stderr, "Can't open directory.\n\n");
		exit(1);
	}
	closedir(pDir);

	if(pipe(pipeArr) == -1){
		fprintf(stderr, "Error while creating pipe.\n");
		exit(1);
	}

	sprintf(sWordFile, "%s/%s", argv[1], "word_data");
	write_nums(1, 0, pipeArr);	/* write pipe num of paths as 1 (the root) and num of files as 0 */
	
	pid = fork();
	if(pid != -1){
		if(pid == 0){
			recursive_directory(argv[1], sWordFile, pipeArr);
		}
		else{
			wait(&iStatus);	/* there is only one child here */
			if(iStatus == 1){
				fprintf(stderr, "Some errors in child processes.\n");
				exit(1);
			}
		
			scan_nums(&iNumDirs, &iNumFiles, pipeArr);	
			iNumWords = create_report(argv[1]);
		
			printf("\nTotal words: %d\n%d files have been scanned.\n%d directories exist (including root).\n\n", iNumWords, iNumFiles, iNumDirs);
			printf("The log file created on the given path as 'Words.log'\n\n");

			remove(sWordFile);
		}
	}
	else{
		fprintf(stderr, "Error while forking.\n");
		exit(1);
	}
	
	return 0;
}