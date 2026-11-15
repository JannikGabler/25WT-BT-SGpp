// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// RP-5 sizing of WP-56 (a): cost of the CC weights' FFT for the largest rows, 1 thread.
// V0 = the library's method (Waldvogel, real input of length 2m, Eigen kissfft, HalfSpectrum);
// V1 = the same values from a real FFT of length m: b has non-zeros only at even indices, so
//      Re B[k] = DFT_m(c)[k] with c[j] = b[2j] (period m in k, symmetric: B[m - k] = B[k]);
// V2 = V1 with the Eigen::FFT object (plan, twiddles) reused across calls (as a cached plan).
// Prints ms per call (median of 5) and the max abs difference of the weights V1 vs. V0.
// Build: g++ -O3 -march=native -std=c++23 -I/usr/include/eigen3 cc_fft_variants.cpp -o cc_fft
#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdio>
#include <eigen3/unsupported/Eigen/FFT>
#include <vector>

namespace {

double input(size_t m, std::vector<double>& b) {  // as buildSymmetricInput
  b.assign(2 * m, 0.0);
  for (size_t j = 1; j <= m / 2; ++j) {
    const double bj = ((m % 2 == 0) && (j == m / 2)) ? 1.0 : 2.0;
    const double f = bj / (4.0 * double(j) * double(j) - 1.0);
    b[2 * j] = f;
    if (2 * j < m) b[2 * m - 2 * j] = f;
  }
  return (m % 2 == 0) ? b[m] : 0.0;
}

void finish(size_t m, double aN, std::vector<double>& out) {  // as genWeightsFFTInplace
  for (size_t i = 0; i <= m; i++) out[i] = 0.5 * (out[i] + ((i % 2 == 0) ? 1.0 : -1.0) * aN);
  for (size_t i = 0; i <= m; i++) out[i] = ((i == 0 || i == m) ? 1.0 : 2.0) / double(m) * (1.0 - out[i]);
  std::ranges::reverse(out);
  for (double& w : out) w *= 0.5;
}

void v0(size_t m, std::vector<double>& out) {
  std::vector<double> b;
  const double aN = input(m, b);
  Eigen::FFT<double> fft;
  fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
  std::vector<std::complex<double>> B;
  fft.fwd(B, b);
  for (size_t i = 0; i <= m; i++) out[i] = B[i].real();
  finish(m, aN, out);
}

void v1(size_t m, std::vector<double>& out, Eigen::FFT<double>* shared) {
  std::vector<double> c(m, 0.0);
  double aN = 0.0;
  for (size_t j = 1; j <= m / 2; ++j) {
    const double bj = ((m % 2 == 0) && (j == m / 2)) ? 1.0 : 2.0;
    const double f = bj / (4.0 * double(j) * double(j) - 1.0);
    c[j] = f;
    if (j < m - j) c[m - j] = f;
    if (2 * j == m) aN = f;
  }
  Eigen::FFT<double> local;
  Eigen::FFT<double>& fft = shared ? *shared : local;
  fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
  std::vector<std::complex<double>> B;
  fft.fwd(B, c);
  for (size_t k = 0; k <= m / 2; k++) out[k] = out[m - k] = B[k].real();
  finish(m, aN, out);
}

template <typename F>
double timeMs(F f) {
  std::vector<double> t;
  for (int r = 0; r < 5; r++) {
    const auto a = std::chrono::steady_clock::now();
    f();
    t.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - a).count());
  }
  std::ranges::sort(t);
  return t[2];
}

}  // namespace

int main() {
  for (size_t m : {size_t(1) << 15, size_t(1) << 19, size_t(1) << 20, size_t(1) << 21}) {
    std::vector<double> w0(m + 1), w1(m + 1), w2(m + 1);
    Eigen::FFT<double> shared;
    v1(m, w2, &shared);  // fill the shared plan
    const double t0 = timeMs([&] { v0(m, w0); });
    const double t1 = timeMs([&] { v1(m, w1, nullptr); });
    const double t2 = timeMs([&] { v1(m, w2, &shared); });
    double d = 0, rel = 0;
    for (size_t i = 0; i <= m; i++) {
      d = std::max(d, std::abs(w1[i] - w0[i]));
      rel = std::max(rel, std::abs(w1[i] - w0[i]) / std::abs(w0[i]));
    }
    std::printf("n = 2^%d+1: V0 %.2f ms | V1 %.2f ms (%.2fx) | V2 (plan reused) %.2f ms (%.2fx) | "
                "max |V1-V0| %.2e (rel %.2e, w ~ 1/m = %.2e)\n",
                int(std::log2(double(m))), t0, t1, t0 / t1, t2, t0 / t2, d, rel, 1.0 / double(m));
  }
}
