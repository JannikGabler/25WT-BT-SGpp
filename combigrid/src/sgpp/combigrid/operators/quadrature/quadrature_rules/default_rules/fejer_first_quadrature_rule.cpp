// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/fejer_first_quadrature_rule.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

// The FFT backend as in the Clenshaw-Curtis rule: Eigen if enabled, else Armadillo
#if defined(USE_EIGEN)
#include <eigen3/unsupported/Eigen/FFT>
#elif defined(USE_ARMADILLO)
#include <armadillo>
#endif

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

namespace {

/// Returns @f$\pi p / n@f$ for @p p reduced modulo @f$2n@f$ into @f$(-n, n]@f$ in integer
/// arithmetic: the angle of @f$e^{i \pi p / n}@f$, accurate also for large @p p.
double reducedAngle(const uint64_t p, const uint64_t n) {
  const uint64_t r = p % (2 * n);
  const double signedR = (r > n) ? -static_cast<double>(2 * n - r) : static_cast<double>(r);
  return std::numbers::pi * signedR / static_cast<double>(n);
}

/// Returns @f$e^{i \pi p / n}@f$ (see @ref reducedAngle).
std::complex<double> unitRoot(const uint64_t p, const uint64_t n) {
  const double angle = reducedAngle(p, n);
  return {std::cos(angle), std::sin(angle)};
}

/// The coefficient @f$1 / (4j^2 - 1)@f$ of the cosine sums of Fejér's first rule.
double fejerCoefficient(const uint64_t j) {
  const double jDouble = static_cast<double>(j);
  return 1.0 / (4.0 * jDouble * jDouble - 1.0);
}

/// Writes the weights @f$(1 - 2 s_k) / n@f$ of Fejér's first rule from the cosine sums
/// @f$s_k@f$, @f$k \le (n - 1) / 2@f$, to @p out and their mirror images (symmetric bitwise).
void writeSymmetricWeights(const std::span<const double> sums, const std::span<double> out) {
  const size_t n = out.size();
  assert(sums.size() == (n - 1) / 2 + 1);

  for (size_t k = 0; k < sums.size(); k++) {
    const double weight = (1.0 - 2.0 * sums[k]) / static_cast<double>(n);
    out[k] = weight;
    out[n - 1 - k] = weight;
  }
}

/// The integral of the Chebyshev polynomial @f$T_k@f$ over @f$[-1, 1]@f$.
double chebyshevIntegral(const uint64_t k) {
  if (k % 2 == 1) return 0.0;
  const double kDouble = static_cast<double>(k);
  return 2.0 / (1.0 - kDouble * kDouble);
}

/// The weight of both boundary nodes on [0, 1] for @p m inner nodes:
/// @f$\tfrac14 \int_{-1}^1 (1 + t) T_m(t)\,dt@f$, with @f$t T_m = (T_{m+1} + T_{m-1}) / 2@f$.
double boundaryWeight(const uint64_t m) {
  const double integralOfT = chebyshevIntegral(m);
  const double integralOfTTimesT = 0.5 * (chebyshevIntegral(m + 1) + chebyshevIntegral(m - 1));
  return 0.25 * (integralOfT + integralOfTTimesT);
}

#ifdef SGPP_COMBIGRID_HAVE_FFT

/**
 * Unnormalized forward discrete Fourier transforms of power-of-two length @p M. One object serves
 * all transforms of a weight computation: Eigen computes the twiddle factors of a length (one
 * sincos per entry) once per object, and only forward transforms are used (the inverse is formed
 * by conjugation), so the plan is built once instead of three times.
 */
class ForwardDFT {
 public:
  /// @brief Writes the transform of @p in to @p out (both of the same power-of-two length).
  void operator()(const std::vector<std::complex<double>>& in,
                  std::vector<std::complex<double>>& out) {
    assert(in.size() == out.size() && std::has_single_bit(in.size()));
#if defined(USE_EIGEN)
    fft_.fwd(out.data(), in.data(), static_cast<Eigen::Index>(in.size()));
#elif defined(USE_ARMADILLO)
    arma::cx_vec input(in.size());
    std::ranges::copy(in, input.begin());
    const arma::cx_vec result = arma::fft(input);
    std::ranges::copy(result, out.begin());
#endif
  }

