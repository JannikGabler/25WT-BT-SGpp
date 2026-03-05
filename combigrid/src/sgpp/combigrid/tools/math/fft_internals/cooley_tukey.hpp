#pragma once

#include <cmath>
#include <complex>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace fourier_transform {

/*
Compute bit-reversed indices for n = power of two
*/
inline std::vector<size_t> bitReversedIndices(const size_t n) {
  size_t bits = 0;
  while ((size_t(1) << bits) < n) ++bits;
  std::vector<size_t> rev(n);
  for (size_t i = 0; i < n; ++i) {
    size_t x = i;
    size_t r = 0;
    for (size_t b = 0; b < bits; ++b) {
      r = (r << 1) | (x & 1);
      x >>= 1;
    }
    rev[i] = r;
  }
  return rev;
}

/*
Iterative FFT algorithm based on Cooley turkey
*/
template <typename T>
void fftPow2(std::vector<std::complex<T>>& a, const bool inverse) {
  // Preconditions: length is power of two
  const size_t n = a.size();
  if (n == 0) return;

  // bit reversal permutation
  static std::vector<size_t> rev_cache;
  static size_t rev_cache_n = 0;
  std::vector<size_t> rev = bitReversedIndices(n);
  for (size_t i = 0; i < n; ++i) {
    if (i < rev[i]) std::swap(a[i], a[rev[i]]);
  }

  const T PI = static_cast<T>(3.14159265358979323846264338327950288L);
  const int sign = inverse ? 1 : -1;

  // iterative Danielson-Lanczos
  for (size_t len = 2; len <= n; len <<= 1) {
    T angle = static_cast<T>(sign) * (2 * PI / static_cast<T>(len));
    std::complex<T> wlen = std::polar(static_cast<T>(1), angle);
    for (size_t i = 0; i < n; i += len) {
      std::complex<T> w(1, 0);
      size_t half = len >> 1;
      for (size_t j = 0; j < half; ++j) {
        std::complex<T> u = a[i + j];
        std::complex<T> v = a[i + j + half] * w;
        a[i + j] = u + v;
        a[i + j + half] = u - v;
        w *= wlen;
      }
    }
  }

  if (inverse) {
    // scale by 1/n
    T invn = static_cast<T>(1) / static_cast<T>(n);
    for (size_t i = 0; i < n; ++i) a[i] *= invn;
  }
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp