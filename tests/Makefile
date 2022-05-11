
NRLEX=../nrlex

all: tests test_readtobuffer

tests: test_utils.o test_readtobuffer
	for a in test*.nrlex; do \
	b=`basename $$a .nrlex`; \
	$(NRLEX) < $$a > $$b.c; \
	gcc -o $$b $$b.c test_utils.o; \
	done

test_readtobuffer: test_readtobuffer.o test_utils.o

clean:
	rm -f test_readtobuffer
	rm -f *.o 
	for a in test*.nrlex; do \
	b=`basename $$a .nrlex`; \
	rm -f $$b.c; \
	rm -f $$b; \
	done
