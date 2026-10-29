CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl
VERSIONS=indexer

all: $(VERSIONS)

indexer: indexer.c
	gcc $(CFLAGS) indexer.c $(LIBS) -o $@

clean:
	rm -f $(VERSIONS)

rebuild:
	$(MAKE) clean
	$(MAKE) all
