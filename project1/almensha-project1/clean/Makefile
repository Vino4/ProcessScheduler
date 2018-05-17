CC = gcc
CFLAGS = -W -Wall
SOURCE = child_handler.c p1fxns.c
OBJECTS = ${SOURCE:.c=.o}
PROGS = ch_hndl

ch_hndl: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

child_handler.o: child_handler.c p1fxns.h

p1fxns.o: p1fxns.c p1fxns.h

clean:
	rm $(PROGS) *.o
