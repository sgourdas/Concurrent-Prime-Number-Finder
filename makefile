HEADER  = Functions.h Types.h
SOURCE	= myprime.c innerLeafs.c work.c util.c List.c
OBJS 	= myprime.o innerLeafs.o work.o util.o List.o
EXECS   = myprime innerLeafs workers
OUT  	= myprime
CC	    = gcc
CFLAGS  = -g -Wall -Werror


make: myprime.o innerLeafs.o work.o util.o List.o
	$(CC) $(CFLAGS) myprime.o util.o List.o -o myprime
	$(CC) $(CFLAGS) innerLeafs.o util.o List.o -o innerLeafs
	$(CC) $(CFLAGS) work.o -o workers -lm

run: 
	./myprime -l 1 -u 1000 -w 4

myprime.o: myprime.c
	$(CC) -c $(CFLAGS) myprime.c

innerLeafs.o: innerLeafs.c
	$(CC) -c $(CFLAGS) innerLeafs.c

work.o: work.c
	$(CC) -c $(CFLAGS) work.c

util.o: util.c
	$(CC) -c $(CFLAGS) util.c 

List.o: List.c
	$(CC) -c $(CFLAGS) List.c 

clean:
	rm -f $(OBJS) $(EXECS)