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
 * Fills @p b (length 2m) with the symmetric extension of the coefficient vector A[0..m] of
 * Waldvogel's method (A[2j] = b_j / (4j^2 - 1) for j = 1, ..., m/2, all other entries 0):
 * b[i] = A[i] for i <= m and b[i] = A[2m - i] for m < i < 2m. Returns A_N = A[m] for even m,
 * else 0.
 */
double buildSymmetricInput(const size_t m, std::vector<double>& b) {
  b.assign(2 * m, 0.0);

  for (size_t j = 1; j <= m / 2; ++j) {
    const double bj = ((m % 2 == 0) && (j == m / 2)) ? 1.0 : 2.0;
    const double denom = 4.0 * static_cast<double>(j) * static_cast<double>(j) - 1.0;
    const double factor = bj / denom;

    b[2 * j] = factor;
    if (2 * j < m) {
      b[2 * m - 2 * j] = factor;
    }
  }

  return (m % 2 == 0) ? b[m] : 0.0;
}

/**
 * Writes the real parts of the bins 0, ..., out.size() - 1 of the unnormalized forward discrete
 * Fourier transform of the real vector @p b (out.size() = b.size() / 2 + 1) to @p out.
 */
void forwardDFTRealParts(const std::vector<double>& b, const std::span<double> out) {
  assert(out.size() == b.size() / 2 + 1);

#if defined(USE_EIGEN)
  // Only the first half of the spectrum (b is real): Eigen computes it as for the full spectrum
  // and skips the reflection into the second half
  Eigen::FFT<double> fft;
  fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
  std::vector<std::complex<double>> B;
  fft.fwd(B, b);
#elif defined(USE_ARMADILLO)
  arma::vec br(b.size());
  std::ranges::copy(b, br.begin());
  const arma::cx_vec B = arma::fft(br);
#endif

  for (size_t i = 0; i < out.size(); i++) {
    out[i] = B[i].real();
  }
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

/**
 * TODO: Document (update)
 * Naive approach(runtime complexity in O(n ^ 2)).* Only intended as a fallback.
 */
base::DataVector ClenshawCurtisQuadRule::genWeightsNaive(const size_t nNodes) const {
  base::DataVector weights(nNodes);
  genWeightsNaiveInplace(weights);
  return weights;
}

// TODO: Decompose functionality into smaller functions(?)
/**
 * TODO: Document (update)
 * Naive approach(runtime complexity in O(n ^ 2)).* Only intended as a fallback.
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
 * TODO: Document (update)
 * Waldvogel method using the Fourier transform. The steps run in separate loops over the output (s,
 * the weights on [-1, 1], the weights on [0, 1]), so that every value is rounded as in a
 * computation with an array per step.
 */
void ClenshawCurtisQuadRule::genWeightsFFTInplace(const std::span<double> out) const {
  assert(out.size() >= 2);

  const size_t m = out.size() - 1;
  std::vector<double> b;
  const double A_N = buildSymmetricInput(m, b);

  // Recover s[k] = sum_{j=1..Jmax} factor_j * cos(2*pi*j*k/N)
  // Using relation: Re(B[k]) = A0 + (-1)^k AN + 2 * sum_{m=1..N-1} A[m] cos(pi*m*k/N)
  // Here A0 = 0 and only even m=2j contribute. The sum in Re(B[k]) excludes the j = N/2 term
  // (even N), which s[k] contains once, so s[k] = (Re(B[k]) + (-1)^k * A_N)/2
  forwardDFTRealParts(b, out);
  for (size_t i = 0; i <= m; i++) {
    const double sign = (i % 2 == 0) ? 1.0 : -1.0;  // (-1)^k
    out[i] = 0.5 * (out[i] + sign * A_N);
  }

  for (size_t i = 0; i <= m; i++) {  // Weights on [-1, 1]
    const double ck = (i == 0 || i == m) ? 1.0 : 2.0;
    out[i] = ck / static_cast<double>(m) * (1.0 - out[i]);
  }

  std::ranges::reverse(out);  // Weights on [0, 1]: w01[i] = 0.5 * wcc[m - i]
  for (double& weight : out) {
    weight *= 0.5;
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
