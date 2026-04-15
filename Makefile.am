bin_PROGRAMS = mandlebrot
mandlebrot_CFLAGS = -I include
mandlebrot_SOURCES = src/color.c \
	src/main.c \
	src/fractal.c

if HAVE_OPENCL
mandlebrot_SOURCES += src/opencl.c
else
mandlebrot_SOURCES += src/cpu.c src/fractals/mandlebrot.c
endif

