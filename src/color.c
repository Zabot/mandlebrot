#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "color.h"

Color interpolate(const Gradient *gradient, unsigned short x,
                  unsigned short min, unsigned short max, int px, int py) {
  const int repeat = 3;
  const float scale = 1.0;

  float fmax = scale * log(max);
  float fmin = scale * log(min);
  float fx = scale * log(x);
  float frange = fmax - fmin;

  float f = (fx - fmin) / frange;
  unsigned short stop = ((unsigned short)(f * ((repeat * gradient->len) - 1)));
  float f2 = (f * (repeat * gradient->len - 1)) - (float)stop;

  Color *l = &gradient->stops[stop % gradient->len];
  Color *r = &gradient->stops[(stop + 1) % gradient->len];

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
           const Gradient *gradient) {
  unsigned short min = USHRT_MAX;
  unsigned short max = 0;

  for (int i = 0; i < xRes * yRes; i++) {
    unsigned short value = data[i];
    min = MIN(min, value);
    max = MAX(max, value);
  }

  for (int x = 0; x < xRes; x++) {
    for (int y = 0; y < yRes; y++) {
      const unsigned short o = data[xRes * y + x];
      const Color color = interpolate(gradient, o, min, max, x, y);
      memcpy(image + (xRes * y) + x, &color, sizeof(Color));
    }
  }
}
