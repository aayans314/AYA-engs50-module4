CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl

all: indexer indexer_3 indexer_4

indexer:
	gcc $(CFLAGS) indexer.c $(LIBS) -o $@
indexer_3: indexer_3.c
	gcc $(CFLAGS) indexer_3.c $(LIBS) -o $@

indexer_4: indexer_4.c
	gcc $(CFLAGS) indexer_4.c $(LIBS) -o $@

clean:
	rm -f indexer indexer_3 indexer_4

rebuild:
	$(MAKE) clean
	$(MAKE) all
