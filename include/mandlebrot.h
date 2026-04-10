#ifndef MANDLEBROT_H
#define MANDLEBROT_H

#include <complex.h>

typedef struct {
  unsigned int steps;
  long double zoom;
  long double complex center;

  unsigned int x_res;
  unsigned int y_res;

  char *outfile;
  unsigned char threads;
} MandlebrotParams;

void render(const MandlebrotParams *params);

#endif
