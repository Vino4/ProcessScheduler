CC = gcc
CFLAGS = -W -Wall -g
SOURCE1 = p1fxns.c procfxns.c procList.c procNode.c uspsv1.c
OBJECTS1 = ${SOURCE1:.c=.o}
PROGS1 = 1

SOURCE2 = p1fxns.c procfxns.c procList.c procNode.c uspsv2.c
OBJECTS2 = ${SOURCE2:.c=.o}
PROGS2 = 2

SOURCE3 = p1fxns.c procfxns.c procList.c procNode.c uspsv3.c
OBJECTS3 = ${SOURCE3:.c=.o}
PROGS3 = 3

SOURCE4 = p1fxns.c procfxns.c procList.c procNode.c uspsv4.c
OBJECTS4 = ${SOURCE4:.c=.o}
PROGS4 = 4

SOURCE5 = EXTRA_p1fxns.c EXTRA_procfxns.c EXTRA_procList.c EXTRA_procNode.c uspsv5.c
OBJECTS5 = ${SOURCE5:.c=.o}
PROGS5 = 5

all: 1 2 3 4 5

1: $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^

2: $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^

3: $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^

4: $(OBJECTS4)
	$(CC) $(CFLAGS) -o $@ $^

5: $(OBJECTS5)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

p1fxns.o: p1fxns.c p1fxns.h

procfxns.o: procfxns.c procfxns.h

procList.o: procList.c procList.h

procNode.o: procNode.c procNode.h

EXTRA_p1fxns.o: EXTRA_p1fxns.c EXTRA_p1fxns.h

EXTRA_procfxns.o: EXTRA_procfxns.c EXTRA_procfxns.h

EXTRA_procList.o: EXTRA_procList.c EXTRA_procList.h

EXTRA_procNode.o: EXTRA_procNode.c EXTRA_procNode.h

uspsv1.o: uspsv1.c

uspsv2.o: uspsv2.c

uspsv3.o: uspsv3.c

uspsv4.o: uspsv4.c

uspsv5.o: uspsv5.c

clean:
	-rm -rf $(PROGS1) *.o
	-rm -rf $(PROGS2) *.o
	-rm -rf $(PROGS3) *.o
	-rm -rf $(PROGS4) *.o
	-rm -rf $(PROGS5) *.o
