
#include "WordCount.h"

int main(int argc, char **argv){

	int iTotalWords = 0;
	int iDirs = 0, iFiles = 0;
	clock_t tstart, tend;
	double timedif;

	if(argc != 2){
		fprintf(stderr, "\nUsage: %s {path}\n\n", argv[0]);
		exit(1);
	}

	tstart = clock();
	iTotalWords = countWordsInDirectory(argv[1], &iDirs, &iFiles);
	tend = clock();

	timedif = (double) (tend - tstart) / CLOCKS_PER_SEC; 

	if(iTotalWords == -1)
		printf("Can't open the directory.\n");
	else		
		printf("\nTotal words: %d\nIn %d direcories, %d files.\n", iTotalWords, iDirs, iFiles);

	printf("Elapsed time: %f seconds (%f miliseconds).\n\n", timedif, timedif*1000);
	return 0;
}