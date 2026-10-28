CFLAGS= -Wall -pedantic -std=c11 -I ../utils -L ../lib -g
LIBS=-lutils -lcurl

all: pageio_test indexio_test

pageio_test:
	gcc $(CFLAGS) pageio_test.c $(LIBS) -o $@

indexio_test:
	gcc $(CFLAGS) indexio_test.c $(LIBS) -o $@

clean:
	rm -f pageio_test indexio_test

rebuild:
	$(MAKE) clean
	$(MAKE) pageio_test
	$(MAKE) indexio_test
