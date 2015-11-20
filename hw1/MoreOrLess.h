
/**
 *	The header file of MoreOrLess program.
 *
 *	I've looked http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/ 
 *	to understand how to use termios, unistd and select libraries.
 *
 *	And also I've looked http://mrbook.org/blog/tutorials/make/ to create a makefile.
 *
 *  UP ARROW: 		scroll one line up.
 *	DOWN ARROW: 	scroll one line down.
 *	RIGHT ARROW:	next page.
 *	LEFT ARROW:		previous page.
 *	ENTER:			scroll one line down.
 *
 *	Q or q:			quit program.
 */

#ifndef MORELESS
#define MORELESS

#include <stdio.h>
#include <stdlib.h>		
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#define LINE_SIZE 256
#define ARROW '\033'
#define UP '\101'
#define DOWN '\102'
#define RIGHT '\103'
#define LEFT '\104'
#define LITTLE_Q '\161'
#define CAPITAL_Q '\121'
#define ENTER '\012'


/**
 *	Rewinds the file and goes forward to set file the specific position.
 *  @param fInp: The file that data will be read.
 *  @param iWhere: The place where the cursor will be taken.
 */
void set_prev(FILE *fInp, int iWhere);

/*
 *	Checks if there is any action in standart input.
 *	@returns: A non-zero integer if there is an action; otherwise zero.
 */
int is_key_hitted();


/*
 *	Turns off the standard input procedure and makes it getting input without enter.
 */
void skip_enter();

/*
 *  Prints the lines and keeps and long pointer that will be used in down arrow.
 *	@param fInp: The file that data will be read.
 *	@param iShownLines: The amount of lines that will be shown on console.
 *	@param lpNext:	The pointer that shows the next of that function start to print. 
 */
void print(FILE *fInp, int iShownLines, long *lpNext);

#endif
