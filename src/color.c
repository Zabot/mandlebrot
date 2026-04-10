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

  /*unsigned short range = (max - min);*/
  /*float f = (float) (x - min) / ((float) range);*/
  /*unsigned short stop = ((unsigned short)(f * ((repeat * gradient->len) -
   * 1)));*/
  /*float f2 = (f * (repeat * gradient->len - 1)) - (float)stop;*/

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

  /*Color c = {*/
  /*(unsigned short)(r->r - l->r) * f2 + l->r,*/
  /*(unsigned short)(r->g - l->g) * f2 + l->g,*/
  /*(unsigned short)(r->b - l->b) * f2 + l->b,*/
  /*};*/
  /*return c;*/

  if (f2 > d) {
    return *r;
  }
  return *l;
}

void color(int xRes, int yRes, unsigned short *data, Color *image) {
  int gradientStopsLen = 3;
  Color gradientStops[] = {
      /*{0,0,0},*/
      /*{255,255,255},*/
      /*{255,0,0},*/
      /*{255,255,0},*/
      /*{0,255,0},*/
      /*{0,255,255},*/
      /*{255,255,255},*/
      {0, 43, 54},
      {7, 54, 66},
      {88, 110, 117},
      /*{101,123,131},*/
      /*{131,	148,	150},*/
      /*{147,	161,	161},*/
      /*{238,	232,	213},*/
      /*{253,	246,	227},*/
      /*{181,	137,	0},*/
      /*{203,	75 ,22},*/
      /*{220,	50 ,47},*/
      /*{211,	54 ,130},*/
      /*{108,	113,	196},*/
      /*{38 ,139 ,210},*/
      /*{42 ,161 ,152},*/
      /*{133,	153,	0},*/
      /*{0,255,0},*/
      /*{0,255,255},*/
      /*{0,0,255},*/
      /*{255,255,255},*/
  };
  const Gradient gradient = {
      gradientStops,
      gradientStopsLen,
  };

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
      const Color color = interpolate(&gradient, o, min, max, x, y);
      memcpy(image + (xRes * y) + x, &color, sizeof(Color));
    }
  }
}
