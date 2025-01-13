flags=-O3 -Wall -std=c2x -I.
ldflags=-fPIC -shared -ldl -D_GNU_SOURCE
libdir=/lib/x86_64-linux-gnu/
incdir=/usr/include

all: clean utils.so

install: utils.so
	cp utils.so $(libdir)/libbu.so
	cp utils.h  $(incdir)
	ldconfig

utils.so: utils.o
	cc ${flags} $^ -o $@ ${ldflags}

utils.o: utils.c
	cc ${flags} -c $^ $(ldflags)

clean:
	rm -f utils.o utils.so
