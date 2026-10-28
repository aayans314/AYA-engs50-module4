CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl
VERSIONS=indexer indexer_3 indexer_4 indexer_5 indexer_final

all: $(VERSIONS)

indexer: indexer.c
	gcc $(CFLAGS) indexer.c $(LIBS) -o $@
indexer_3: indexer_3.c
	gcc $(CFLAGS) indexer_3.c $(LIBS) -o $@

indexer_4: indexer_4.c
	gcc $(CFLAGS) indexer_4.c $(LIBS) -o $@

indexer_5: indexer_5.c
	gcc $(CFLAGS) indexer_5.c $(LIBS) -o $@

indexer_final: indexer_final.c
	gcc $(CFLAGS) indexer_final.c $(LIBS) -o $@

clean:
	rm -f $(VERSIONS)

rebuild:
	$(MAKE) clean
	$(MAKE) all
