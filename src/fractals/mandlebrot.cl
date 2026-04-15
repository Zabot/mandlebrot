__kernel void mandlebrot(__global double *real, __global double *imag,
                         __global unsigned short *iterations,
                         const int maxSteps) {
  int i = get_global_id(0);

  double r0 = real[i];
  double i0 = imag[i];
  for (int steps = 0; steps < maxSteps; steps++) {
    double r2 = real[i] * real[i];
    double i2 = imag[i] * imag[i];

    if (r2 + i2 >= 4.0) {
      iterations[i] = steps;
      return;
    }

    double rri = (real[i] + real[i]) * imag[i];
    real[i] = (r2 - i2) + r0;
    imag[i] = rri + i0;
  }

  iterations[i] = maxSteps;
}
