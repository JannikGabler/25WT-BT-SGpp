#pragma once

#include <cmath>
#include <complex>
#include <vector>
#include "sgpp/combigrid/tools/math/fft_internals/cooley_tukey.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

namespace fourier_transform {

inline size_t nextPow2(size_t n) {
  if (n == 0) return 1;
  --n;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
#if SIZE_MAX > UINT32_MAX
  n |= n >> 32;
#endif
  return ++n;
}

template <typename T>
std::vector<std::complex<T>> convolution(const std::vector<std::complex<T>>& a,
                                         const std::vector<std::complex<T>>& b) {
  size_t n = a.size();
  size_t m = b.size();
  if (!n || !m) return {};
  size_t sz = nextPow2(n + m - 1);
  std::vector<std::complex<T>> fa(sz, std::complex<T>(0, 0));
  std::vector<std::complex<T>> fb(sz, std::complex<T>(0, 0));
  for (size_t i = 0; i < n; ++i) fa[i] = a[i];
  for (size_t i = 0; i < m; ++i) fb[i] = b[i];
  fftPow2(fa, false);
  fftPow2(fb, false);
  for (size_t i = 0; i < sz; ++i) fa[i] *= fb[i];
  fftPow2(fa, true);
  fa.resize(n + m - 1);
  return fa;
}

/*
Inplace implementation of Bluestein's algorithm (Chirp-Z) to compute DFT of length n.
This reduces the DFT to a convolution which we compute via next power-of-two FFT.
*/
template <typename T>
void bluesteinTransform(std::vector<std::complex<T>>& a, bool inverse = false) {
  const size_t n = a.size();
  if (n == 0) return;

  const T PI = static_cast<T>(3.14159265358979323846264338327950288L);
  const int forward_sign = -1;
  const int inv_sign = 1;
  const int sign = inverse ? inv_sign : forward_sign;

  // w = exp(i * sign * pi / n)   (note: sign negative for forward)
  auto w = std::complex<T>(std::cos(sign * PI / static_cast<T>(n)),
                           std::sin(sign * PI / static_cast<T>(n)));

  // a_mod[k] = a[k] * w^{k^2}
  std::vector<std::complex<T>> a_mod(n);
  std::complex<T> wk(1, 0);
  for (size_t k = 0; k < n; ++k) {
    // w^{k*k} = exp(i * sign * pi * k^2 / n)
    T angle = static_cast<T>(sign) * PI * static_cast<T>(k) * static_cast<T>(k) / static_cast<T>(n);
    std::complex<T> wk2 = std::polar(static_cast<T>(1), angle);
    a_mod[k] = a[k] * wk2;
  }

  // b sequence length = 2*n - 1, b[k] = w^{-k^2} for k = -(n-1)..(n-1)
  std::vector<std::complex<T>> b(2 * n - 1);
  for (size_t k = 0; k < 2 * n - 1; ++k) {
    long idx = static_cast<long>(k) - static_cast<long>(n) + 1;  // idx in [-(n-1) .. (n-1)]
    T angle =
        static_cast<T>(-sign) * PI * static_cast<T>(idx) * static_cast<T>(idx) / static_cast<T>(n);
    b[k] = std::polar(static_cast<T>(1), angle);
  }

  // convolution c = a_mod * b
  auto c = convolution<T>(a_mod, b);  // length n + (2n-1) -1 = 3n-2, but we only need certain part

  // recover X[m] = w^{m^2} * c[m + n -1]
  for (size_t m = 0; m < n; ++m) {
    T angle = static_cast<T>(sign) * PI * static_cast<T>(m) * static_cast<T>(m) / static_cast<T>(n);
    std::complex<T> wmm = std::polar(static_cast<T>(1), angle);
    a[m] = wmm * c[m + n - 1];
  }

  if (inverse) {
    // Bluestein produced unscaled inverse; scale by 1/n here
    T invn = static_cast<T>(1) / static_cast<T>(n);
    for (size_t i = 0; i < n; ++i) a[i] *= invn;
  }
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp