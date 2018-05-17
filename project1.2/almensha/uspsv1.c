/*
*
*   uspsv1 - proccess scheduler
*		Usage: ./1 < workload_path
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
#include <sys/wait.h>

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
////	SCHEDULER
//////////////////////////////////////
//////////////////////////////////////

int main(UNUSED int argc, UNUSED char *argv[]){

	list = pList_create(); // create a list to hold procs

	/* proc list initialization. */
	pList_insert_stdin(list); // populate the proc list with STDIN-read commands

	pNode ** tracker = pList_create_tracker(list);
	int tracker_size = pList_getQ1Size(list);

	pList_run_NOWAIT(list); // run all procs
	p1putstr(1, "Started all forked processes\n");
	pList_wait(list);
	p1putstr(1, "Done\n");

	queue_destroy(tracker, tracker_size);
	pList_destroy(list);
	return 0;
}