#ifndef _TSLINKEDLIST_H_
#define _TSLINKEDLIST_H_

/*
 * Copyright (c) 2012, Court of the University of Glasgow
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Glasgow nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * interface definition for generic linked list
 *
 * patterned roughly after Java 6 LinkedList generic class, with many
 * duplicate methods removed
 */

#include "tsiterator.h"
#include "linkedlist.h"
 
/*
 * create a linked list
 *
 * returns a pointer to the linked list, or NULL if there are malloc() errors
 */
LinkedList *tsll_create(void);

/*
 * destroys the linked list; for each element, if userFunction != NULL, invokes
 * userFunction on the element; then returns any list structure associated with
 * the element; finally, deletes any remaining structures associated with the
 * list
 */
void tsll_destroy(LinkedList *ll, void (*userFunction)(void *element));

/*
 * appends `element' to the end of the list
 *
 * returns 1 if successful, 0 if unsuccesful (malloc errors)
 */
int tsll_add(LinkedList *ll, void *element);

/*
 * inserts `element' at the specified position in the list;
 * all elements from `index' upwards are shifted one position;
 * if current size is N, 0 <= index <= N must be true
 *
 * returns 1 if successful, 0 if unsuccessful (malloc errors)
 */
int tsll_insert(LinkedList *ll, long i, void *element);

/*
 * inserts `element' at the beginning of the list
 * equivalent to ll_insert(ll, 0, element);
 */
int tsll_addFirst(LinkedList *ll, void *element);

/*
 * appends `element' at the end of the list
 * equivalent to ll_add(ll, element);
 */
int tsll_addLast(LinkedList *ll, void *element);

/*
 * clears the linked list; for each element, if userFunction != NULL, invokes
 * userFunction on the element; then returns any list structure associated with
 * the element
 *
 * upon return, the list is empty
 */
void tsll_clear(LinkedList *ll, void (*userFunction)(void *element));

/*
 * Retrieves, but does not remove, the element at the specified index
 *
 * return 1 if successful, 0 if not
 */
int tsll_get(LinkedList *ll, long index, void **element);

/*
 * Retrieves, but does not remove, the first element
 *
 * return 1 if successful, 0 if not
 */
int tsll_getFirst(LinkedList *ll, void **element);

/*
 * Retrieves, but does not remove, the last element
 *
 * return 1 if successful, 0 if not
 */
int tsll_getLast(LinkedList *ll, void **element);

/*
 * Retrieves, and removes, the element at the specified index
 *
 * return 1 if successful, 0 if not
 */
int tsll_remove(LinkedList *ll, long index, void **element);

/*
 * Retrieves, and removes, the first element
 *
 * return 1 if successful, 0 if not
 */
int tsll_removeFirst(LinkedList *ll, void **element);

/*
 * Retrieves, and removes, the last element
 *
 * return 1 if successful, 0 if not
 */
int tsll_removeLast(LinkedList *ll, void **element);

/*
 * Replaces the element at the specified index; the previous element
 * is returned in `*previous'
 *
 * return 1 if successful, 0 if not
 */
int tsll_set(LinkedList *ll, long index, void *element, void **previous);

/*
 * returns the number of elements in the linked list
 */
long tsll_size(LinkedList *ll);

/*
 * returns an array containing all of the elements of the linked list in
 * proper sequence (from first to last element); returns the length of the
 * list in `len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 */
void **tsll_toArray(LinkedList *ll, long *len);

/*
 * creates an iterator for running through the linked list
 *
 * returns pointer to the Iterator or NULL
 */
Iterator *tsll_it_create(LinkedList *ll);

#endif /* _TSLINKEDLIST_H_ */
