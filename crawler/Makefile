CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl

all: crawler crawler_hash

crawler:
	gcc $(CFLAGS) crawler.c $(LIBS) -o $@

crawler_hash:
	gcc $(CFLAGS) crawler_hash.c $(LIBS) -o $@

clean:
	rm -f crawler crawler_hash

rebuild:
	$(MAKE) clean
	$(MAKE) crawler
	$(MAKE) crawler_hash
