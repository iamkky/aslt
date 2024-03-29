
NRLEX=./nrlex

include make.aslt.inc

CFLAGS=

all: nrlex rdppgen

nrlex: nrlex.o utils.o

nrlex.o: nrlex.usagestr.h nrlex.c

nrlex.usagestr.h: nrlex.c
	grep //UU nrlex.c | cut -c 5- | od -t x1 | cut -c 9- | tr -d ' ' | tr -d '\n'| sed -e 's/^/char static *usagestr="/' -e 's/$$/";\n/' > nrlex.usagestr.h

rdppgen: rdppgen.embedded.h rdppgen.o rdpplex.o utils.o string.o

rdppgen.o: rdppgen.embedded.h rdppgen.c

rdppgen.embedded.h: rdppgen.c ddebug.h
	grep //UU rdppgen.c | cut -c 6- | od -t x1 | cut -c 9- | tr -d ' ' | tr -d '\n'| sed -e 's/^/char static *usagestr="/' -e 's/$$/";\n/' > rdppgen.embedded.h
	grep //E1 rdppgen.c | cut -c 6- | od -t x1 | cut -c 9- | tr -d ' ' | tr -d '\n'| sed -e 's/^/char static *emb1="/' -e 's/$$/";\n/' >> rdppgen.embedded.h
	grep //E2 rdppgen.c | cut -c 6- | od -t x1 | cut -c 9- | tr -d ' ' | tr -d '\n'| sed -e 's/^/char static *emb2="/' -e 's/$$/";\n/' >> rdppgen.embedded.h
	cat ddebug.h | od -t x1 | cut -c 9- | tr -d ' ' | tr -d '\n'| sed -e 's/^/char static *ddebug="/' -e 's/$$/";\n/' >> rdppgen.embedded.h

tests:
	cd json; make
	cd tests; make tests

clean:
	rm -f rdpplex.c nrlex.usagestr.h *.o nrlex rdppgen rdppgen.embedded.h
	cd tests; make clean

