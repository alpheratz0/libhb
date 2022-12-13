.POSIX:
.PHONY: all clean

include config.mk

all: libhb.a

src/stadium.o: src/stadium.c

libhb.a: src/stadium.o
	$(AR) -rcs libhb.a src/stadium.o

clean:
	rm -f src/*.o libhb.a
