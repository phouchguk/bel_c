cc=gcc
CFLAGS=-O2 -Wall -Wpedantic -Wswitch -Werror
bel_objs= bel.o pair.o parse.o sym.o
test_objs= test.o pair.o print.o sym.o

all : bel test

bel : $(bel_objs)
	cc -o bel $(bel_objs)

test : $(test_objs)
	cc -o test $(test_objs)

bel.o : type.h pair.h parse.h sym.h

test.o : type.h pair.h print.h

print.o : type.h pair.h sym.h

sym.o : type.h pair.h

pair.o : type.h

.PHONY : clean
clean :
	rm bel test *.o
