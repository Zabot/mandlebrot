CC=gcc
CFLAGS=-Wall
LDFLAGS=-lm -lpthread

SRCS    = mandlebrot.c
OBJS    = $(SRCS:.c=.o)

all: mandlebrot

mandlebrot: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

install: mandlebrot
	mkdir -p $(out)/bin
	install mandlebrot $(out)/bin/mandlebrot

clean:
	rm $(OBJS) mandlebrot

.PHONY: all clean
