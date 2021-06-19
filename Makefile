CC=gcc
CFLAGS=-std=c11 -pedantic -D_POSIX_SOURCE -D_XOPEN_SOURCE -Wall
CFLAGS+=-DDEBUG -g

malloctest: malloctest.c bestfit.c bestfit.h
	$(CC) $(CFLAGS) -o malloctest malloctest.c bestfit.c

clean:
	rm -f malloctest

.PHONY: clean
