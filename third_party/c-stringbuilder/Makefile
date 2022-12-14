all: libsb.a

sb.o: sb.c

libsb.a: sb.o
	ar -rcs libsb.a sb.o

clean:
	rm -f sb.o libsb.a

.PHONY: all clean
