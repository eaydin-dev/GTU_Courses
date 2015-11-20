
/*
 * Eyyup Aydin - 131044038
 * A program that finds single words and its amount in a directory, recursively. Creates a logfile in given path.
 */


#ifndef NEWWORDCOUNT
#define NEWWORDCOUNT 

#define MAX 256			/* maximum path length 	*/

#include <stdio.h>		/* fopen, fclose 		*/
#include <stdlib.h>		/* exit					*/
#include <unistd.h>		/* fork, pipe			*/
#include <dirent.h>		/* opendir, closedir 	*/
#include <string.h>		/* strtok, strcpy		*/
#include <sys/wait.h>	/* wait					*/
#include <sys/stat.h>	/* stat					*/
#include <sys/types.h>	/* pid_t				*/


/**
 *	Writes num of directories and num of files to pipe.
 *	iDir:		num of directories.
 *	iFile:		num of files.
 *	pipeArr:	the array that is being pipe to write and read.
 */
void write_nums(int iDir, int iFile, int *pipeArr);


/**
 *	Gets num of directories and num of files from pipe.
 *	iDir:		num of directories.
 *	iFile:		num of files.
 *	pipeArr:	the array that is being pipe to write and read.
 */
void scan_nums(int *iDir, int *iFile, int *pipeArr);


/*	Recursive function to go in all subdirectories and calculate num of words.
 * 	sDir: 			the directory that function will be work on.
 * 	iNumOfFiles: 	an integer pointer that keeps number of child processes.
 * 	iNumOpipeArrirs: 	an integer pointer that keeps number of subdirectories which recursive method worked on.
 *	sWordFile:		the path of the temporary file that keeps all words.
 *	pipeArr:	the array that is being pipe to write and read.
 */
void recursive_directory(const char *sDir, const char *sWordFile, int *pipeArr);


/* 	Calculates total number of words in a file.
 * 	fCurrentFile:	the file pointer to count words in it.
 *	sWordFile:		the path of the temporary file that keeps all words.
 */
int scan_file(FILE *fCurrentFile, const char *sWordFile);


/*	Checks the sPath is a directory or not.
 *	sPath:	the path to check.
 */
int is_directory(const char *sPath);


/**
 *	Count the num of sWord in the temporary file.
 *	sWord:		the word to be searched.
 *	sWordFile:	the path of the temporary file that keeps all words.
 */
int count_word(const char * sWord, const char *sWordFile);


/**
 *	Creates log file in the given path.
 *	sPath:	the path that file will be created.
 */
int create_report(const char *sPath);


/**
 *	Checks log file if sWord is already written or not.
 *	sWord:		the word to be searched.
 *	sLogFile:	the file path of the log file.
 */
int is_exist(const char *sWord, const char *sLogFile);

#endif