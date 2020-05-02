cc=gcc
CFLAGS=-std=c89 -pedantic-errors -O3 -Wall -Wpedantic -Wswitch -Werror
bel_objs= bel.o pair.o parse.o print.o sym.o pg.o

all : bel

bel : $(bel_objs)
	cc -o bel $(bel_objs)

test : $(test_objs)
	cc -o test $(test_objs)

bel.o : env.h type.h pair.h parse.h sym.h

continuation.o : env.h type.h pair.h sym.h print.h eval.h

env.o : continuation.h pair.h sym.h type.h

eval.o : type.h continuation.h pair.h sym.h

pair.o : type.h sym.h

parse.o : type.h pair.h print.h sym.h env.h eval.h pg.h

pg.h : type.h

print.o : type.h pair.h sym.h

sym.o : type.h pair.h

.PHONY : clean
clean :
	rm bel *.o
