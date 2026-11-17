// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file test_functions.hpp
 * @brief Test integrands on [0, 1]^d with anisotropy parameters and exact integrals (Genz
 * families, the forward-UQ integrand) for the adaptive prototype.
 */

#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace wp62 {

using sgpp::base::DataVector;

struct TestFunction {
  std::function<double(const DataVector&)> func;
  double exact;
};

/// Per-dimension coefficients a_k = c * decay(k); @p reversed puts the important ones last.
inline std::vector<double> coefficients(const std::string& decay, const size_t nDim,
                                        const double c, const bool reversed) {
  std::vector<double> a(nDim);
  for (size_t k = 0; k < nDim; k++) {
    const double kk = static_cast<double>(k + 1);
    if (decay == "alg1") {
      a[k] = c / kk;
    } else if (decay == "alg2") {
      a[k] = c / (kk * kk);
    } else if (decay == "alg3") {
      a[k] = c / (kk * kk * kk);
    } else if (decay == "geo") {
      a[k] = c * std::pow(2.0, -static_cast<double>(k));
    } else if (decay == "iso") {
      a[k] = c;
    } else if (decay == "eff5") {  // 5 important dimensions, the rest 100 times weaker
      a[k] = k < 5 ? c : 0.01 * c;
    } else {
      throw std::invalid_argument("unknown decay " + decay);
    }
  }
  if (reversed) std::ranges::reverse(a);
  return a;
}

/// Shifts u_k in [0.3, 0.7] without symmetry about the midpoint.
inline std::vector<double> shifts(const size_t nDim) {
  std::vector<double> u(nDim);
  for (size_t k = 0; k < nDim; k++) {
    const double x = 0.6180339887498949 * static_cast<double>(k + 1);
    u[k] = 0.3 + 0.4 * (x - std::floor(x));
  }
  return u;
}

/// int_0^inf t e^{-t} prod_k (1 - e^{-a_k t}) / (a_k t) dt (composite Gauss-Legendre).
inline double cornerPeakIntegral(const std::vector<double>& a) {
  static const long double X[5] = {0.0L, 0.5384693101056831L, -0.5384693101056831L,
                                   0.9061798459386640L, -0.9061798459386640L};
  static const long double W[5] = {0.5688888888888889L, 0.4786286704993665L,
                                   0.4786286704993665L, 0.2369268850561891L,
                                   0.2369268850561891L};
  const int nPanels = 8000;
  const long double upper = 80.0L;
  const long double h = upper / nPanels;
  long double sum = 0;
  for (int p = 0; p < nPanels; p++) {
    for (int i = 0; i < 5; i++) {
      const long double t = h * (p + 0.5L + 0.5L * X[i]);
      long double prod = t * std::exp(-t);
      for (const double ak : a) {
        const long double s = static_cast<long double>(ak) * t;
        prod *= -std::expm1(-s) / s;
      }
      sum += 0.5L * h * W[i] * prod;
    }
  }
  return static_cast<double>(sum);
}

