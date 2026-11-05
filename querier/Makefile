CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl
VERSIONS=query query-3 query-4 query-5

all: $(VERSIONS)

query: query.c
	gcc $(CFLAGS) query.c $(LIBS) -o $@

query-3: query-3.c
	gcc $(CFLAGS) query-3.c $(LIBS) -o $@

query-4: query-4.c
	gcc $(CFLAGS) query-4.c $(LIBS) -o $@

query-5: query-5.c
	gcc $(CFLAGS) query-5.c $(LIBS) -o $@

clean:
	rm -f $(VERSIONS)

rebuild:
	$(MAKE) clean
	$(MAKE) all
