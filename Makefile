.POSIX:
.PHONY: all clean valgrind

include config.mk

all: valgrind

src/stadium.o: src/stadium.c

libhb.a: src/stadium.o
	$(AR) -rcs libhb.a src/stadium.o

valgrind: 
	cc src/stadium.c stadium_print.c -ljq -I./include -o stadium_print -g
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./stadium_print

clean:
	rm -f src/*.o libhb.a stadium_print
