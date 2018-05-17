/*
*
*   procNode - handling the proccess structure
*       All this code in this file is compeletly written by me.
*           Author: Matt Almenshad
*/

#ifndef _PROCNODE_H_INCLUDED_
#define _PROCNODE_H_INCLUDED_

typedef struct procList pList;
typedef struct procNode pNode;

#include <signal.h>

/*
 * pNode_create creates a pNode structure from `buf[]` with a number of arguemnts argc
 * `buf' is an EOS terminated command to execute a program
 * returns pointer to pNode structure if successful,
 *         NULL if not (failed to run program)
 */
pNode *pNode_create(char buf[], int argc);

void pNode_run_NOWAIT(pNode *node);

/*
 * pNode_terminate sets the status of a node to 0 to indicate termination
 */
void pNode_terminate(pNode *node);
/*
 * pNode_setStats sets the stat_ variables of a node
 */
void pNode_setStats(pNode *node, long long stat_reads, long long stat_writes, long long stat_cpuUsrTime, long long stat_cpuKrnlTime);

/*
 * pNode_getReads returns the stat_read of a node
 */
long long pNode_getReads(pNode *node);
/*
 * pNode_getWrites returns the stat_writes of a node
 */
long long pNode_getWrites(pNode *node);

/*
 * pNode_getCPUKrnlTime returns the stat_cpuKrnlTime of a node
 */
long long pNode_getCPUKrnlTime(pNode *node);

/*
 * pNode_getCPUUsrTime returns the stat_cpuUsrTime of a node
 */
long long pNode_getCPUUsrTime(pNode *node);

/*
 * pNode_getArgc returns the "node"'s number of arguments
 */
int pNode_getArgc(pNode *node);

/*
 * pNode_getPID returns the "node"'s process ID
 *         (-1) if no process ID is asscociated with the program (program is not on)
 */
pid_t pNode_getPID(pNode *node);

/*
 * pNode_isRunning 1 if the node is not terminated, 0 otherwise
 */
int pNode_isRunning(pNode *node);

/*
 * pNode_setPID sets the "node"'s process ID
 */
void pNode_setPID(pNode *node, pid_t PID);
/*
 * pNode_getArgv returns the "node"'s arguments reference in the form of a NULL-Terminated array of strings (char **)
 *		with the format: {"Knock, knock.", "Who’s there?", "very long pause….", "Java.", NULL}
 *         NULL if no arguments available
 */
char ** pNode_getArgv(pNode *node);

/*
 * pNode_getArgv returns the "node"'s path reference in the form of a string (char *) with the format: "x/y/z/a.out"
 *         NULL if no path is available
 */
char * pNode_getPath(pNode *node);

/*
 * runs a proccess and associates the proper PID with it
 */
void pNode_run(pNode *node);

/*
 * wait for the proccess to finish
 */
void pNode_wait(pNode *node);

/*
 * starts a process by sending SIGUSR1
 */
void pNode_start(pNode *node);

/*
 * stops a process by sending SIGSTOP
 */
void pNode_stop(pNode *node);
/*
 * resume a process by sending SIGCONT
 */
void pNode_cont(pNode *node);

/*
 * pNode_destroy returns any storage associated with `node' to the system
 */
void pNode_destroy(pNode *node);

#endif /* _PROCNODE_H_INCLUDED_ */
