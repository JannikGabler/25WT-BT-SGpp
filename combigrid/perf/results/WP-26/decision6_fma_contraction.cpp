// WP-26 decision 6 (scratch measurement, not part of the build). Build: g++ -std=c++23 -O3 -march=native
// -c decision6_fma_contraction.cpp && objdump -d (which constructs keep x * w + a unfused: only the empty-asm barrier)
#include <cstddef>
void plain(const double* __restrict in, double* __restrict out, size_t n, double w, double a) {
  for (size_t i = 0; i < n; i++) out[i] = in[i] * w + a;
}
void barrier(const double* __restrict in, double* __restrict out, size_t n, double w, double a) {
  for (size_t i = 0; i < n; i++) out[i] = __builtin_assoc_barrier(in[i] * w) + a;
}
inline double mapToDomain(double x, double lower, double width) {
  double scaled = x * width;
  asm("" : "+x"(scaled));
  return scaled + lower;
}
void asmbar(const double* __restrict in, double* __restrict out, size_t n, double w, double a) {
  for (size_t i = 0; i < n; i++) out[i] = mapToDomain(in[i], a, w);
}
void sep(const double* __restrict in, double* __restrict out, size_t n, double w, double a) {
  for (size_t i = 0; i < n; i++) { double s = in[i] * w; out[i] = s + a; }
}
