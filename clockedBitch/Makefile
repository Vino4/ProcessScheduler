CFLAGS=-W -Wall -g -pedantic -lpthread

fileCrawler: libADTS.a tslibADTS.a fileCrawler.o 
	gcc $(CFLAGS) -o fileCrawler fileCrawler.o iterator.o linkedlist.o treeset.o tsiterator.o tslinkedlist.o tstreeset.o re.o

tslibADTS.a: tsiterator.o tslinkedlist.o tstreeset.o

libADTS.a: iterator.o linkedlist.o treeset.o re.o

clean:
	rm -f *.o libADTS.a tslibADTS.a fileCrawler

iterator.o: iterator.c iterator.h
linkedlist.o: linkedlist.c linkedlist.h iterator.h
treeset.o: treeset.c treeset.h iterator.h
re.o: re.c re.h
tsiterator.o: tsiterator.c tsiterator.h iterator.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h tsiterator.h linkedlist.h iterator.h
tstreeset.o: tstreeset.c tstreeset.h tsiterator.h iterator.h treeset.h
fileCrawler.o: fileCrawler.c iterator.h linkedlist.h treeset.h tsiterator.h tslinkedlist.h tstreeset.h re.h