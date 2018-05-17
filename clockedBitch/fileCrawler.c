#define _DEFAULT_SOURCE 1	/* enables macros to test type of directory entry */

#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tslinkedlist.h"
#include "tstreeset.h"
#include "re.h"
#include "pthread.h"
#include <libgen.h>

#ifndef CRAWLER_THREADS
#define CRAWLER_THREADS 300
#endif


#define UNUSED __attribute__ ((unused))

pthread_mutex_t workerLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t workerCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mainLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mainCond = PTHREAD_COND_INITIALIZER;

LinkedList *ll = NULL;
TreeSet *ts = NULL;
char *sp;
RegExp *reg;
char pattern[4096];
Iterator *it;

int mainThreadDone;
int zThreads;

int cArgs;
char * vArgs;

pthread_t harvestThread;
pthread_t workerThreads[CRAWLER_THREADS];

/*
* routine to convert bash pattern to regex pattern
* 
* e.g. if bashpat is "*.c", pattern generated is "^.*\.c$"
*      if bashpat is "a.*", pattern generated is "^a\..*$"
*
* i.e. '*' is converted to ".*"
*      '.' is converted to "\."
*      '?' is converted to "."
*      '^' is put at the beginning of the regex pattern
*      '$' is put at the end of the regex pattern
*
* assumes 'pattern' is large enough to hold the regular expression
*/
static void cvtPattern(char pattern[], const char *bashpat) {
  char *p = pattern;

  *p++ = '^';
  while (*bashpat != '\0') {
      switch (*bashpat) {
          case '*':
          *p++ = '.';
          *p++ = '*';
          break;
          case '.':
          *p++ = '\\';
          *p++ = '.';
          break;
          case '?':
          *p++ = '.';
          break;
          default:
          *p++ = *bashpat;
      }
      bashpat++;
  }
  *p++ = '$';
  *p = '\0';
}

/*
* recursively opens directory files
*
* if the directory is successfully opened, it is added to the linked list
*
* for each entry in the directory, if it is itself a directory,
* processDirectory is recursively invoked on the fully qualified name
*
* if there is an error opening the directory, an error message is
* printed on stderr, and 1 is returned, as this is most likely indicative
* of a protection violation
*
* if there is an error duplicating the directory name, or adding it to
* the linked list, an error message is printed on stderr and 0 is returned
*
* if no problems, returns 1
*/

static int processDirectory(char *dirname, LinkedList *ll, int verbose) {
    DIR *dd;
    char *sp;
    int len, status = 1;
    char d[4096];


    /*
    * eliminate trailing slash, if there
    */
    strcpy(d, dirname);
    len = strlen(d);
    if (len > 1 && d[len-1] == '/')
        d[len-1] = '\0';
    /*
    * open the directory
    */
    if ((dd = opendir(d)) == NULL) {
        if (verbose)
            fprintf(stderr, "Error opening directory '%s'\n", d);
        return 1;
    }

    /*
    * duplicate directory name to insert into linked list
    */
    sp = strdup(d);
    if (sp == NULL) {
        fprintf(stderr, "Error adding `%s' to linked list\n", d);
        status = 0;
        goto cleanup;
    }
    if (!tsll_add(ll, sp)) {
        fprintf(stderr, "Error adding `%s' to linked list\n", sp);
        free(sp);
        status = 0;
        goto cleanup;
    } else {
        pthread_cond_signal(&workerCond);
    }
    cleanup:
        (void) closedir(dd);
        return status;
}

/*
* comparison function between strings
*
* need this shim function to match the signature in treeset.h
*/
static int scmp(void *a, void *b) {
    return strcmp((char *)a, (char *)b);
}

/*
* applies regular expression pattern to contents of the directory
*
* for entries that match, the fully qualified pathname is inserted into
* the treeset
*/
static int applyRe(char *dir, RegExp *reg, TreeSet *ts) {
    char * newSP;
    char * newDir;
    int status = 1;
    newDir = strdup(dir); 

    if (! re_match(reg, basename(newDir)))
        return 0;
    if ((newSP = strdup(dir)) != NULL) {
        if (!tsts_add(ts, newSP)) {
            fprintf(stderr, "Error adding `%s' to tree set\n", newSP);
            free(newSP);
            status = 0;
            return 0;
        }
    } else {
        fprintf(stderr, "Error adding `%s' to tree set\n", newSP);
        status = 0;
        return 0;
    }

    return status;
}

