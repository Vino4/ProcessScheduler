/*
*
*   uspsv3 - proccess scheduler
*		Usage: ./3 < workload_path
*       All this code in this file is compeletly written by me.
*			Except for professor Sventek's child handler
*           Author: Matt Almenshad
*/

#include "procList.h"
#include "procNode.h"
#include "procfxns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p1fxns.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define UNUSED __attribute__ ((unused))

struct procList
{
    pNode ** queue1;
    pNode ** queue2;
    int Q1_size;
    int Q2_size;
    int index;
    int max_size, size, currentQ;

};

//////////////////////////////////////
//////////////////////////////////////
////	GLOBAL VARIABLES
//////////////////////////////////////
//////////////////////////////////////

pList * list;
pNode * currentNode;
int sig_received , createdProcs, deadProcs;

typedef struct procList pList;
typedef struct procNode pNode;


//////////////////////////////////////
//////////////////////////////////////
////	SIGNAL HANDLERS
//////////////////////////////////////
//////////////////////////////////////


/* SIGCHLD handler. */
// handles child termination
static void sigchld_handler (UNUSED int sig) {
	pid_t pid;
	int status;
	/* Wait for all dead processes.
	 * We use a non-blocking call to be sure this signal handler will not
	 * block if a child was cleaned up in another part of the program. */
	 // -1 - Wait for any child
	 // WNOHANG - Return immediately if no child has exited.
	 // WIFEXITED - returns non zero value if child terminated normally
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			p1putstr(1, "Terminated: ");
			p1putint(1, pid);
			p1putstr(1, "\n");
			deadProcs++;
			if (pNode_getPID(currentNode) == pid)
				pNode_terminate(currentNode);
		}
	}
}

/* SIGUSR handler. */
// handles child starting
void sigusr_handler(int sig_num)
{
    if (sig_num == SIGUSR1){
		sig_received++;
    }

}
/* SIGALRM handler. */
// handles scheduling events
void sigalrm_handler(int sig_num)
{

    if (sig_num == SIGALRM){
    	if (pNode_isRunning(currentNode)){
			pNode_stop(currentNode);
			p1putstr(1, "Stopped: ");
			p1putint(1, pNode_getPID(currentNode));
			p1putstr(1, "\n");
			if (pNode_isRunning(currentNode))
				pList_reinsert_node(list, currentNode);
			currentNode = pList_extractNode(list);
			pNode_cont(currentNode);
			p1putstr(1, "Resumed: ");
			p1putint(1, pNode_getPID(currentNode));
			p1putstr(1, "\n");
		} else {
			currentNode = pList_extractNode(list);

		}
			if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
		        p1putstr(1, "can't catch SIGALRM\n");
		    }
			alarm(1);
    }

}

//////////////////////////////////////
//////////////////////////////////////
////	SCHEDULER
//////////////////////////////////////
//////////////////////////////////////

int main(UNUSED int argc, UNUSED char *argv[]){

	/* Initialize Global Variables. */
	sig_received = 0;
	createdProcs = 0;
	deadProcs = 0;
	list = pList_create(); // create a list to hold procs
	currentNode = NULL;

	/* signal handling. */
	struct sigaction act;
	memset (&act, 0, sizeof(act));
	act.sa_handler = sigchld_handler;
 
	if (sigaction(SIGCHLD, &act, 0)) {
		perror ("sigaction");
		return 1;
	}

	if (signal(SIGUSR1, sigusr_handler) == SIG_ERR) {
        p1perror(2, "can't catch SIGUSR1\n");
    }
    if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        p1perror(2, "can't catch SIGALRM\n");
    }

	/* proc list initialization. */
	pList_insert_stdin(list); // populate the proc list with STDIN-read commands
	pNode ** tracker = pList_create_tracker(list);
	int tracker_size = pList_getQ1Size(list);
	pList_runAndStop(list); // run all procs
	p1putstr(1, "Initialized and stopped all procs with SIGUSR1 and SIGSTOP...\n");
	pList_start(list); // run all procs
	int i;
	for(i = 2; i > 0; i--){
		sleep(1);
	}
	pList_stop(list);// stop all procs
	alarm(1); // generate the first alarm 

	/* while children are still running, sleep. */
	while (deadProcs < createdProcs) {
		sleep(1);
	}

	p1putstr(1, "Done!\n");
	queue_destroy(tracker, tracker_size);
	pList_destroy(list);
	return 0;
}