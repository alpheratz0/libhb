.POSIX:
.PHONY: all clean install uninstall shared benchmark test

include config.mk

all: libhb.a
shared: libhb.so

src/stadium.o: src/stadium.c

libhb.a: src/stadium.o
	$(AR) -rcs libhb.a src/stadium.o

libhb.so: src/stadium.o
	$(CC) -shared -fPIC src/stadium.o -o libhb.so -ljq

benchmark: benchmark/benchmark.o libhb.a
	$(CC) benchmark/benchmark.o -o benchmark/benchmark libhb.a -ljq
	@benchmark/benchmark

test: test/test.o libhb.a
	$(CC) test/test.o -o test/test libhb.a -ljq
	@test/test

install: libhb.a
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	cp -r include/hb $(DESTDIR)$(PREFIX)/include/hb
	cp libhb.a $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/hb
	rm -f $(DESTDIR)$(PREFIX)/lib/libhb.a

clean:
	rm -f */*.o libhb.a libhb.so benchmark/benchmark test/test
