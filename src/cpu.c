#include "fractal.h"

#include "fractals/mandlebrot.h"

int execFractal(const FractalBounds *bounds, unsigned short *buffer) {
  const double dr = (bounds->r_end - bounds->r_start) / (double)bounds->r_steps;
  const double di = (bounds->i_end - bounds->i_start) / (double)bounds->i_steps;

  double imag = bounds->i_start;
  for (unsigned int i = 0; i < bounds->i_steps; i++) {
    double real = bounds->r_start;
    for (unsigned int r = 0; r < bounds->r_steps; r++) {
      buffer[bounds->r_steps * i + r] =
          mandlebrot(real, imag, bounds->max_iters);
      real += dr;
    }
    imag += di;
  }

  return 0;
}

#ifndef HAVE_PTHREAD
int exec(const FractalBounds *bounds, unsigned short *buffer) {
  return execFractal(bounds, buffer);
}
#else
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  unsigned short *buffer;
  const FractalBounds bounds;
} ThreadInfo;

void *mandlebrotThread(void *in) {
  ThreadInfo *info = (ThreadInfo *)in;
  execFractal(&info->bounds, info->buffer);
  return NULL;
}

int exec(const FractalBounds *bounds, unsigned short *buffer) {
  const size_t threads = 2 * sysconf(_SC_NPROCESSORS_ONLN);

  // Can't put infos on the stack inside of the for loop because they
  // need to outlive the thread.
  ThreadInfo infos[threads];
  pthread_t ids[threads];

  for (unsigned char t = 0; t < threads; t++) {
    double i_thread_range = (bounds->i_end - bounds->i_start) / (double)threads;
    size_t i_thread_steps = bounds->i_steps / threads;
    ThreadInfo info = {
        .bounds =
            {
                .i_start = bounds->i_start + i_thread_range * t,
                .i_end = bounds->i_start + i_thread_range * (t + 1),
                .i_steps = i_thread_steps,

                .r_start = bounds->r_start,
                .r_end = bounds->r_end,
                .r_steps = bounds->r_steps,

                .max_iters = bounds->max_iters,
            },
        .buffer = buffer + (t * i_thread_steps) * bounds->r_steps,
    };
    // We have to memcpy the struct because bounds is const
    memcpy(infos + t, &info, sizeof(ThreadInfo));
    pthread_create(&ids[t], NULL, &mandlebrotThread, &infos[t]);
  }

  for (unsigned char t = 0; t < threads; t++) {
    pthread_join(ids[t], NULL);
  }

  return 0;
}
#endif
