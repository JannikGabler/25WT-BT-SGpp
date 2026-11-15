// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

// Select the FFT backend from the SCons flags. Eigen is preferred if both are enabled, as its FFT
// is 1.2-3.7x faster than Armadillo's for the transform lengths used here.
#if defined(USE_EIGEN)
#include <eigen3/unsupported/Eigen/FFT>
#elif defined(USE_ARMADILLO)
#include <armadillo>
#endif

namespace sgpp {
namespace combigrid {
namespace quadrature_rules {

#ifdef SGPP_COMBIGRID_HAVE_FFT
namespace {

/**
 * Fills @p c (length m) with the even entries c[j] = A[2j] of the length-2m input of
 * Waldvogel's method (its odd entries are 0): c[0] = 0 and c[j] = c[m - j] = b_j / (4j^2 - 1) for
 * j = 1, ..., m/2 with b_j = 1 for j = m/2 (even m), else 2. Returns A_N = c[m/2] for even m,
 * else 0.
 */
double buildEvenInput(const std::span<double> c) {
  const size_t m = c.size();
  c[0] = 0.0;

  for (size_t j = 1; j <= m / 2; ++j) {
    const double bj = ((m % 2 == 0) && (j == m / 2)) ? 1.0 : 2.0;
    const double denom = 4.0 * static_cast<double>(j) * static_cast<double>(j) - 1.0;
    const double factor = bj / denom;

    c[j] = factor;
    c[m - j] = factor;  // The same entry for j = m/2 (even m)
  }

  return (m % 2 == 0) ? c[m / 2] : 0.0;
}

/**
 * Writes the bins 0, ..., c.size() / 2 of the unnormalized forward discrete Fourier transform of
 * the real vector @p c to @p spectrum (spectrum.size() = c.size() / 2 + 1).
 */
void forwardRealDFT(const std::span<const double> c, std::vector<std::complex<double>>& spectrum) {
  assert(spectrum.size() == c.size() / 2 + 1);

  if (c.size() == 1) {  // The identity (Eigen's kissfft fails for length 1)
    spectrum[0] = c[0];
    return;
  }

#if defined(USE_EIGEN)
  // Only the first half of the spectrum (c is real): for lengths divisible by 4, Eigen computes it
  // by a complex transform of half the length
  Eigen::FFT<double> fft;
  fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
  fft.fwd(spectrum.data(), c.data(), static_cast<Eigen::Index>(c.size()));
#elif defined(USE_ARMADILLO)
  arma::vec input(c.size());
  std::ranges::copy(c, input.begin());
  const arma::cx_vec full = arma::fft(input);
  std::copy_n(full.begin(), spectrum.size(), spectrum.begin());
#endif
}

}  // namespace
#endif  // SGPP_COMBIGRID_HAVE_FFT

base::DataVector ClenshawCurtisQuadRule::genWeights(const GPCntType nNodes) const {
  if (nNodes == 0) {
    return base::DataVector();
  } else if (nNodes == 1) {
    return base::DataVector{1};
  }

#ifdef SGPP_COMBIGRID_HAVE_FFT
  return genWeightsFFT(nNodes);
#else
  return genWeightsNaive(nNodes);
#endif
}

void ClenshawCurtisQuadRule::genWeightsInplace(const std::span<double> out) const {
  if (out.empty()) {
    return;
  } else if (out.size() == 1) {
    out[0] = 1.0;
    return;
  }

#ifdef SGPP_COMBIGRID_HAVE_FFT
  genWeightsFFTInplace(out);
#else
  genWeightsNaiveInplace(out);
#endif
}

base::DataVector ClenshawCurtisQuadRule::genWeightsNaive(const size_t nNodes) const {
  base::DataVector weights(nNodes);
  genWeightsNaiveInplace(weights);
  return weights;
}

/**
 * The sums s_k of Waldvogel's method (see genWeightsFFTInplace) evaluated directly, the cosines
 * by the Chebyshev recurrence: O(n^2), only intended as a fallback without an FFT backend.
 */
void ClenshawCurtisQuadRule::genWeightsNaiveInplace(const std::span<double> out) const {
  assert(out.size() >= 2);

  std::fill(out.begin(), out.end(), 0.0);

  const size_t nNodes = out.size();
  const size_t m = nNodes - 1;
  const size_t mHalf = m / 2;

  for (size_t i = 1; i <= mHalf; i++) {
    const double bj = ((m % 2 == 0) && (i == m / 2)) ? 1.0 : 2.0;
    const double denom = 4.0 * static_cast<double>(i) * static_cast<double>(i) - 1.0;
    const double factor = bj / denom;
    const double phi = 2.0 * static_cast<double>(i) * M_PI / static_cast<double>(m);
    const double cos_phi = std::cos(phi);
    double c_km1 = cos_phi;
    double c_km2 = 1.0;

    out[0] += factor * c_km2;

    if (mHalf >= 1) {
      out[1] += factor * c_km1;

      for (size_t j = 2; j <= mHalf; j++) {
        double c_k = 2.0 * cos_phi * c_km1 - c_km2;
        out[j] += factor * c_k;
        c_km2 = c_km1;
        c_km1 = c_k;
      }
    }
  }

  // Mirror values
  for (size_t i = 0; i <= mHalf; i++) {
    out[m - i] = out[i];
  }

  for (size_t k = 0; k <= m; ++k) {  // Weights on [-1, 1]
    const double ck = (k == 0 || k == m) ? 1.0 : 2.0;
    out[k] = ck / static_cast<double>(m) * (1.0 - out[k]);
  }

  std::ranges::reverse(out);  // Weights on [0, 1]: w01[i] = 0.5 * wcc[m - i]
  for (double& weight : out) {
    weight *= 0.5;
  }
}

base::DataVector ClenshawCurtisQuadRule::genWeightsFFT(const size_t nNodes) const {
  base::DataVector weights(nNodes);
  genWeightsFFTInplace(weights);
  return weights;
}

#ifdef SGPP_COMBIGRID_HAVE_FFT

/**
 * Waldvogel's method: the weights on [-1, 1] are w_k = c_k / m * (1 - s_k) with c_0 = c_m = 1,
 * else 2, and s_k = sum_{j=1..m/2} b_j / (4j^2 - 1) * cos(2 pi j k / m). Waldvogel computes s_k
 * from the real parts of the FFT of a length-2m vector A whose odd entries are 0; its even
 * entries c[j] = A[2j] give the same bins by a real FFT of length m:
 * Re DFT_2m(A)[k] = DFT_m(c)[k] = sum_j c[j] cos(2 pi j k / m), which is real and symmetric
 * (DFT_m(c)[m - k] = DFT_m(c)[k]), and s_k = (DFT_m(c)[k] + (-1)^k A_N) / 2 (the sum counts the
 * j = m/2 term of even m twice, s_k once). Hence only the bins k <= m/2 are computed and every
 * weight is written together with its mirror image: the weights are symmetric bitwise, so the
 * weights on [0, 1], w01[i] = w[m - i] / 2, need no reversal. The input is built in @p out
 * itself.
 */
void ClenshawCurtisQuadRule::genWeightsFFTInplace(const std::span<double> out) const {
  assert(out.size() >= 2);

  const size_t m = out.size() - 1;
  const double A_N = buildEvenInput(out.first(m));

  std::vector<std::complex<double>> spectrum(m / 2 + 1);
  forwardRealDFT(out.first(m), spectrum);

  const double mDouble = static_cast<double>(m);
  for (size_t k = 0; k <= m / 2; k++) {
    const double sign = (k % 2 == 0) ? 1.0 : -1.0;  // (-1)^k, equal to (-1)^(m-k) if A_N != 0
    const double s = 0.5 * (spectrum[k].real() + sign * A_N);
    const double ck = (k == 0) ? 1.0 : 2.0;  // c_k = c_{m-k}; k = m/2 < m as m >= 1
    const double weight = 0.5 * (ck / mDouble * (1.0 - s));
    out[k] = weight;
    out[m - k] = weight;
  }
}

#else

void ClenshawCurtisQuadRule::genWeightsFFTInplace(const std::span<double> out) const {
  throw base::dependency_exception(
      "FFT backend (EIGEN or ARMADILLO) is required but not available. Please make sure SG++ is "
      "built including EIGEN or ARMADILLO.");
}

#endif  // SGPP_COMBIGRID_HAVE_FFT

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
