/*
*
*   procfxns- Functions for copying and parsing proc information
*       All this code in this file is compeletly written by me.
*           Author: Matt Almenshad		
*/

#ifndef _PROCFXNS_H_INCLUDED_
#define _PROCFXNS_H_INCLUDED_

/*
*   p1strcpy - copies char * 'dst' and a char * 'dst' until it reaches a '\0'
*/
void p1strcpy(char *dst, char *src);

/*
*   parsePid - takes a pid_t 'pid' and parses it into the buffer buf[]
*/
void parsePid(char buf[], pid_t pid);

/*
*   strToLong - takes a char buffer 'buf[]' and returns the content as a long
*       contant must be a number, otherwise it will break 
*/
long strToLong(char buf[]);

/*
 *	pid_getProcFileInfo - takes a destination char * 'dst', a pid_t 'pid', a char* 'filename' and the index on the information 
 *      copies the result to 'dst' (over-write)
 *	
 */
void pid_getProcFileInfo(char * dst, pid_t pid, char * fileName, int lineIndex, int wordIndex);

/*
*   proc_updatePrintInfo - takes a pNode 'node' and updates its proc info then prints relavent information
*   Depends on global variable procTimer (=0)
*/
void proc_updatePrintInfo(pNode * node);

#endif /* _PROCFXNS_H_INCLUDED_ */