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

all: 1 2 3 4

1: $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^

2: $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^

3: $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^

4: $(OBJECTS4)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

p1fxns.o: p1fxns.c p1fxns.h

procfxns.o: procfxns.c procfxns.h

procList.o: procList.c procList.h

procNode.o: procNode.c procNode.h

uspsv1.o: uspsv1.c

uspsv2.o: uspsv2.c

uspsv3.o: uspsv3.c

uspsv4.o: uspsv4.c

clean:
	-rm -rf $(PROGS1) *.o
	-rm -rf $(PROGS2) *.o
	-rm -rf $(PROGS3) *.o
	-rm -rf $(PROGS4) *.o
