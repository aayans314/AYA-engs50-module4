CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl
VERSIONS=query query-3

all: $(VERSIONS)

query: query.c
	gcc $(CFLAGS) query.c $(LIBS) -o $@

query-3: query-3.c
	gcc $(CFLAGS) query-3.c $(LIBS) -o $@

clean:
	rm -f $(VERSIONS)

rebuild:
	$(MAKE) clean
	$(MAKE) all
