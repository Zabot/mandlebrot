CC=gcc
CFLAGS=-Wall -I include -O3
LDFLAGS=-lm -lpthread

SRCS = src/mandlebrot.c src/color.c
OBJS = $(SRCS:.c=.o)

all: mandlebrot

mandlebrot: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

install: mandlebrot
	mkdir -p $(out)/bin
	install mandlebrot $(out)/bin/mandlebrot

clean:
	rm $(OBJS) mandlebrot

.PHONY: all clean