 private:
#if defined(USE_EIGEN)
  Eigen::FFT<double> fft_;
#endif
};

/**
 * Fills the inputs of Bluestein's convolution for Fejér's sums of length @p n (see
 * FejerFirstQuadRule::genFejerWeightsFFTInplace): @p chirp with @f$h_d = \omega^{-d^2}@f$ at
 * @f$d \bmod M@f$ for @f$d \in [-J, K]@f$, @p input with @f$b_j = c_j \omega^{j(j+1)}@f$,
 * @f$j = 1, \ldots, J@f$; all other entries 0.
 */
void fillBluesteinInputs(const uint64_t n, std::vector<std::complex<double>>& chirp,
                         std::vector<std::complex<double>>& input) {
  const uint64_t J = n / 2;
  const uint64_t K = (n - 1) / 2;  // K <= J
  const size_t M = chirp.size();
  assert(input.size() == M && M >= n);

  std::ranges::fill(chirp, 0.0);
  std::ranges::fill(input, 0.0);
  for (uint64_t d = 0; d <= J; d++) {
    const std::complex<double> value = std::conj(unitRoot(d * d, n));
    if (d <= K) chirp[d] = value;
    if (d >= 1) chirp[M - d] = value;
  }
  for (uint64_t j = 1; j <= J; j++) {
    input[j] = fejerCoefficient(j) * unitRoot(j * (j + 1), n);
  }
}

/**
 * Overwrites @p input with the complex conjugate of its cyclic convolution with @p chirp, times
 * the length @f$M@f$: @f$\mathrm{conj}(F(\mathrm{conj}(F b \cdot F h)))@f$ is @f$M@f$ times the
 * inverse transform of @f$F b \cdot F h@f$, so all three transforms are forward ones (one plan).
 * @p chirp is used as scratch.
 */
void convolveCyclicallyConjugated(std::vector<std::complex<double>>& input,
                                  std::vector<std::complex<double>>& chirp) {
  ForwardDFT forward;
  std::vector<std::complex<double>> chirpSpectrum(chirp.size());
  forward(chirp, chirpSpectrum);
  forward(input, chirp);  // chirp: the spectrum of the input
  for (size_t i = 0; i < chirp.size(); i++) chirp[i] = std::conj(chirp[i] * chirpSpectrum[i]);
  forward(chirp, input);
}

#endif  // SGPP_COMBIGRID_HAVE_FFT

}  // namespace

FejerFirstQuadRule::FejerFirstQuadRule(const bool includeBoundary)
    : includeBoundary_(includeBoundary) {}

base::DataVector FejerFirstQuadRule::genWeights(const GPCntType nNodes) const {
  base::DataVector weights(nNodes);
  genWeightsInplace(weights);
  return weights;
}

void FejerFirstQuadRule::genWeightsInplace(const std::span<double> out) const {
  if (includeBoundary_) {
    genWeightsWithBoundaryInplace(out);
  } else {
    genFejerWeightsInplace(out);
  }
}

void FejerFirstQuadRule::genFejerWeightsInplace(const std::span<double> out) {
#ifdef SGPP_COMBIGRID_HAVE_FFT
  genFejerWeightsFFTInplace(out);
#else
  genFejerWeightsNaiveInplace(out);
#endif
}

/**
 * Fejér's weights of the @f$m@f$ inner nodes, corrected by @f$-W (\ell_k(1) + \ell_k(-1))@f$
 * (class documentation). At the first-kind node @f$t_k = \cos\theta_k@f$,
 * @f$T_m'(t_k) = (-1)^k m / \sin\theta_k@f$, so @f$\ell_k(1) = (-1)^k \cot(\theta_k / 2) / m@f$
 * and @f$\ell_k(-1) = -(-1)^{m+k} \tan(\theta_k / 2) / m@f$; their sum is symmetric under
 * @f$k \to m - 1 - k@f$, so each correction is applied to a node and its mirror image.
 */
