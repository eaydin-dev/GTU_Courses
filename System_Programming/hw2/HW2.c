/*
 * Eyyup Aydin - 131044038
 * A program that finds all words in a directory recursively.
 */

#define MAX 256	/* maximum path length */
#define DATAFILE "data.txt~" /* a temp data that will be used to get info from child to parent */

#include <stdio.h>		/* fopen, fclose */
#include <stdlib.h>		/* exit	*/
#include <unistd.h>		/* fork	*/
#include <dirent.h>		/* opendir, closedir */
#include <string.h>		/* strtok	*/
#include <sys/wait.h>	/* wait		*/
#include <sys/stat.h>	/* stat		*/
#include <sys/types.h>	/* pid_t	*/


/*	Recursive function to go in all subdirectories and calculate num of words.
 * 	sDir: 			the directory that function will be work on.
 * 	iNumOfChilds: 	an integer pointer that keeps number of child processes.
 * 	iNumOfDirs: 	an integer pointer that keeps number of subdirectories which recursive method worked on.
 */
int recursive_directory(const char *sDir, int *iNumOfChilds, int *iNumOfDirs);


/* 	Calculates total number of words in a file.
 * 	fCurrentFile:	the file pointer to count words in it.
 */
int count_words(FILE *fCurrentFile);

/*	Checks the sPath is a directory or not.
 *	sPath:	the path to check.
 */
int is_directory(const char *sPath);


/*	START_OF_MAIN	*/
int main(int argc, char **argv) {

	int iNumChilds = 0;
	int iNumDirs = 0;
	int iNumWords = 0;
	
	if(argc != 2){
		fprintf(stderr, "Wrong arguments.\nIt should: %s [a_path]\n\n", argv[0]);
		exit(1);
	}

	iNumWords = recursive_directory(argv[1], &iNumChilds, &iNumDirs);

	printf("\nTotal words in all files: %d\n", iNumWords);
	printf("Total sub-directories: %d\n", iNumDirs);
	printf("Total child processes(files): %d\n\n", iNumChilds);

	return 0;
}
/* END_OF_MAIN	*/

int recursive_directory(const char *sDir, int *iNumOfChilds, int *iNumOfDirs) {
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	DIR *pDir;				/* directory pointer */
	FILE *fCurrentFile;		/* file that will be read */
	FILE *fTempData;		/* file that will be used to connect child and parent */
	pid_t pidchild;			/* pid_t to use fork function */

	int iTotalWords = 0;	/* will be used to append num of words of each file */
	int iChild = 0;			/* will be used to keep num of childs to assign iNumOfChilds at the end of function */
	int iCounts;			/* each file's num of words */
	char sSub[MAX];			/* a subdirectory or a file path */

	if((pDir = opendir(sDir)) == NULL)
		return 0;	/* there is no words if directory cannot open */

	while((pDirent = readdir(pDir)) != NULL){
		/* skip current directory and parent directory */
		if(!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, ".."))
			continue;

		/* make a sub path to investigate */
		sprintf(sSub, "%s/%s", sDir, pDirent->d_name);
		
		if(is_directory(sSub) == 1){
			++(*iNumOfDirs);	/* if this sub-path is a directory, increase num of dirs and call recursive */
			iTotalWords  += recursive_directory(sSub, iNumOfChilds, iNumOfDirs);
		}
		else if(sSub[strlen(sSub) - 1] != '~'){	/* skip hidden files */
			fCurrentFile = fopen(sSub, "r");
			if(fCurrentFile != NULL){
				pidchild = fork();
				if(pidchild >= 0){	/* success on fork	*/
					if(pidchild == 0){	/* child part	*/
						iCounts = count_words(fCurrentFile);
						++iChild;
						fTempData = fopen(DATAFILE, "w");
						/* write num of words and num of childs to file */
						if(fTempData != NULL)
							fprintf(fTempData, "%d %d", iCounts, iChild);

						fclose(fTempData);
						exit(0);	/* terminate child process	*/
					}
					else{	/* parent part. should wait all childs	*/
						wait(NULL);
						fTempData = fopen(DATAFILE, "r");
						if(fTempData != NULL){	/* get num of words and num of childs from file	*/
							fscanf(fTempData, "%d %d", &iCounts, &iChild);
							iTotalWords  += iCounts; /* increase total words	*/
						}
						
						/* close and delete temporary data file	*/
						fclose(fTempData);
						remove(DATAFILE);
					}
				}
			}
			fclose(fCurrentFile);
		}
	}
	closedir(pDir);
	*iNumOfChilds += iChild;
	return iTotalWords ;
}

int count_words(FILE *fCurrentFile){
	int iTemp = 0;
	int iResult = 0;
	char *sDelim = " ., ':;!";
	char *sToken;
	char sLine[MAX];
	
	/* get a line and split it */
	while(fgets(sLine, sizeof(sLine), fCurrentFile)) {
		sToken = strtok(sLine, sDelim);
		while(sToken != NULL){
			iTemp = sscanf(sToken, "%d", &iTemp);
			if(iTemp == 0)	/* check if it's a number or not	*/
				++iResult;
			sToken = strtok(NULL, sDelim);
		}
	}
	return iResult;
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}


