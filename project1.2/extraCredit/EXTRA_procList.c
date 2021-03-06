#include "EXTRA_procList.h"
#include "EXTRA_procNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EXTRA_p1fxns.h"
#include <unistd.h>
#include <signal.h>

#define DEFAULT_NUMBER_OF_PROCS 200

extern pList * list;
extern pNode * currentNode;
extern int sig_received, createdProcs, deadProcs;	/* global data indicating SIGWHATEVER received */

/*
* a list of processes 
* Based on two FIFO queues
*/
struct procList
{
    pNode ** queue1;
    pNode ** queue2;
    int Q1_size;
    int Q2_size;
    int index;
    int max_size, size, currentQ;

};

/*
 * pList_create returns a new pList
 *         NULL if not (failed to allocate)
 */
pList *pList_create(void){
	pList * newList = (pList*) malloc(sizeof(pList));
	if (newList == NULL)
		return NULL;
	newList->queue1 = (pNode**) malloc(sizeof(pNode*) * DEFAULT_NUMBER_OF_PROCS);
	if (newList->queue1 == NULL)
		return NULL;
	newList->queue2 = (pNode**) malloc(sizeof(pNode*) * DEFAULT_NUMBER_OF_PROCS);
	if (newList->queue2 == NULL)
		return NULL;
    newList->Q1_size = 0;
    newList->Q2_size = 0;
    newList->size = 0;
    newList->currentQ = 1;
    newList->index = 0;
    newList->max_size = DEFAULT_NUMBER_OF_PROCS;
	return newList;
};

/*
 * pList_create_tracker returns a new pList
 *         NULL if not (failed to allocate)
 */
pNode ** pList_create_tracker(pList * list){
	pNode ** tracker = (pNode**) malloc(sizeof(pNode*) * list->Q1_size);
	int i = 0;
	for(i = 0; i < list->Q1_size; i++){
		tracker[i] = list->queue1[i];
	}
	return tracker;
};

void queue_destroy(pNode ** tracker, int size){
	int i = 0;
	for(i = 0; i < size; i++){
		pNode_destroy(tracker[i]);
	}
	free(tracker);

}


/*
 * pList_insert_stdin fills the list with pNodes created from parsed stdin
 */
void pList_insert_stdin(pList *list){
	char buf[1024];
	char outputBuf[1024];
	int len, argc = 0, wIdx = 0;
	while((len = p1getline(0, buf, 1024)) > 0){
		if (buf[len-1] == '\n')
			buf[len-1] = '\0';
		while((wIdx = p1getword(buf, wIdx, outputBuf)) > -1){
			argc++;
		}
		pList_insert_node(list, pNode_create(buf, argc));
		argc = 0;
		wIdx = 0;
	}
}

/*
 * pList_insert_stdin fills the list with pNodes created from parsed stdin
 */
int pList_getQ1Size(pList *list){
	return list->Q1_size;
}

/*
 * pList_insert_node inserts a node 'node' to the list 'list'
 */
void pList_insert_node(pList * list, pNode * node){
	if (node != NULL){
		if (list->size >= list->max_size){
			list->max_size += 10;
			list->queue1 = (pNode**) realloc(list->queue1, list->max_size);
				if (list->queue1 == NULL){
					p1perror(1, "pList_insert_node: failed to realloc queue1");
					return;
				}
			list->queue2 = (pNode**) realloc(list->queue2, list->max_size);
				if (list->queue2 == NULL){
					p1perror(1, "pList_insert_node: failed to realloc queue2");
					return;
				}
		}
		pNode ** qPtr = ((list->currentQ == 1) ? (list->queue1) : (list->queue2));
		qPtr[list->Q1_size++] = node;
		list->size++;
		createdProcs++;
	} else {
		p1perror(1, "Insert");
	}
};

/*
 * pList_reinsert_node reinserts a node 'node' to the list 'list'
 */
void pList_reinsert_node(pList * list, pNode * node){
	if (node != NULL){
		if (list->size >= list->max_size){
			list->max_size += 10;
			list->queue1 = (pNode**) realloc(list->queue1, list->max_size);
				if (list->queue1 == NULL){
					p1perror(1, "pList_reinsert_node: failed to realloc queue1");
					return;
				}
			list->queue2 = (pNode**) realloc(list->queue2, list->max_size);
				if (list->queue2 == NULL){
					p1perror(1, "pList_reinsert_node: failed to realloc queue2");
					return;
				}
		}
		pNode ** qPtr = ((list->currentQ == 1) ? (list->queue2) : (list->queue1));

		if (list->currentQ == 1){
			qPtr[list->Q2_size++] = node;
		} else {
			qPtr[list->Q1_size++] = node;
		}
	} else {
		p1perror(1, "reinsert");
	}
};

/*
 * pList_extractNode extracts the first node in the list 
 *         NULL if not (failed to allocate)
 */
pNode * pList_extractNode(pList * list){
	int * currentSize = ((list->currentQ == 1) ? (&list->Q1_size) : (&list->Q2_size));
	if (list->index == (*currentSize)){
		(list->currentQ == 1) ? (list->Q1_size = 0) : (list->Q2_size = 0);
		list->currentQ *= -1;
		list->index = 0;
		*currentSize = 0;
	}
	pNode ** qPtr = ((list->currentQ == 1) ? (list->queue1) : (list->queue2));
	pNode * extractedNode = qPtr[list->index];
	qPtr[list->index++] = NULL;
	return extractedNode;
};


/*
 * pList_run runs every program in a pList 'list' using SIGUSR1
 */
void pList_run(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_run(list->queue1[i]);
	}
}

/*
 * pList_run runs every program in a pList 'list' using SIGUSR1
 */
void pList_run_NOWAIT(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_run_NOWAIT(list->queue1[i]);
	}
}

/*
 * pList_run runs every program in a pList 'list' then stops it using SIGUSR1 then SIGSTOP
 */
void pList_runAndStop(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_run(list->queue1[i]);
		pNode_stop(list->queue1[i]);
	}
}


/*
 * pList_wait waits for every program in a pList 'list' using waitpid()
 */
void pList_wait(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_wait(list->queue1[i]);
	}
}

/*
 * pList_start starts every program in a pList 'list' using SIGUSR1
 */
void pList_start(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_start(list->queue1[i]);
	}
}

/*
 * pList_stop stops every program in a pList 'list' using SIGSTOP
 */
void pList_stop(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_stop(list->queue1[i]);
	}
}

/*
 * pList_cont resumes every program in a pList 'list' using SIGCONT
 */
void pList_cont(pList *list){
	int i;
	for (i = 0; i < list->size; i++){
		pNode_cont(list->queue1[i]);
	}
}

/*
 * pNode_destroy returns any storage associated with `node' to the system
 */
void pList_destroy(pList *list){
	if (list != NULL){
		free(list->queue1);
		free(list->queue2);
		free(list);
	}

};