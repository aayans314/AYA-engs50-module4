# Makefile for the module 3
CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o pageio.o indexio.o

# build the .o files, then use ar to create (cr) the libutils.a in the lib directory
all: $(OFILES)
	ar cr ../lib/libutils.a $(OFILES)

%.o: %.c %.h
	gcc $(CFLAGS) -c $<


clean:
	rm -f *.o *.log 
