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
 * implementation for generic linked list
 */

#include "linkedlist.h"
#include "tslinkedlist.h"
#include "pthread.h"
#include <stdlib.h>

pthread_mutex_t ll_lock = PTHREAD_MUTEX_INITIALIZER; 

typedef struct llnode LLNode;

LinkedList *tsll_create(void) {
   pthread_mutex_init(&ll_lock, NULL);
   return ll_create();
}

void tsll_destroy(LinkedList *ll, void (*userFunction)(void *element)) {
   ll_destroy(ll, userFunction);
}

int tsll_add(LinkedList *ll, void *element) {
   int result;
   pthread_mutex_lock(&ll_lock);
   result = ll_add(ll, element);
   pthread_mutex_unlock(&ll_lock);
   return result;
}

int tsll_insert(LinkedList *ll, long index, void *element) {
   return ll_insert(ll, index, element);
}

int tsll_addFirst(LinkedList *ll, void *element) {
   return ll_addFirst(ll, element);
}

int tsll_addLast(LinkedList *ll, void *element) {
   return ll_addLast(ll, element);
}

void tsll_clear(LinkedList *ll, void (*userFunction)(void *element)) {
   ll_clear(ll, userFunction);
}

int tsll_get(LinkedList *ll, long index, void **element) {
   return ll_get(ll, index, element);
}

int tsll_getFirst(LinkedList *ll, void **element) {
   return ll_getFirst(ll, element);
}

int tsll_getLast(LinkedList *ll, void **element) {
   return ll_getLast(ll, element);
}

int tsll_remove(LinkedList *ll, long index, void **element) {
   return ll_remove(ll, index, element);
}

int tsll_removeFirst(LinkedList *ll, void **element) {
   int result;
   pthread_mutex_lock(&ll_lock);
   result = ll_removeFirst(ll, element);
   pthread_mutex_unlock(&ll_lock);
   return result;
}

int tsll_removeLast(LinkedList *ll, void **element) {
   return ll_removeLast(ll, element);
}

int tsll_set(LinkedList *ll, long index, void *element, void **previous) {
   return ll_set(ll, index, element, previous);
}

long tsll_size(LinkedList *ll) {
   return ll_size(ll);
}

void **tsll_toArray(LinkedList *ll, long *len) {
   return ll_toArray(ll, len);
}

Iterator *tsll_it_create(LinkedList *ll) {
   return ll_it_create(ll);
}
