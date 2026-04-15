#ifndef FRACTAL_H
#define FRACTAL_H

#include <complex.h>
#include <stddef.h>

#include "color.h"

typedef struct {
  unsigned int steps;
  long double zoom;
  long double complex center;

  unsigned int x_res;
  unsigned int y_res;

  char *outfile;
  unsigned char threads;

  Gradient gradient;
} FractalParams;

typedef struct {
  long double r_start;
  long double r_end;
  size_t r_steps;

  long double i_start;
  long double i_end;
  size_t i_steps;

  unsigned short max_iters;
} FractalBounds;

void render(const FractalParams *params);

int exec(const FractalBounds *bounds, unsigned short *buffer);

#endif
