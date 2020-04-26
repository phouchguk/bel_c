cc=gcc
CFLAGS=-std=c89 -pedantic-errors -O3 -Wall -Wpedantic -Wswitch -Werror
bel_objs= bel.o pair.o parse.o print.o sym.o eval.o continuation.o
test_objs= test.o

all : bel test

bel : $(bel_objs)
	cc -o bel $(bel_objs)

test : $(test_objs)
	cc -o test $(test_objs)

bel.o : type.h pair.h parse.h sym.h

print.o : type.h pair.h sym.h

sym.o : type.h pair.h

pair.o : type.h sym.h

parse.o : type.h pair.h print.h sym.h eval.h

continuation.o : type.h pair.h sym.h print.h eval.h

eval.o : type.h continuation.h pair.h sym.h

.PHONY : clean
clean :
	rm bel test *.o
