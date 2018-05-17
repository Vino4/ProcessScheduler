#include "date.h"
#include "tldlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tldnode{
	char tld[3];
	int count;
	TLDNode * leftChild;
	TLDNode * rightChild;

	int height;
	int balance;


};

struct tldlist{

	TLDNode * root;
	int count;

	Date *begin;
	Date *end;

};

struct tlditerator{

	TLDNode **array;
	int ptr;
	int size;
};

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){

	TLDList * newList = (TLDList*) malloc(sizeof(TLDList));
	if (newList != NULL){

		newList->root = NULL;

		newList->begin = begin;
		newList->end = end;

		newList->count = 0;
	}
	return newList;

}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld){
	TLDIterator * itr = tldlist_iter_create(tld);
	TLDNode * currentPtr = itr->next;
	do {
		free(currentPtr);
		currentPtr = itr->next;
	} while(currentPtr != NULL);
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
	TLDNode * node = (TLDNode*) malloc(sizeof(TLDNode));
	if(node == NULL){
		return 0;
	}
	strcpy(node->tld, hostname);
	node->count = 1;
	node->height = 1;
	node->balance = 1;

	if(date_compare(d, tld->begin) < 0 ||  date_compare(d, tld->end) > 0){
		printf("FFFFFUCK YOOOOU\n");
		return 0;
	}
	printf("insert received: %s\n", node->tld);
	tld->root = tldlist_insert(tld, tld->root, node);

	return 1;
}

TLDNode * RR_rotation(TLDNode * node) {
    TLDNode * temp = node->rightChild;  // right child of c
    node->rightChild = temp->leftChild;
    temp->leftChild = node;

    node->height = max(height(node->leftChild), height(node->rightChild)) + 1;
    temp->height = max(height(temp->leftChild), height(temp->rightChild)) + 1;
	return temp;
}

/**
 * rotate fix the left-right unbalanced case
 * @param node Represents the root of the subtree that is being balanced.
 * @return A reference to the node.
 */
TLDNode * LR_rotation(TLDNode * node) {
	node->leftChild = RR_rotation( node->leftChild );
    return LL_rotation( node );
}

/**
 * rotate fix the right-left unbalanced case
 * @param node Represents the root of the subtree that is being balanced. 
 * @return A reference to the node.
 */
TLDNode * RL_rotation(TLDNode *  node) {
	node->rightChild = LL_rotation( node->rightChild );
    return RR_rotation( node );
}

/**
 * rotate fix the left-left unbalanced case
 * @param node Represents the root of the subtree that is being balanced.
 * @return A reference to the node.
 */
TLDNode * LL_rotation(TLDNode *  node) {
    TLDNode * temp = node->leftChild;  // right child of c
    node->leftChild = temp->rightChild;
    temp->rightChild = node;

    node->height = max(height(node->leftChild), height(node->rightChild)) + 1;
    temp->height = max(height(temp->leftChild), height(temp->rightChild)) + 1;
	return temp;
}



TLDNode * balance(TLDNode * current) {
	current->height = max(height(current->leftChild),height(current->rightChild)) +1;
	current->balance = getBalance(current);
	if (current->balance == 2){
		if( getBalance(current->leftChild) == 0 || getBalance(current->leftChild) == 1){
			current = LL_rotation(current);
		} else {
			current = LR_rotation(current);
		}
	} else if (current->balance == -2){
		if( getBalance(current->rightChild) == 0 || getBalance(current->rightChild) == -1){
			current = RR_rotation(current);
		} else {
			current = RL_rotation(current);
		}
	}
	return current;	
}

TLDNode * tldlist_insert(TLDList * tld, TLDNode * current, TLDNode * node){
		if(current == NULL){ 			// Reached a null leaf node. 
			current = node;				// Make that null node the node we want to insert and then return current.  
			tld->count++;
			printf("Inserting : %s\n", current->tld);
			printf("Insert done.\n");
		} else if(strcmp( current->tld, node->tld ) < 0){	// Go down the right.
			current->rightChild = tldlist_insert(tld, current->rightChild, node);	// Important to notice I assign whatever is returned to current.right. 
		} else if(strcmp( current->tld, node->tld ) > 0){ // Go down the left.
			current->leftChild = tldlist_insert(tld, current->leftChild, node);		// Important to notice I assign whatever is returned to current.left.
		} else if(strcmp( current->tld, node->tld ) == 0){	// Return current as it is.  
			printf("Incrementing : %s\n", current->tld);
			printf("Insert done.\n");
			current->count++;
		}
		current = balance(current);
		return current;	
}

