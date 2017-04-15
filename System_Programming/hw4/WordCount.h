/**************************************************************/
/*															  */
/* Implemented by Eyyüp Aydın on 22 April.					  */
/*															  */
/* A program that finds number of all words in a directory.   */
/* Word split rules: " ., ':;!"								  */
/*															  */
/* Usage: ./WordCount /home/username/Desktop/example_dir      */
/*															  */
/**************************************************************/

#ifndef COUNTWORDS
#define COUNTWORDS

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>	
#include <sys/errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX 256						/* maximum path length 	*/

/* a struct to send/get info to/from thread */
typedef struct {
   	/* first value	: file descriptor */
	/* second value : return value (num of words in file) */
	int args[2];

	/* id of thread */
	pthread_t tid;

} wordcount_t;

/* the struct that used on recursive calls to keep num of files and directories */
typedef struct {
	int files;
	int directories;
} nums_t;


/*
 *	Calculates total words in a directory recursively.
 *
 *	@param sDirectory:	string of directory that will be examined.
 *	@param nums      :	the struct that keeps num of files and directories.
 *
 *	@return num of words in directory.
 */
int total_words(const char * sDirectory, nums_t * nums);

/*
 * The thread function that calculates num of words in a file.
 *
 * @param paramArr:	an array that first index for file descriptor and the second for result.
 *
 * @return num of words in a file.
 */
void * scan_file(void * paramArr);

/*
 *	Checks if sPath is a directory or not.
 *	
 *	@param sPath:	the string that will be checked.
 *
 *	@return 1 if it is directory, 0 otherwise.
 */
int is_directory(const char * sPath);


/*
 *	The wrapper for recursive total_words function.
 *
 *	@param sDirectory: 	string of directory that will be examined.
 *	@param iNumOfDirs: 	num of directories in sDirectory.
 *	@param iNumOfFiles: num of files in sDirectory.
 *
 *  @return num of words in sDirectory or
 *			-1 if can't open the given directory.
 */
int countWordsInDirectory(const char *sDirectory, int *iNumOfDirs, int *iNumOfFiles);

#endif