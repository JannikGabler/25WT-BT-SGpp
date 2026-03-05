#pragma once

#include <cmath>
#include <complex>
#include <vector>
#include "sgpp/combigrid/tools/math/power.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

namespace fourier_transform {

/*
Compute bit-reversed indices for n = power of two
*/
inline std::vector<size_t> getBitReversedIndices(const size_t n) {
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

template <typename T>
inline std::vector<std::vector<std::complex<T>>> getTwiddles(const size_t n) {
  std::vector<std::vector<std::complex<T>>> twiddles;

  // twiddles per stage
  for (size_t len = 2; len <= n; len <<= 1) {
    const size_t half = len >> 1;
    const double ang = 2.0 * M_PI / double(len);

    std::vector<std::complex<T>> stage(half);

    for (size_t j = 0; j < half; ++j) {
      stage[j] =
          std::complex<T>(std::cos(ang * double(j) * -1.0), std::sin(ang * double(j) * -1.0));
    }

    twiddles.push_back(std::move(stage));
  }

  return twiddles;
}

/*
Iterative FFT algorithm based on Cooley turkey
*/
template <typename T>
void fftPow2(std::vector<std::complex<T>>& vec, const bool inverse) {
  assert(tools::isPowerOfTwo(vec.size()));

  const size_t n = vec.size();
  const std::vector<size_t> bitrev = getBitReversedIndices(n);
  const std::vector<std::vector<std::complex<T>>> twiddles = getTwiddles<T>(n);

  // bit-reversal
  for (size_t i = 0; i < n; ++i) {
    const size_t j = bitrev[i];

    if (i < j) {
      std::swap(vec[i], vec[j]);
    }
  }

  size_t stage_index = 0;
  for (size_t len = 2; len <= n; len <<= 1) {
    size_t half = len >> 1;
    const std::vector<std::complex<T>>& stage_twiddles = twiddles[stage_index++];

    // depending on inverse, twiddle signs are precomputed for forward (-) so invert if needed
    if (!inverse) {
      for (size_t i = 0; i < n; i += len) {
        for (size_t j = 0; j < half; ++j) {
          std::complex<T> w = stage_twiddles[j];
          std::complex<T> u = vec[i + j];
          std::complex<T> v = vec[i + j + half] * w;
          vec[i + j] = u + v;
          vec[i + j + half] = u - v;
        }
      }
    } else {
      // inverse -> use conjugate twiddles (because stage_tw stores forward twiddles)
      for (size_t i = 0; i < n; i += len) {
        for (size_t j = 0; j < half; ++j) {
          std::complex<T> w = std::conj(stage_twiddles[j]);
          std::complex<T> u = vec[i + j];
          std::complex<T> v = vec[i + j + half] * w;
          vec[i + j] = u + v;
          vec[i + j + half] = u - v;
        }
      }
    }
  }

  // // Preconditions: length is power of two
  // const size_t n = a.size();
  // if (n == 0) return;

  // // bit reversal permutation
  // static std::vector<size_t> rev_cache;
  // static size_t rev_cache_n = 0;
  // std::vector<size_t> rev = bitReversedIndices(n);
  // for (size_t i = 0; i < n; ++i) {
  //   if (i < rev[i]) std::swap(a[i], a[rev[i]]);
  // }

  // const T PI = static_cast<T>(3.14159265358979323846264338327950288L);
  // const int sign = inverse ? 1 : -1;

  // // iterative Danielson-Lanczos
  // for (size_t len = 2; len <= n; len <<= 1) {
  //   T angle = static_cast<T>(sign) * (2 * PI / static_cast<T>(len));
  //   std::complex<T> wlen = std::polar(static_cast<T>(1), angle);
  //   for (size_t i = 0; i < n; i += len) {
  //     std::complex<T> w(1, 0);
  //     size_t half = len >> 1;
  //     for (size_t j = 0; j < half; ++j) {
  //       std::complex<T> u = a[i + j];
  //       std::complex<T> v = a[i + j + half] * w;
  //       a[i + j] = u + v;
  //       a[i + j + half] = u - v;
  //       w *= wlen;
  //     }
  //   }
  // }

  // if (inverse) {
  //   // scale by 1/n
  //   T invn = static_cast<T>(1) / static_cast<T>(n);
  //   for (size_t i = 0; i < n; ++i) a[i] *= invn;
  // }
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp