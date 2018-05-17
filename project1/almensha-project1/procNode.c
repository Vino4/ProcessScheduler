#include "procNode.h"
#include "procList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p1fxns.h"
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

extern pList * list;
extern pNode * currentNode;
extern int sig_received, createdProcs, deadProcs;	/* global data indicating SIGWHATEVER received */



/*
*
*	procNode is a structure holding information about a process
*		argvData holds the commands of the proccess, terminated by null pointer
*		PID holds the process ID of the program
*		argc holds the number of arguments in the command
*		status holds 1 if the process is not yet terminated, 0 otherwise
*		stat_ taged variables contain proc information
*/
struct procNode
{
    char ** argvData ;
    pid_t PID;
    int argc;
    int status;

    long long stat_reads;
    long long stat_writes;
    long long stat_cpuUsrTime;
    long long stat_cpuKrnlTime;

};

/*
 * pNode_create creates a pNode structure from `buf[]` with a number of arguemnts argc
 * `buf' is an EOS terminated command to execute a program
 * returns pointer to pNode structure if successful,
 *         NULL if not (failed to run program)
 */
pNode *pNode_create(char buf[], int argc){
	char * ptr = buf;
	pNode * newNode = (pNode*) malloc(sizeof(pNode));
	if (newNode == NULL){
		p1perror(1, "pNode_create: failed to malloc new node");
		return;
	}
	int i, index;
	for(i = 0; i < argc; i++){
		index = p1strchr(ptr, ' ');
		ptr += index;
		*ptr = '\0';
		index++;
		ptr++;
	}

	ptr = buf; //reset pointer
	argc++; //to include the program path as the 0th argument
	newNode->argvData = (char**) calloc(argc +1, sizeof(char*));
	if (newNode->argvData == NULL){
		p1perror(1, "pNode_create: failed to malloc argvData");
		return NULL;
	}

	newNode->argvData[0] = malloc(strlen(ptr) + 1); 
	if (newNode->argvData[0] == NULL){
		p1perror(1, "pNode_create: failed to malloc argvData");
		return NULL;
	}

	p1strcpy(newNode->argvData[0], ptr); 

	for(i = 1; i < argc; i++){
		index = strlen(ptr) + 1;
		ptr += index;
		newNode->argvData[i] = malloc(strlen(ptr) + 1); 
		if (newNode->argvData[i] == NULL){
			p1perror(1, "pNode_create: failed to malloc argvData");
			return NULL;
		}
		p1strcpy(newNode->argvData[i], ptr); 
		index++;
		ptr++;
	}

	newNode->argvData[argc] = NULL;
	newNode->PID = -1;
	newNode->argc = argc;
	newNode->status = 1;

    newNode->stat_reads = 0;
    newNode->stat_writes = 0;
    newNode->stat_cpuUsrTime = 0;
    newNode->stat_cpuKrnlTime = 0;

};

/*
 * pNode_terminate sets the status of a node to 0 to indicate termination
 */
void pNode_terminate(pNode *node){
	node->status = 0;
}

/*
 * pNode_setStats sets the stat_ variables of a node
 */
void pNode_setStats(pNode *node, long long stat_reads, long long stat_writes, long long stat_cpuUsrTime, long long stat_cpuKrnlTime){
	node->stat_reads = stat_reads;
	node->stat_writes = stat_writes;
	node->stat_cpuUsrTime = stat_cpuUsrTime;
	node->stat_cpuKrnlTime = stat_cpuKrnlTime;
}

/*
 * pNode_getReads returns the stat_read of a node
 */
long long pNode_getReads(pNode *node){
	return node->stat_reads;
}

/*
 * pNode_getWrites returns the stat_writes of a node
 */
long long pNode_getWrites(pNode *node){
	return node->stat_writes;
}

/*
 * pNode_getCPUKrnlTime returns the stat_cpuKrnlTime of a node
 */
long long pNode_getCPUKrnlTime(pNode *node){
	return node->stat_cpuKrnlTime;
}

/*
 * pNode_getCPUUsrTime returns the stat_cpuUsrTime of a node
 */
long long pNode_getCPUUsrTime(pNode *node){
	return node->stat_cpuUsrTime;
}

/*
 * pNode_getArgc returns the "node"'s number of arguments
 */
int pNode_getArgc(pNode *node){
	return node->argc;
}


/*
 * pNode_getPID returns the "node"'s process ID
 *         (-1) if no process ID is asscociated with the program (program is not on)
 */
pid_t pNode_getPID(pNode *node){
	return node->PID;
};

/*
 * pNode_isRunning 1 if the node is not terminated, 0 otherwise
 */
int pNode_isRunning(pNode *node){
	if (node != NULL)
		return node->status;
};


/*
 * pNode_setPID sets the "node"'s process ID
 */
void pNode_setPID(pNode *node, pid_t PID){
	node->PID = PID;
};

/*
 * pNode_getArgv returns the "node"'s arguments reference in the form of a NULL-Terminated array of strings (char **)
 *		with the format: {"Knock, knock.", "Whoâ€™s there?", "very long pauseâ€¦.", "Java.", NULL}
 *         NULL if no arguments available
 */
char ** pNode_getArgv(pNode *node){
	return node->argvData;
};

/*
 * pNode_getArgv returns the "node"'s path reference in the form of a string (char *) with the format: "x/y/z/a.out"
 *         NULL if no path is available
 */
char * pNode_getPath(pNode *node){
	return node->argvData[0];
};

/*
 * runs a proccess and associates the proper PID with it
 */
void pNode_run(pNode *node){

	pid_t pid = fork();
	if (pid == -1){
	    printf("Error: Fork failed.\n");
	} else if (pid == 0){
		while(!sig_received){
			sleep(1);
		}
		int errX = execvp(pNode_getArgv(node)[0], pNode_getArgv(node));
		if (errX < 0)
			p1perror(1, "pNode_run: failed to execvp");
		_exit(EXIT_FAILURE);   // done executing proccess, child exit
	}
    pNode_setPID(node, pid);
};

/*
 * wait for the proccess to finish
 */
void pNode_wait(pNode *node){
	pid_t pid = pNode_getPID(node);

    int status;
    waitpid(pid, &status, 0);
};

/*
 * starts a process by sending SIGUSR1
 */
void pNode_start(pNode *node){
	pid_t pid = pNode_getPID(node);

    kill(pid, SIGUSR1);
};

/*
 * stops a process by sending SIGSTOP
 */
void pNode_stop(pNode *node){
	pid_t pid = pNode_getPID(node);

    kill(pid, SIGSTOP);
};

/*
 * resume a process by sending SIGCONT
 */
void pNode_cont(pNode *node){
	pid_t pid = pNode_getPID(node);

    kill(pid, SIGCONT);
};

/*
 * pNode_destroy returns any storage associated with `node' to the system
 */
void pNode_destroy(pNode *node){
	if (node != NULL){
		int i, argc = pNode_getArgc(node);
		for(i = 0; i < argc; i++){
			free(node->argvData[i]);

		}
		free(node->argvData);
		free(node);
		node = NULL;
	}

};