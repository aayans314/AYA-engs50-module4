CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl
VERSIONS=query

all: $(VERSIONS)

query: query.c
	gcc $(CFLAGS) query.c $(LIBS) -o $@

clean:
	rm -f $(VERSIONS)

rebuild:
	$(MAKE) clean
	$(MAKE) all
