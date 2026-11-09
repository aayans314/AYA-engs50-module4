# Makefile for the locked-queue tests
CFLAGS=-Wall -pedantic -std=c11 -I ../utils -L ../lib -g -pthread
LIBS=-lutils -lcurl
TESTS=lqtest1 lqtest2

all: $(TESTS)

lqtest1: lqtest1.c
	gcc $(CFLAGS) lqtest1.c $(LIBS) -o $@

lqtest2: lqtest2.c
	gcc $(CFLAGS) lqtest2.c $(LIBS) -o $@

clean:
	rm -f $(TESTS)

.PHONY: all clean
