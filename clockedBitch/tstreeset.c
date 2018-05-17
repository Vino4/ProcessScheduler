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

#include "tstreeset.h"
#include <stdlib.h>
#include "pthread.h"

pthread_mutex_t ts_lock = PTHREAD_MUTEX_INITIALIZER; 

TreeSet *tsts_create(int (*cmpFunction)(void *, void *)) {
   return ts_create(cmpFunction);
}

void tsts_destroy(TreeSet *ts, void (*userFunction)(void *element)) {
   ts_destroy(ts, userFunction);
}

int tsts_add(TreeSet *ts, void *element) {
   int result;
   pthread_mutex_lock(&ts_lock);
   result = ts_add(ts, element);
   pthread_mutex_unlock(&ts_lock);
   return result;
}

int tsts_ceiling(TreeSet *ts, void *element, void **ceiling) {
   return ts_ceiling(ts, element, ceiling);
}

void tsts_clear(TreeSet *ts, void (*userFunction)(void *element)) {
   ts_clear(ts, userFunction);
}

int tsts_contains(TreeSet *ts, void *element) {
   return ts_contains(ts, element);
}

int tsts_first(TreeSet *ts, void **element) {
   return ts_first(ts, element);
}

int tsts_floor(TreeSet *ts, void *element, void **floor) {
   return ts_floor(ts, element, floor);
}

int tsts_higher(TreeSet *ts, void *element, void **higher) {
   return ts_higher(ts, element, higher);
}

int tsts_isEmpty(TreeSet *ts) {
   return ts_isEmpty(ts);
}


int tsts_last(TreeSet *ts, void **element) {
   return ts_last(ts, element);
}

int tsts_lower(TreeSet *ts, void *element, void **lower) {
   return ts_lower(ts, element, lower);
}

int tsts_pollFirst(TreeSet *ts, void **element) {
   return ts_pollFirst(ts, element);
}

int tsts_pollLast(TreeSet *ts, void **element) {
   return ts_pollLast(ts, element);
}

int tsts_remove(TreeSet *ts, void *element, void (*userFunction)(void *element)) {
   return ts_remove(ts, element, userFunction);
}

long tsts_size(TreeSet *ts) {
   return ts_size(ts);
}

void **tsts_toArray(TreeSet *ts, long *len) {
   return ts_toArray(ts, len);
}

Iterator *tsts_it_create(TreeSet *ts) {
   return ts_it_create(ts);
}
