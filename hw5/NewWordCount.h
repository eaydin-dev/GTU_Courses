/********************************************************************/
/* Eyyüp Aydın - 131044038									  		*/
/* Implemented on 7 May.						  					*/
/*															  		*/
/* A program that finds number of uniuque words in a directory.   	*/
/*																	*/
/* Word split rules: Gets characters up to ' ' or '\n' and 	  		*/
/* eliminates punctations. (ignores a word if it has numericals)	*/
/*															  		*/
/* Usage: ./WordCount /home/username/Desktop/example_dir      		*/
/*															  		*/
/********************************************************************/

#ifndef COUNTNEWWORDS
#define COUNTNEWWORDS

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
#include <limits.h>

#define MAX 5000						/* maximum length  */
#define WORD_LOG_FILE "Words.log"
#define DIR_LOG_FILE "Directories.log"

/* a struct to send/get info to/from thread */
typedef struct {
   	/* first value	: file descriptor */
	int filedes;

	/* id of thread */
	pthread_t tid;

	/* num of all words in file */
	int iNumOfWords;

	/* num of unique words in file */
	int iNumOfWholeWords;
} wordcount_t;


/* the struct that keeps word and its amount */
typedef struct {
	char word[MAX];
	int amount;
} word_t;


/* the struct that keeps info about a directory, its name, num of words and elapsed time */
typedef struct {
	char directory[MAX];
	int iWordAmount;
	double elapsedTime;
} directory_t;


/*
 *	Gets the name of folder from a path.
 *
 *	@param sPath	:	the path to be examined.
 *	@param sName  	:	name of folder (return value).
 *
 *	@return lenght of sName.
 */
int getName(const char* sPath, char *sName);


/*
 *	Locks and unlock the mutex.
 *	Tries to add a word to data structures that keeps words and its amount.
 *	If this word is not in data structure, adds it and sets amount to 1; otherwise increases amount.
 *
 *	@param sWord 	:	the word to add.
 *
 *	@return 	1 if word added, 0 otherwise.
 */
int tryadd(const char *sWord);


/*
 *	Checks the global arrays and performs reallocation if needed.
 */
void check_allocation();


/*
 *	Checks if a character punctation or not.
 *	
 *	@return 1 if punctation, 0 otherwise.
 */
int is_punctuation(char ch);


/*
 *	Calculates total words in a directory recursively.
 *
 *	@param sDirectory:	string of directory that will be examined.
 *	@param nums      :	the struct that keeps num of files and directories.
 *
 *	@return num of words in directory.
 */
int total_words(const char * sDirectory, int *iFiles);


/*
 * The thread function that calculates num of words in a file.
 *
 * @param paramArr:	an array that first index for file descriptor and the second for result.
 *
 * @return num of words in a file.
 */
void * scan_file(void * thread_info);


/*
 *	Creates words' log file.
 *	@return num of unique words.
 */
int create_word_log();


/*
 *	Creates directories log file.
 *	@return num of directories.
 */
int create_directory_log();


/*
 *	Checks if given path is a hidden file or root directory.
 *
 *	@param sPath: the to be examined.
 *
 *	@return 1 or 0.
 */
int dont_look(const char *sPath);


/*
 * 	Checks if a word already written the log file or not.
 *	@param sWord: the word that will be checkedç
 * 	@return 1 if exist, 0 if not.
 */
int is_exist(const char *sWord, int *where);


/*
 *	Checks if sPath is a directory or not.
 *	@param sPath:	the string that will be checked.
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
int countWordsInDirectory(const char *sDirectory, int *iNumOfDirs, int *iNumOfFiles, int *iNumOfUniques);

#endif
