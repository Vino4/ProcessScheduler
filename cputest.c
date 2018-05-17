/*
 *	cputest - test program that is CPU-intensive
 *
 *	usage: ./cputest [-b <burstsize>] [-n <ncycles>]
 *
 *	alternates 'ncycles' of CPU burst|sleep
 *	'burstsize' is the number of microseconds for each burst and sleep
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define UNUSED __attribute__ ((unused))
#define DEFAULT_BURSTSIZE 1000
#define DEFAULT_NCYCLES 10000
#define USAGE "./cputest [-b <burstsize>] [-n <ncycles>]"

pid_t pid;

void CONT_handler(UNUSED int signo) {
    time_t t;

    (void)time(&t);
    //fprintf(stderr, "%ld: continued at %s", (long int)pid, ctime(&t));
}

void crunch(int usecs) {
    int i;

    while (usecs-- > 0) {
        for (i = 0; i < 1000; i++)
            ;
    }
}

int main(int argc, char *argv[]) {
    int ncycles = DEFAULT_NCYCLES;
    int burstsize = DEFAULT_BURSTSIZE;
    int i, j;

    pid = getpid();
    for (i = 1; i < argc; ) {
        if ((j = i + 1) == argc) {
            fprintf(stderr, "usage %s\n", USAGE);
            exit(1);
        }
        if (strcmp(argv[i], "-b") == 0) {
            burstsize = atoi(argv[j]);
            if (burstsize < 1000)
                burstsize = 1000;
        } else if (strcmp(argv[i], "-n") == 0) {
            ncycles = atoi(argv[j]);
            if (ncycles < 1)
                ncycles = 1;
        } else {
            fprintf(stderr,"Unknown flag: %s %s\n", argv[i], argv[j]);
        }
        i = j + 1;
    }
    signal(SIGCONT, CONT_handler);
    while (ncycles-- > 0) {
        usleep(burstsize);
        crunch(burstsize);
    }
    return 0;
}
