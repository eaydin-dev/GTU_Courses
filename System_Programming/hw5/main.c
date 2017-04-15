
#include "NewWordCount.h"

int main(int argc, char **argv){

	int iTotalWords = 0;
	int iDirs = 0, iFiles = 0, iNumOfUniqueWords;

	if(argc != 2){
		fprintf(stderr, "\nUsage: %s {path}\nCheck header file.\n\n", argv[0]);
		exit(1);
	}

	iTotalWords = countWordsInDirectory(argv[1], &iDirs, &iFiles, &iNumOfUniqueWords);

	if(iTotalWords == -1){
		printf("Can't open the directory.\n");
		exit(1);
	}
	else{
		printf("All informations about this program written to header file.\n");
		printf("\nTotal words: %d\nTotal unique words: %d\nIn %d direcories, %d files.\n", iTotalWords, iNumOfUniqueWords, iDirs, iFiles);
	}

	printf("'%s' and '%s' files have created at current working directory.\n\n", WORD_LOG_FILE, DIR_LOG_FILE);

	return 0;
}