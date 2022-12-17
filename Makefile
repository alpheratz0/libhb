.POSIX:
.PHONY: all clean install uninstall

include config.mk

all: libhb.a

src/stadium.o: src/stadium.c

libhb.a: src/stadium.o
	$(AR) -rcs libhb.a src/stadium.o

install: libhb.a
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	cp -r include/hb $(DESTDIR)$(PREFIX)/include/hb
	cp libhb.a $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	rm -f $(DESTDIR)$(PREFIX)/lib/libhb.a

clean:
	rm -f src/*.o libhb.a