void FejerFirstQuadRule::genWeightsWithBoundaryInplace(const std::span<double> out) {
  const size_t nNodes = out.size();
  if (nNodes <= 2) {  // The midpoint, the endpoints {0, 1} (or no node)
    std::ranges::fill(out, 1.0 / static_cast<double>(std::max<size_t>(nNodes, 1)));
    return;
  }

  const size_t m = nNodes - 2;
  const std::span<double> inner = out.subspan(1, m);
  genFejerWeightsInplace(inner);

  const double weight = boundaryWeight(m);
  out.front() = weight;
  out.back() = weight;

  const double mDouble = static_cast<double>(m);
  const double mSign = (m % 2 == 0) ? 1.0 : -1.0;
  for (size_t k = 0; k < (m + 1) / 2; k++) {
    const double halfTheta = std::numbers::pi * static_cast<double>(2 * k + 1) / (4.0 * mDouble);
    const double kSign = (k % 2 == 0) ? 1.0 : -1.0;
    const double lagrangeSum =
        kSign * (1.0 / std::tan(halfTheta) - mSign * std::tan(halfTheta)) / mDouble;
    const double corrected = inner[k] - weight * lagrangeSum;
    inner[k] = corrected;
    inner[m - 1 - k] = corrected;
  }
}

/**
 * @f$s_k = \sum_{j=1}^{\lfloor n/2 \rfloor} \cos(\pi j (2k+1) / n) / (4j^2 - 1)@f$ directly for
 * @f$k \le (n-1)/2@f$, the arguments reduced exactly (@ref reducedAngle).
 */
void FejerFirstQuadRule::genFejerWeightsNaiveInplace(const std::span<double> out) {
  const uint64_t n = out.size();
  if (n == 0) return;

  std::vector<double> sums((n - 1) / 2 + 1, 0.0);
  for (uint64_t k = 0; k < sums.size(); k++) {
    for (uint64_t j = 1; j <= n / 2; j++) {
      sums[k] += fejerCoefficient(j) * std::cos(reducedAngle(j * (2 * k + 1), n));
    }
  }

  writeSymmetricWeights(sums, out);
}

#ifdef SGPP_COMBIGRID_HAVE_FFT

/**
 * With @f$\omega = e^{i\pi/n}@f$, @f$s_k = \mathrm{Re}\, S_k@f$ for
 * @f$S_k = \sum_{j=1}^{J} c_j \omega^{j} \omega^{2jk}@f$, @f$J = \lfloor n/2 \rfloor@f$,
 * @f$c_j = 1 / (4j^2 - 1)@f$: a DFT of length @f$n@f$. Bluestein's identity
 * @f$2jk = j^2 + k^2 - (k - j)^2@f$ turns it into
 * @f$S_k = \omega^{k^2} \sum_j b_j h_{k-j}@f$ with @f$b_j = c_j \omega^{j(j+1)}@f$ and the chirp
 * @f$h_d = \omega^{-d^2}@f$. Only @f$k \le K = \lfloor (n-1)/2 \rfloor@f$ is needed (symmetry),
 * so the differences @f$k - j@f$ span @f$[-J, K]@f$, @f$J + K + 1 = n@f$ values: a cyclic
 * convolution of any length @f$M \ge n@f$ is free of wrap-around; @f$M@f$ is the next power of
 * two. All powers of @f$\omega@f$ are formed from exponents reduced exactly modulo @f$2n@f$.
 */
void FejerFirstQuadRule::genFejerWeightsFFTInplace(const std::span<double> out) {
  const uint64_t n = out.size();
  if (n <= 2) {  // Weights 1 and {1/2, 1/2}; avoids transforms of length 1
    std::ranges::fill(out, 1.0 / static_cast<double>(std::max<uint64_t>(n, 1)));
    return;
  }

  const size_t M = std::bit_ceil(static_cast<size_t>(n));
  std::vector<std::complex<double>> chirp(M), input(M);
  fillBluesteinInputs(n, chirp, input);
  convolveCyclicallyConjugated(input, chirp);

  const double scale = 1.0 / static_cast<double>(M);
  std::vector<double> sums((n - 1) / 2 + 1);
  for (uint64_t k = 0; k < sums.size(); k++) {
    sums[k] = scale * (unitRoot(k * k, n) * std::conj(input[k])).real();
  }

  writeSymmetricWeights(sums, out);
}

#else

void FejerFirstQuadRule::genFejerWeightsFFTInplace(const std::span<double> out) {
  throw base::dependency_exception(
      "FFT backend (EIGEN or ARMADILLO) is required but not available. Please make sure SG++ is "
      "built including EIGEN or ARMADILLO.");
}

#endif  // SGPP_COMBIGRID_HAVE_FFT

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
