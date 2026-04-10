#ifndef COLOR_H
#define COLOR_H

typedef struct __attribute__((packed)) {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} Color;

typedef struct {
  Color *stops;
  size_t len;
} Gradient;

void color(Color *image, int xRes, int yRes, const unsigned short *data,
           const Gradient *gradient);

#endif