inline TestFunction makeFunction(const std::string& name, const std::vector<double>& a) {
  const size_t nDim = a.size();
  const std::vector<double> u = shifts(nDim);
  TestFunction f;
  if (name == "osc") {  // Genz oscillatory
    f.func = [a, u](const DataVector& x) {
      double s = 2.0 * M_PI * u[0];
      for (size_t k = 0; k < a.size(); k++) s += a[k] * x[k];
      return std::cos(s);
    };
    std::complex<long double> prod = std::polar(1.0L, 2.0L * M_PIl * u[0]);
    for (const double ak : a) {
      const std::complex<long double> i(0.0L, 1.0L);
      prod *= (std::exp(i * static_cast<long double>(ak)) - 1.0L) /
              (i * static_cast<long double>(ak));
    }
    f.exact = static_cast<double>(prod.real());
  } else if (name == "prodpeak") {  // Genz product peak, scaled to O(1)
    f.func = [a, u](const DataVector& x) {
      double p = 1.0;
      for (size_t k = 0; k < a.size(); k++) {
        const double t = a[k] * (x[k] - u[k]);
        p *= 1.0 + t * t;
      }
      return 1.0 / p;
    };
    long double prod = 1;
    for (size_t k = 0; k < nDim; k++) {
      const long double ak = a[k], uk = u[k];
      prod *= (std::atan(ak * (1 - uk)) + std::atan(ak * uk)) / ak;
    }
    f.exact = static_cast<double>(prod);
  } else if (name == "gauss") {  // Genz Gaussian
    f.func = [a, u](const DataVector& x) {
      double s = 0;
      for (size_t k = 0; k < a.size(); k++) {
        const double t = a[k] * (x[k] - u[k]);
        s += t * t;
      }
      return std::exp(-s);
    };
    long double prod = 1;
    for (size_t k = 0; k < nDim; k++) {
      const long double ak = a[k], uk = u[k];
      prod *= std::sqrt(M_PIl) / (2 * ak) * (std::erf(ak * (1 - uk)) + std::erf(ak * uk));
    }
    f.exact = static_cast<double>(prod);
  } else if (name == "cont") {  // Genz continuous (kinks at u_k)
    f.func = [a, u](const DataVector& x) {
      double s = 0;
      for (size_t k = 0; k < a.size(); k++) s += a[k] * std::abs(x[k] - u[k]);
      return std::exp(-s);
    };
    long double prod = 1;
    for (size_t k = 0; k < nDim; k++) {
      const long double ak = a[k], uk = u[k];
      prod *= (2 - std::exp(-ak * uk) - std::exp(-ak * (1 - uk))) / ak;
    }
    f.exact = static_cast<double>(prod);
  } else if (name == "corner") {  // corner peak with fixed exponent: (1 + sum a_k x_k)^-2
    f.func = [a](const DataVector& x) {
      double s = 1.0;
      for (size_t k = 0; k < a.size(); k++) s += a[k] * x[k];
      return 1.0 / (s * s);
    };
    f.exact = cornerPeakIntegral(a);
  } else if (name == "inter") {
    // Gaussian plus a pure interaction of the two least important dimensions that is invisible
    // on the axes: k(x_p) k(x_q), k(x) = (2x - 1)^2 vanishes at the level-0 node 0.5.
    const TestFunction base = makeFunction("gauss", a);
    size_t p = 0, q = 1;
    for (size_t k = 0; k < nDim; k++) {
      if (a[k] < a[p]) p = k;
    }
    q = p == 0 ? 1 : 0;
    for (size_t k = 0; k < nDim; k++) {
      if (k != p && a[k] < a[q]) q = k;
    }
    f.func = [base, p, q](const DataVector& x) {
      const double kp = 2.0 * x[p] - 1.0, kq = 2.0 * x[q] - 1.0;
      return base.func(x) + kp * kp * kq * kq;
    };
    f.exact = base.exact + 1.0 / 9.0;
  } else if (name == "uq") {  // forward-UQ integrand of the thesis (isotropic)
    f.func = [nDim](const DataVector& x) {
      double prod = 1.0, sum = 0.0;
      for (size_t k = 0; k < nDim; k++) {
        const double v = 1.0 - x[k];
        prod *= 4.0 * v * v * v;
        sum += x[k];
      }
      return prod * std::exp(-sum / static_cast<double>(nDim));
    };
    const long double aa = 1.0L / static_cast<long double>(nDim);
    long double term = 1.0L, sum = 0.0L;
    for (int k = 0; k < 60; k++) {
      sum += term / static_cast<long double>(k + 4);
      term *= aa / static_cast<long double>(k + 1);
    }
    f.exact = static_cast<double>(std::pow(4.0L * std::exp(-aa) * sum,
                                           static_cast<long double>(nDim)));
  } else {
    throw std::invalid_argument("unknown function " + name);
  }
  return f;
}

}  // namespace wp62
