#include <complex.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

int mandlebrot(long double complex c, size_t maxSteps) {
  long double real = creall(c);
  long double imag = cimagl(c);
  long double zr = 0;
  long double zi = 0;

  for (size_t counter = 0; counter < maxSteps; counter++) {
    if (sqrt(zr * zr + zi * zi) >= 2) {
      return counter;
    }

    long double t = ((zr * zr) - (zi * zi)) + real;
    zi = (2 * zr * zi) + imag;
    zr = t;
  }

  return maxSteps;
}

int mandlebrot_simple(long double complex c, size_t maxSteps) {
  long double complex z = 0.0;

  for (size_t counter = 0; counter < maxSteps; counter++) {
    if (cabsl(z) >= 2.0) {
      return counter;
    }
    z = z * z + c;
  }

  return maxSteps;
}

typedef struct {
  long double iStep;
  long double rStep;

  unsigned short xRes;
  unsigned short yRes;

  size_t maxSteps;
} MandlebrotConstants;

typedef struct {
  long double complex cStart;

  unsigned int xRange;
  unsigned int yRange;

  unsigned short *buffer;

  const MandlebrotConstants *cn;
} ThreadInfo;

void *mandlebrotThread(void *in) {
  ThreadInfo *info = (ThreadInfo *)in;

  for (unsigned int y = 0; y < info->yRange; y++) {
    long double complex c = info->cStart + y * info->cn->iStep * I;
    for (unsigned int x = 0; x < info->xRange; x++) {
      c += info->cn->rStep;
      info->buffer[info->xRange * y + x] =
          mandlebrot_simple(c, info->cn->maxSteps);
    }
  }

  return NULL;
}

typedef struct {
  unsigned int steps;
  long double zoom;
  long double complex center;

  unsigned int x_res;
  unsigned int y_res;

  char *outfile;
  unsigned char threads;
} MandlebrotParams;

void render(const MandlebrotParams *params) {
  const long double rCenter = creall(params->center);
  const long double iCenter = cimagl(params->center);

  const long double aspectRatio = params->y_res / (long double)params->x_res;

  const long double rRange = 4.0 / params->zoom;
  const long double iRange = rRange * aspectRatio;

  const long double rStart = rCenter - rRange / 2;
  const long double iStart = iCenter - iRange / 2;

  const MandlebrotConstants consts = {
      .xRes = params->x_res,
      .yRes = params->y_res,
      .maxSteps = params->steps,
      .rStep = rRange / params->x_res,
      .iStep = iRange / params->y_res,
  };

  unsigned short *buffer =
      malloc(params->y_res * params->x_res * sizeof(unsigned short));

  // Split the window into chunks for each thread
  pthread_t ids[params->threads];

  // Can't put infos on the stack inside of the for loop because they
  // need to outlive the thread.
  ThreadInfo infos[params->threads];
  for (unsigned char t = 0; t < params->threads; t++) {
    unsigned int yRange = (params->y_res / params->threads);

    ThreadInfo info = {
        .cn = &consts,
        .xRange = params->x_res,
        .yRange = yRange,
        .cStart = rStart + I * (iStart + ((yRange * t) * consts.iStep)),
        .buffer = buffer + ((yRange * t) * params->x_res),
    };

    infos[t] = info;
    pthread_create(&ids[t], NULL, &mandlebrotThread, &infos[t]);
  }

  for (unsigned char t = 0; t < params->threads - 1; t++) {
    pthread_join(ids[t], NULL);
  }

  Color *image = malloc(params->x_res * params->y_res * sizeof(Color));
  color(params->x_res, params->y_res, buffer, image);

  // Write the bitmap
  printf("Writing %s...\n", params->outfile);

  FILE *fp = fopen(params->outfile, "wb");
  fprintf(fp, "P6\n%d %d\n255\n", params->x_res, params->y_res);
  fwrite(image, 1, 3 * params->y_res * params->x_res, fp);
  fclose(fp);

  free(image);
  free(buffer);

  printf("Done\n");
}

int main(int argc, char **argv) {
  MandlebrotParams params = {
      .steps = 5000,
      .zoom = 59979000000.0,
      .center = -.743643887037151 + 0.131825904205330 * I,
      .x_res = 2880,
      .y_res = 1920,
      .outfile = "mandlebrot.ppm",
      .threads = 16,
  };

  render(&params);
}
