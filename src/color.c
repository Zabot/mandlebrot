#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "color.h"

Color interpolate(const Gradient *gradient, double f, int px, int py) {
  f = f * (gradient->len - 1);

  size_t stop = (size_t)floor(f);
  double f2 = f - stop;

  Color *l = &gradient->stops[stop];

  if (stop == gradient->len - 1) {
    return *l;
  }

  Color *r = &gradient->stops[(stop + 1)];

  if (!gradient->dither) {
    return (Color){
      .r = (r->r - l->r) * f2 + l->r,
      .g = (r->g - l->g) * f2 + l->g,
      .b = (r->b - l->b) * f2 + l->b,
    };
  }

  unsigned char bayer[4][4] = {
      {0, 8, 2, 10},
      {12, 4, 14, 6},
      {3, 11, 1, 9},
      {15, 7, 13, 5},
  };
  float d = (float)bayer[px % 4][py % 4] / 16.0;

  if (f2 > d) {
    return *r;
  }
  return *l;
}

void color(Color *image, int xRes, int yRes, const unsigned short *data,
           const Gradient *gradient, int maxSteps) {
  /*unsigned short min = USHRT_MAX;*/
  /*unsigned short max = 0;*/

  int *histogram = calloc(maxSteps, sizeof(int));
  int total = 0;
  for (int i = 0; i < xRes * yRes; i++) {
    histogram[data[i]]++;
    total++;
  }

  int cum = 0;
  for (int i = 0; i < maxSteps; i++) {
      cum += histogram[i];
      histogram[i] = cum;
  }

  for (int y = 0; y < yRes; y++) {
    for (int x = 0; x < xRes; x++) {
      const unsigned short steps = data[xRes * y + x];
      const double n = (double)histogram[steps] / (double)(xRes * yRes);
      const Color color = interpolate(gradient, n, x, y);

      memcpy(image + (xRes * y) + x, &color, sizeof(Color));
    }
  }
}