void * htFunction(UNUSED void * blah){
    return NULL;
}

void * workerFunction(UNUSED void * blah){
    char * sp;
    DIR *dd;

    /*
    * for each directory in the linked list, apply regular expression
    */
    while (((!mainThreadDone) || (zThreads < CRAWLER_THREADS))) {
        int rfStat;
        struct dirent *dent;
        int status = 1;
        pthread_mutex_lock(&workerLock);
        while(!tsll_size(ll)){
            zThreads++;
            if (((mainThreadDone) && ((zThreads) == CRAWLER_THREADS))){
                fprintf(stderr, "THREAD EXITED\n");
                pthread_cond_broadcast(&workerCond);
                pthread_mutex_unlock(&workerLock);
                return NULL;
            }
            pthread_cond_wait(&workerCond, &workerLock);
            if (((mainThreadDone) && ((zThreads) == CRAWLER_THREADS))){
                fprintf(stderr, "THREAD EXITED\n");
                pthread_mutex_unlock(&workerLock);
                return NULL;
            } else {
                fprintf(stderr, "NOPE %d %d\n", mainThreadDone, zThreads);
            }
            zThreads--;
        }
        rfStat = tsll_removeFirst(ll, (void **)&sp);
        if (rfStat){ 
            if ((dd = opendir(sp)) == NULL) {
                fprintf(stderr, "Error opening directory '%s'\n", sp);
                return NULL;
            } else {
                while (status && (dent = readdir(dd)) != NULL) {
                    char b[4096];
                    if (strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0)
                        continue;
                    sprintf(b, "%s/%s", sp, dent->d_name);
                    if (dent->d_type & DT_DIR) {
                        fprintf(stderr, "Adding DIR %s\n",b);
                        processDirectory(b, ll, 1);
                    } else {
                        fprintf(stderr, "Adding FILE %s\n",b);
                        applyRe(b, reg, ts);
                    }
                }
                (void) closedir(dd);
            }
            pthread_mutex_unlock(&workerLock); 
        }
    }  
    return NULL;
}

int main(int argc, char *argv[]) {

    int i;
    mainThreadDone = 0;
    zThreads = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./fileCrawler pattern [dir] ...\n");
        return -1;
    }

    /*
    * convert bash expression to regular expression and compile
    */
    cvtPattern(pattern, argv[1]);
    if ((reg = re_create()) == NULL) {
        fprintf(stderr, "Error creating Regular Expression Instance\n");
        return -1;
    }
    if (! re_compile(reg, pattern)) {
        char eb[4096];
        re_status(reg, eb, sizeof eb);
        fprintf(stderr, "Compile error - pattern: `%s', error message: `%s'\n",
        pattern, eb);
        re_destroy(reg);
        return -1;
    }
    /*
    * create linked list and treeset
    */
    if ((ll = tsll_create()) == NULL) {
        fprintf(stderr, "Unable to create linked list\n");
        goto done;
    }
    if ((ts = tsts_create(scmp)) == NULL) {
        fprintf(stderr, "Unable to create tree set\n");
        goto done;
    }

    for (i = 0; i < CRAWLER_THREADS; i++){
        pthread_create(&workerThreads[i], NULL, workerFunction, NULL);
    }

    /*
    * populate linked list
    */
    if (argc == 2) {
        if (! processDirectory(".", ll, 1))
            goto done;
    } else {
        int i;
        for (i = 2; i < argc; i++) {
            if (! processDirectory(argv[i], ll, 1))
                goto done;
        }
    }

    mainThreadDone++;

    for (i = 0; i < CRAWLER_THREADS; i++){
        pthread_join(workerThreads[i], NULL);
    }  

    /*
    * create iterator to traverse files matching pattern in sorted order
    */
    if ((it = ts_it_create(ts)) == NULL) {
        fprintf(stderr, "Unable to create iterator over tree set\n");
        goto done;
    }
    while (it_hasNext(it)) {
        char *s;
        (void) it_next(it, (void **)&s);
        printf("%s\n", s);
    }
    it_destroy(it);

/*
* cleanup after ourselves so there are no memory leaks
*/
done:
    if (ll != NULL)
        ll_destroy(ll, free);
    if (ts != NULL)
        ts_destroy(ts, free);
    re_destroy(reg);
    return 0;
}
