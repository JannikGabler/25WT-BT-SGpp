#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

// Detect Eigen or Armadillo at compile time
#if __has_include(<unsupported/Eigen/FFT>)
#define HAVE_EIGEN_FFT 1
#include <unsupported/Eigen/FFT>
#elif __has_include(<armadillo>)
#define HAVE_ARMADILLO 1
#include <armadillo>
#endif

namespace sgpp {
namespace combigrid {
namespace quadrature_rules {

base::DataVector ClenshawCurtisQuadRule::getWeights(const GPCntType nNodes) const {
  if (nNodes == 0) {
    return base::DataVector();
  } else if (nNodes == 1) {
    return base::DataVector{1};
  }

#if defined(HAVE_EIGEN_FFT) || defined(HAVE_ARMADILLO)
  try {
    return clenshawCurtisWeightsFFT(nNodes);
  } catch (...) {
    // If anything goes wrong, fallback
    return genWeightsNaive(nNodes);
  }
#else
  return genWeightsNaive(nNodes);
#endif
}

// TODO: Optimize
void ClenshawCurtisQuadRule::genWeightsInplace(const GPCntType nNodes, base::DataVector& out,
                                               size_t startIdx) const {
  assert(out.size() - startIdx >= nNodes);

  const base::DataVector weights = getWeights(nNodes);
  std::copy(weights.begin(), weights.end(), out.begin() + startIdx);
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

#if defined(HAVE_EIGEN_FFT) || defined(HAVE_ARMADILLO)

/*
Waldvogel method using the Inverse Fourier Transformation
*/
std::vector<double> clenshawCurtisWeightsFFT(const size_t nNodes) {
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
  for (size_t i = m + 1; m < b.size(); i++) {
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
    arma::vec br(len);

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
  // Here A0 = 0 and only even m=2j contribute, so s[k] = (Re(B[k]) - (-1)^k * A_N)/2
  std::vector<double> s(nNodes);
  for (size_t i = 0; i <= m; i++) {
    const double ReBk = B[i].real();
    const double sign = (i % 2 == 0) ? 1.0 : -1.0;  // (-1)^k
    s[i] = 0.5 * (ReBk - sign * A_N);
  }

  std::vector<double> wcc(nNodes);  // weights on [-1,1]
  for (size_t i = 0; i <= m; i++) {
    double ck = (i == 0 || i == m) ? 1.0 : 2.0;
    wcc[i] = ck / static_cast<double>(m) * (1.0 - s[i]);
  }

  std::vector<double> w01(nNodes);  // weights on [0, 1]
  for (size_t i = 0; i <= m; ++i) {
    w01[i] = 0.5 * wcc[m - i];
  }
  return w01;
}

#endif  // HAVE_EIGEN_FFT || HAVE_ARMADILLO

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
