CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl

all: crawler crawler_hash crawler_5 crawler_6

crawler:
	gcc $(CFLAGS) crawler.c $(LIBS) -o $@

crawler_hash:
	gcc $(CFLAGS) crawler_hash.c $(LIBS) -o $@

crawler_5:
	gcc $(CFLAGS) crawler_5.c $(LIBS) -o $@

crawler_6: crawler_6.c
	gcc $(CFLAGS) crawler_6.c $(LIBS) -o $@

clean:
	rm -f crawler crawler_hash crawler_5 crawler_6

rebuild:
	$(MAKE) clean
	$(MAKE) crawler
	$(MAKE) crawler_hash
	$(MAKE) crawler_5
