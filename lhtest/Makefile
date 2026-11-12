# Makefile for the locked-queue tests
CFLAGS=-Wall -pedantic -std=c11 -I ../utils -L ../lib -g -pthread
LIBS=-lutils -lcurl
TESTS=lhtest1 lhtest2

all: $(TESTS)

lhtest1: lhtest1.c
	gcc $(CFLAGS) lhtest1.c $(LIBS) -o $@

lhtest2: lhtest2.c
	gcc $(CFLAGS) lhtest2.c $(LIBS) -o $@

clean:
	rm -f $(TESTS)

.PHONY: all clean
