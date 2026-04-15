#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "cpu.h"

#include "fractal.h"

void render(const FractalParams *params) {
  const double r_center = creall(params->center);
  const double i_center = cimagl(params->center);

  const double aspect_ratio = params->y_res / (double)params->x_res;

  const double r_range = 4.0 / params->zoom;
  const double i_range = r_range * aspect_ratio;

  const FractalBounds bounds = {
      .r_start = r_center - r_range / 2,
      .r_end = r_center + r_range / 2,
      .r_steps = params->x_res,

      .i_start = i_center - i_range / 2,
      .i_end = i_center + i_range / 2,
      .i_steps = params->y_res,

      .max_iters = params->steps,
  };

  unsigned short *buffer =
      malloc(params->x_res * params->y_res * sizeof(unsigned short));
  exec(&bounds, buffer);

  Color *image = malloc(params->x_res * params->y_res * sizeof(Color));
  color(image, params->x_res, params->y_res, buffer, &params->gradient,
        params->steps);

  free(buffer);

  FILE *fp = fopen(params->outfile, "wb");
  fprintf(fp, "P6\n%d %d\n255\n", params->x_res, params->y_res);
  fwrite(image, 1, 3 * params->y_res * params->x_res, fp);
  fclose(fp);

  free(image);
}
