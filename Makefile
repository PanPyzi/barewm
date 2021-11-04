PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall -fpermissive

all:
	g++ $(CFLAGS) -I$(PREFIX)/include main.cpp -L$(PREFIX)/lib -lX11 -o barewm

clean:
	rm -f barewm
