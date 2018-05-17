#ifndef _PROCLIST_H_INCLUDED_
#define _PROCLIST_H_INCLUDED_

typedef struct procList pList;
typedef struct procNode pNode;

pNode ** pList_create_tracker(pList * list);

/*
 * pList_create returns a new pList
 *         NULL if not (failed to allocate)
 */
pList *pList_create(void);

/*
 * pList_insert_stdin fills the list with pNodes created from parsed stdin
 */
void pList_insert_stdin(pList *list);
/*
 * pList_insert_node inserts a node 'node' to the list 'list'
 */
void pList_insert_node(pList * list, pNode * node);

/*
 * pList_reinsert_node reinserts a node 'node' to the list 'list'
 */
void pList_reinsert_node(pList * list, pNode * node);

/*
 * pList_extractNode extracts the first node in the list 
 *         NULL if not (failed to allocate)
 */
pNode * pList_extractNode(pList * list);


/*
 * pList_run runs every program in a pList 'list' using SIGUSR1
 */
void pList_run(pList *list);
/*
 * pList_run runs every program in a pList 'list' then stops it using SIGUSR1 then SIGSTOP
 */
void pList_runAndStop(pList *list);

/*
 * pList_wait waits for every program in a pList 'list' using waitpid()
 */
void pList_wait(pList *list);

/*
 * pList_start starts every program in a pList 'list' using SIGUSR1
 */
void pList_start(pList *list);
/*
 * pList_stop stops every program in a pList 'list' using SIGSTOP
 */
void pList_stop(pList *list);

/*
 * pList_cont resumes every program in a pList 'list' using SIGCONT
 */
void pList_cont(pList *list);

#endif /* _PROCLIST_H_INCLUDED_ */