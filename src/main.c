#include <argp.h>
#include <stdlib.h>
#include <unistd.h>

#include "color.h"
#include "mandlebrot.h"

const char *argp_program_version = "mandlebrot 0.1";
const char *argp_program_bug_address = "<zach@zabot.dev>";

static char doc[] = "generate mandlebrot sets";
static char args_doc[] = "";

static struct argp_option options[] = {
    {"steps", 's', "STEP_COUNT", 0, "Maximum number of steps"},
    {"zoom", 'z', "ZOOM_LEVEL", 0, "Level of magnification"},
    {"real", 'r', "REAL", 0, "Real component of center"},
    {"imag", 'i', "IMAGINARY", 0, "Imaginary component of center"},

    {"width", 'w', "PIXELS", 0, "Width of output image"},
    {"height", 'h', "PIXELS", 0, "Height of output image"},
    {"graident", 'g', "#FFFFFF", 0, "Add a gradient stop"},

    {"output", 'o', "FILE", 0, "Output image"},
    {"threads", 'j', "THREADS", 0,
     "How many threads to use, 0 uses 2 * number of cores"},

    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  MandlebrotParams *params = state->input;

  switch (key) {
  case 's':
    params->steps = atoi(arg);
    break;

  case 'z':
    params->zoom = strtold(arg, NULL);
    break;

  case 'r':
    params->center = CMPLXL(strtold(arg, NULL), cimagl(params->center));
    break;
  case 'i':
    params->center = CMPLXL(creall(params->center), strtold(arg, NULL));
    break;

  case 'w':
    params->x_res = atoi(arg);
    break;
  case 'h':
    params->y_res = atoi(arg);
    break;

  case 'o':
    params->outfile = arg;
    break;
  case 'j':
    params->threads = atoi(arg);
    break;

  case 'g':
    unsigned int rgb = strtoul(arg, NULL, 16);

    params->gradient.stops[params->gradient.len] = (Color){
        .r = (rgb >> 16) & 0xFF,
        .g = (rgb >> 8) & 0xFF,
        .b = rgb & 0xFF,
    };
    params->gradient.len += 1;
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  Color stops[32];
  MandlebrotParams params = {
      .steps = 5000,
      .zoom = 59979000000.0,
      .center = -.743643887037151 + 0.131825904205330 * I,
      .x_res = 800,
      .y_res = 800,
      .outfile = "mandlebrot.ppm",
      .threads = 0,
      .gradient =
          {
              .len = 0,
              .stops = stops,
          },
  };

  argp_parse(&argp, argc, argv, 0, 0, &params);

  if (params.threads == 0) {
    params.threads = 2 * sysconf(_SC_NPROCESSORS_ONLN);
  }

  if (params.gradient.len == 0) {
    params.gradient.len = 2;
    stops[0] = (Color){0, 0, 0};
    stops[1] = (Color){255, 255, 255};
  }

  render(&params);

  return 0;
}
