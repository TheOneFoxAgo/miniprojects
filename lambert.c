#include <math.h>

double lambert(double a) {
  double w = log1p(a);
  double f = w + 1 / (1 + a);
  w -= f * log(f) / (1 + w);
  for (int i = 0; i < 3; i++) {
    f = (w - a * exp(-w)) / (1 + w);
    w -= f + f * f / 2;
  }
  return w;
}
