/*
*
*   uspsv2 - proccess scheduler
*		Usage: ./2 < workload_path
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


/* SIGUSR handler. */
// handles child starting
void sigusr_handler(int sig_num)
{
    if (sig_num == SIGUSR1){
		sig_received++;
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

	if (signal(SIGUSR1, sigusr_handler) == SIG_ERR) {
        p1perror(2, "can't catch SIGUSR1\n");
    }

	/* proc list initialization. */
	pList_insert_stdin(list); // populate the proc list with STDIN-read commands
	pNode ** tracker = pList_create_tracker(list);
	int tracker_size = pList_getQ1Size(list);

	pList_run(list); // run all procs
	p1putstr(1, "Initialized all processes\n");
	pList_start(list); // stop all procs
	p1putstr(1, "Started all processes with SIGUSR1\n");
	pList_stop(list); // stop all procs
	p1putstr(1, "Stopped all processes with SIGSTOP\n");
	pList_cont(list); // start all procs
	p1putstr(1, "Resumed all processes with SIGCONT\n");
	pList_wait(list);
	p1putstr(1, "Done\n");

	queue_destroy(tracker, tracker_size);
	pList_destroy(list);
	return 0;
}