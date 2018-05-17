/*
 *	iotest - test program that is IO-intensive
 *
 *	usage: ./iotest [-b <bufsize>] [-n <nwrites>]
 *
 *	FILE *fd = fopen("/dev/null", "w");
 *	while (n-- > 0)
 *  	    fwrite(fd, buf, bufsize);
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define UNUSED __attribute__ ((unused))
#define DEFAULT_NWRITES 1000000000
#define DEFAULT_BUFSIZE 1024
#define USAGE "./iotest [-b <bufsize>] [-n <nwrites>]"

pid_t pid;

void CONT_handler(UNUSED int signo) {
    time_t t;

    (void)time(&t);
    //fprintf(stderr, "%ld: continued at %s", (long) pid, ctime(&t));
}

int main(int argc, char *argv[]) {
    FILE *fd;
    char buf[4096];
    int nwrites = DEFAULT_NWRITES;
    int bufsize = DEFAULT_BUFSIZE;
    int i, j;

    pid = getpid();
    for (i = 1; i < argc; ) {
        if ((j = i + 1) == argc) {
            fprintf(stderr, "usage %s\n", USAGE);
            exit(1);
        }
        if (strcmp(argv[i], "-b") == 0) {
            bufsize = atoi(argv[j]);
            if (bufsize < 1)
                bufsize = 1;
            else if (bufsize > DEFAULT_BUFSIZE)
                bufsize = DEFAULT_BUFSIZE;
        } else if (strcmp(argv[i], "-n") == 0) {
            nwrites = atoi(argv[j]);
            if (nwrites < 1)
                nwrites = 1;
        } else {
            fprintf(stderr,"Unknown flag: %s %s\n", argv[i], argv[j]);
        }
        i = j + 1;
    }
    signal(SIGCONT, CONT_handler);
    fd = fopen("/dev/null", "w");
    while(nwrites-- > 0)
        (void)fwrite(buf, 1, bufsize, fd);
    fclose(fd);
    return 0;
}
