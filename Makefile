CC = gcc
CFLAGS = -Wall -I include -O3
LDFLAGS = -lm
BACKEND ?= cpu

# TODO Use autotools to conditionally enable openmp or opencl
SRCS = src/color.c \
	src/main.c \
	src/fractal.c

ifeq ($(BACKEND),opencl)
    SRCS += src/opencl.c
    LDFLAGS += -lOpenCL
else ifeq ($(BACKEND),openmp)
    SRCS += src/cpu.c src/fractals/mandlebrot.c
    LDFLAGS += -lpthread
    CFLAGS += -D HAVE_OPENMP
else
    SRCS += src/cpu.c src/fractals/mandlebrot.c
endif

OBJS = $(SRCS:.c=.o)

all: mandlebrot

src/opencl.o: src/fractals/mandlebrot.cl

mandlebrot: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

install: mandlebrot
	mkdir -p $(out)/bin
	install mandlebrot $(out)/bin/mandlebrot

clean:
	rm -f $(OBJS) mandlebrot

.PHONY: all clean
