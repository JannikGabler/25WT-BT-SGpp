#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

// Select the FFT backend from the SCons flags. Eigen is preferred if both are enabled, as its FFT
// is 1.2-3.7x faster than Armadillo's for the transform lengths used here.
#if defined(USE_EIGEN)
#define HAVE_EIGEN_FFT 1
#include <eigen3/unsupported/Eigen/FFT>
#elif defined(USE_ARMADILLO)
#define HAVE_ARMADILLO 1
#include <armadillo>
#endif

namespace sgpp {
namespace combigrid {
namespace quadrature_rules {

// TODO: Optimize
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

// TODO: Optimize
void ClenshawCurtisQuadRule::genWeightsInplace(const std::span<double> out) const {
  const GPCntType nNodes = static_cast<GPCntType>(out.size());

  const base::DataVector weights = genWeights(nNodes);
  std::ranges::copy(weights, out.begin());
}

/*
Naive approach (runtime complexity in O(n^2)).
Only intended as a fallback.
*/
base::DataVector ClenshawCurtisQuadRule::genWeightsNaive(const size_t nNodes) const {
  assert(nNodes >= 2);

  const size_t m = nNodes - 1;
  const size_t mHalf = m / 2;
  std::vector<double> s(nNodes, 0.0);

  for (size_t i = 1; i <= mHalf; i++) {
    const double bj = ((m % 2 == 0) && (i == m / 2)) ? 1.0 : 2.0;
    const double denom = 4.0 * static_cast<double>(i) * static_cast<double>(i) - 1.0;
    const double factor = bj / denom;
    const double phi = 2.0 * static_cast<double>(i) * M_PI / static_cast<double>(m);
    const double cos_phi = std::cos(phi);
    double c_km1 = cos_phi;
    double c_km2 = 1.0;

    s[0] += factor * c_km2;

    if (mHalf >= 1) {
      s[1] += factor * c_km1;

      for (size_t j = 2; j <= mHalf; j++) {
        double c_k = 2.0 * cos_phi * c_km1 - c_km2;
        s[j] += factor * c_k;
        c_km2 = c_km1;
        c_km1 = c_k;
      }
    }
  }

  // Mirror values
  for (size_t i = 0; i <= mHalf; i++) {
    s[m - i] = s[i];
  }

  // Maps weights
  std::vector<double> wcc(nNodes);
  for (size_t k = 0; k <= m; ++k) {
    double ck = (k == 0 || k == m) ? 1.0 : 2.0;
    wcc[k] = ck / static_cast<double>(m) * (1.0 - s[k]);
  }

  base::DataVector w01(nNodes);  // Weights for [0, 1]
  for (size_t i = 0; i <= m; ++i) {
    w01[i] = 0.5 * wcc[m - i];
  }
  return w01;
}

#ifdef SGPP_COMBIGRID_HAVE_FFT

/*
Waldvogel method using the Inverse Fourier Transformation
*/
base::DataVector ClenshawCurtisQuadRule::genWeightsFFT(const size_t nNodes) const {
  assert(nNodes >= 2);

  const size_t m = nNodes - 1;
  const size_t Jmax = m / 2;

  // Build A[0..N] with nonzeros only at even indices A[2j] = factor_j
  std::vector<double> A(nNodes, 0.0);
  for (size_t j = 1; j <= Jmax; ++j) {
    const double bj = ((m % 2 == 0) && (j == m / 2)) ? 1.0 : 2.0;
    const double denom = 4.0 * static_cast<double>(j) * static_cast<double>(j) - 1.0;
    const double factor = bj / denom;
    const size_t idx = 2 * j;

    if (idx <= m) {
      A[idx] = factor;
    }
  }

  const double A_N = (m % 2 == 0) ? A[m] : 0.0;

  // Build symmetric real vector b of length 2N: b[0..N] = A[0..N], b[N+1..2N-1] = A[2N-m]
  std::vector<double> b(2 * m);

  for (size_t i = 0; i <= m; i++) {
    b[i] = A[i];
  }
  for (size_t i = m + 1; i < b.size(); i++) {
    b[i] = A[2 * m - i];
  }

  // Compute unnormalized forward FFT of b (length 2N)
  std::vector<std::complex<double>> B;
  B.resize(b.size());

#if defined(HAVE_EIGEN_FFT)
  {
    Eigen::FFT<double> fft;
    // Eigen::FFT::fwd(out, in) computes the discrete Fourier transform (unnormalized)
    // out is vector<complex>, in is vector<double>
    fft.fwd(B, b);
  }
#elif defined(HAVE_ARMADILLO)
  {
    arma::vec br(b.size());

    for (size_t i = 0; i < b.size(); ++i) {
      br(i) = b[i];
    }

    arma::cx_vec BB = arma::fft(br);  // unnormalized forward FFT

    for (size_t i = 0; i < b.size(); ++i) {
      B[i] = std::complex<double>(BB(i).real(), BB(i).imag());
    }
  }
#endif

  // Recover s[k] = sum_{j=1..Jmax} factor_j * cos(2*pi*j*k/N)
  // Using relation: Re(B[k]) = A0 + (-1)^k AN + 2 * sum_{m=1..N-1} A[m] cos(pi*m*k/N)
  // Here A0 = 0 and only even m=2j contribute. The sum in Re(B[k]) excludes the j = N/2 term
  // (even N), which s[k] contains once, so s[k] = (Re(B[k]) + (-1)^k * A_N)/2
  std::vector<double> s(nNodes);
  for (size_t i = 0; i <= m; i++) {
    const double ReBk = B[i].real();
    const double sign = (i % 2 == 0) ? 1.0 : -1.0;  // (-1)^k
    s[i] = 0.5 * (ReBk + sign * A_N);
  }

  std::vector<double> wcc(nNodes);  // weights on [-1,1]
  for (size_t i = 0; i <= m; i++) {
    double ck = (i == 0 || i == m) ? 1.0 : 2.0;
    wcc[i] = ck / static_cast<double>(m) * (1.0 - s[i]);
  }

  base::DataVector w01(nNodes);  // weights on [0, 1]
  for (size_t i = 0; i <= m; ++i) {
    w01[i] = 0.5 * wcc[m - i];
  }
  return w01;
}

#endif  // SGPP_COMBIGRID_HAVE_FFT

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