int max(int a, int b){
	if (a > b){
		return a;
	} else {
		return b;
	}
}

int height(TLDNode * node) {
	if (node == NULL){
		return 0;
	} else {
		return node->height;
	}
}

int getBalance(TLDNode * node) {
	if (node == NULL){
		return 0;
	} else {
		return (height(node->leftChild) - height(node->rightChild));
	}
}

void traversePreOrder(TLDNode * current) {
	if(current!=NULL){
		printf("%s:%d ", current->tld, current->count);
		traversePreOrder(current->leftChild);
		traversePreOrder(current->rightChild);
	}		
}

long tldlist_count(TLDList *tld){
	return tld->count;
}

void fillIterator(TLDIterator * itr, TLDNode * current) {
	if(current!=NULL){
		itr->array[itr->size++] = current;
		printf("ITR: added %s:%d \n", current->tld, current->count);
		fillIterator(itr, current->leftChild);
		fillIterator(itr, current->rightChild);
	}		
}

TLDNode *tldlist_iter_next(TLDIterator *itr){
	/*if (itr->ptr < itr->size){
		return itr[itr->ptr++];
	} else {
		return NULL
	}
	*/
	return ((itr->ptr < itr->size) ? (itr->array[itr->ptr++]) : (NULL));
}

void tldlist_iter_destroy(TLDIterator *itr){
	free(itr->array);
	free(itr);
}

TLDIterator *tldlist_iter_create(TLDList *tld){

	TLDIterator * itr = (TLDIterator*) malloc(sizeof(TLDIterator));
	if(itr == NULL){
		return itr;
	}
	itr->array = (TLDNode**) malloc(sizeof(TLDNode*) * tld->count);
	if(itr->array == NULL){
		return itr;
	}
	itr->size = 0;
	itr->ptr = 0;
	fillIterator(itr, tld->root);

	/*
	printf("1: \n");
	TLDIterator * itr = (TLDIterator*) malloc(sizeof(TLDIterator));
	printf("1.5: \n");
	itr->array = (TLDNode**) malloc(sizeof(TLDNode*) * tld->count);
	printf("2:\n");
	char * test = (char*) malloc(sizeof(char) * 20);
	printf("3:\n");

	strcpy(test, "20/8/1400");
	Date * test4 = date_create(test);

	TLDNode * newNode = (TLDNode*) malloc(sizeof(TLDNode));
	if(newNode == NULL){
		return itr;
	}
	strcpy(newNode->tld, "com");
	newNode->count = 1;
	newNode->height = 1;
	newNode->balance = 1;
	printf("node created: %s\n", newNode->tld);
	printf("4:\n");
	itr->array[0] = newNode;
	printf("node confirmed: %s\n", itr->array[0]->tld);
	*/

	return itr;

}

char *tldnode_tldname(TLDNode *node){
	return node->tld;
}

long tldnode_count(TLDNode *node){
	return node->count;
}

int main (void) {
	char * test = (char*) malloc(sizeof(char) * 20);
	strcpy(test, "1/4/1000");
	Date * test2 = date_create(test);
	strcpy(test, "4/9/2000");
	Date * test3 = date_create(test);

	TLDList * AVLBITCH = tldlist_create(test2, test3);
	strcpy(test, "20/8/1400");
	Date * test4 = date_create(test);

	strcpy(test, "com");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "uk");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "com");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "a");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "a");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "b");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "c");
	tldlist_add(AVLBITCH, test, test4);
	strcpy(test, "d");
	tldlist_add(AVLBITCH, test, test4);

	printf("TLDCount: %d\n", tldlist_count(AVLBITCH));

	TLDIterator * itr = tldlist_iter_create(AVLBITCH);
	int tracker = 1;
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	printf("ITR->next: %d. %s\n", tracker++, tldlist_iter_next(itr)->tld);
	traversePreOrder(AVLBITCH->root);

	return 0;
}