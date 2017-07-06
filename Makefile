CC := clang
CFLAGS :=

INSTALL := install
PREFIX := /usr/local
BINDIR := $(PREFIX)/bin

all: cd-replace rs-test

cd-replace:
	$(CC) $(CFLAGS) cd-replace.c -o cd-replace

rs-test:
	$(CC) $(CFLAGS) rs-test.c -o rs-test

.PHONY: install clean

install: all
	$(INSTALL) -d $(BINDIR)
	$(INSTALL) cd-replace rs-test $(BINDIR)

clean:
	rm -f cd-replace rs-test
