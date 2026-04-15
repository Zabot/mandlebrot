#include "fractals/mandlebrot.h"

unsigned short mandlebrot(double r, double i, size_t max_steps) {
  const double r0 = r;
  const double i0 = i;
  for (int steps = 0; steps < max_steps; steps++) {
    double r2 = r * r;
    double i2 = i * i;

    if (r2 + i2 >= 4.0) {
      return steps;
    }

    double rri = (r + r) * i;
    r = (r2 - i2) + r0;
    i = rri + i0;
  }

  return max_steps;
}
