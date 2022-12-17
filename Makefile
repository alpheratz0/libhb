.POSIX:
.PHONY: all clean install uninstall shared

include config.mk

all: libhb.a
shared: libhb.so

src/stadium.o: src/stadium.c

libhb.a: src/stadium.o
	$(AR) -rcs libhb.a src/stadium.o

libhb.so: src/stadium.o
	$(CC) -shared -fPIC src/stadium.o -o libhb.so -ljq

install: libhb.a
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	cp -r include/hb $(DESTDIR)$(PREFIX)/include/hb
	cp libhb.a $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	rm -f $(DESTDIR)$(PREFIX)/lib/libhb.a

clean:
	rm -f src/*.o libhb.a libhb.so
