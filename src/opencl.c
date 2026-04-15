#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CL_TARGET_OPENCL_VERSION 210
#include <CL/cl.h>

#include "color.h"
#include "fractal.h"

const char mandlebrotKernel[] = {
#embed "fractals/mandlebrot.cl"
};

const char *kernels[] = {
    mandlebrotKernel,
};

#define CHECK(err)                                                             \
  if (err != CL_SUCCESS) {                                                     \
    fprintf(stderr, "%s:%d(%d)\n", __FILE__, __LINE__, err);                   \
    return -1;                                                                 \
  }

int exec(const FractalBounds *bounds, unsigned short *buffer) {
  cl_int err;
  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);
  CHECK(err);

  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  CHECK(err);

  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  CHECK(err);

  cl_command_queue queue =
      clCreateCommandQueueWithProperties(context, device, NULL, &err);
  CHECK(err);

  cl_program program =
      clCreateProgramWithSource(context, 1, kernels, NULL, &err);
  CHECK(err);

  err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t logSize;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
                          &logSize);

    char *log = (char *)malloc(logSize);
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log,
                          NULL);

    fprintf(stderr, "Failed to compile kernel:\n%s\n", log);
    free(log);
    return -1;
  }

  cl_kernel kernel = clCreateKernel(program, "mandlebrot", &err);
  CHECK(err);

  // Calculate initial value for every pixel
  const size_t n = bounds->r_steps * bounds->i_steps;
  double *reals = malloc(n * sizeof(double));
  double *imags = malloc(n * sizeof(double));

  const double dr = (bounds->r_end - bounds->r_start) / (double)bounds->r_steps;
  const double di = (bounds->i_end - bounds->i_start) / (double)bounds->i_steps;

  double imag = bounds->i_start;
  for (unsigned int i = 0; i < bounds->i_steps; i++) {
    double real = bounds->r_start;
    for (unsigned int r = 0; r < bounds->r_steps; r++) {
      reals[bounds->r_steps * i + r] = real;
      imags[bounds->r_steps * i + r] = imag;
      real += dr;
    }
    imag += di;
  }

  cl_mem realBuffer =
      clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     n * sizeof(double), reals, &err);
  CHECK(err);

  cl_mem imagBuffer =
      clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     n * sizeof(double), imags, &err);
  CHECK(err);

  cl_mem iterBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                     n * sizeof(unsigned short), NULL, &err);
  CHECK(err);

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &realBuffer);
  CHECK(err);

  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &imagBuffer);
  CHECK(err);

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &iterBuffer);
  CHECK(err);

  err = clSetKernelArg(kernel, 3, sizeof(int), &(bounds->max_iters));
  CHECK(err);

  err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &n, NULL, 0, NULL, NULL);
  CHECK(err);

  err = clEnqueueReadBuffer(queue, iterBuffer, CL_TRUE, 0,
                            n * sizeof(unsigned short), buffer, 0, NULL, NULL);
  CHECK(err);

  clReleaseMemObject(iterBuffer);
  clReleaseMemObject(imagBuffer);
  clReleaseMemObject(realBuffer);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  return 0;
}
