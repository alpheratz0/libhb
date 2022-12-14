.POSIX:
.PHONY: all clean stadium_print

include config.mk

all: libhb.a stadium_print

src/stadium.o: src/stadium.c
stadium_print.o: stadium_print.c

libhb.a: src/stadium.o
	make -C third_party/c-stringbuilder -B
	$(AR) -rcs libhb.a src/stadium.o

stadium_print: stadium_print.o libhb.a
	$(CC) $(LDFLAGS) -o stadium_print stadium_print.o $(LDLIBS)

clean:
	rm -f src/*.o libhb.a stadium_print *.o
