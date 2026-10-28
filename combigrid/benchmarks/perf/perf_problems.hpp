// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_problems.hpp
 * @brief Test problems of the combigrid performance suite: integrands with exact integrals,
 * interpolation targets, deterministic evaluation points and multi-index sets.
 *
 * PROTECTED FILE (see combigrid/perf/README.md): the end-to-end benchmarks and the reference
 * results are defined in terms of these problems. Uses the public API of the module only.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace perf {

using DataVector = sgpp::base::DataVector;

/// @brief Deterministic 64-bit generator (SplitMix64), identical on every platform.
class Rng {
 public:
  explicit Rng(const uint64_t seed) : state_(seed) {}

  uint64_t next() {
    uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }

  /// @brief Uniform double in [0, 1).
  double uniform() { return static_cast<double>(next() >> 11) * 0x1.0p-53; }

  /// @brief Uniform double in [a, b).
  double uniform(const double a, const double b) { return a + (b - a) * uniform(); }

  /// @brief Uniform integer in [0, n).
  uint64_t index(const uint64_t n) { return next() % n; }

 private:
  uint64_t state_;
};

/// @brief Node family by short name: "equi", "cc" (Clenshaw-Curtis = Chebyshev 2nd kind),
/// "cheb1" (Chebyshev 1st kind).
inline NodeGenFunc* nodeGenFuncByName(const std::string& name) {
  if (name == "equi") return getEquidistantNodeGenFunc();
  if (name == "cc") return getClenshawCurtisNodeGenFunc();
  if (name == "cheb1") return getFirstTypeChebyshevNodeGenFunc();
  throw std::invalid_argument("Unknown node family " + name);
}

/// @brief Applies the common settings (doubling growth) to a generation instruction.
inline void configure(SGGenInstr& genInstr, const std::string& nodes,
                      const LvlType boundaryLvlOffset, const HyperCubeArea& domain) {
  genInstr.setNodeGenFunc(nodeGenFuncByName(nodes));
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
  genInstr.setDomain(domain);
}

inline HyperCubeArea unitDomain(const size_t nDim) { return HyperCubeArea(nDim, {0.0, 1.0}); }

/// @brief Fixed non-unit box: dimension k is [-0.5 + 0.1 k, 1.0 + 0.2 k].
inline HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t k = 0; k < nDim; k++) {
    const double kd = static_cast<double>(k);
    domain[k] = {-0.5 + 0.1 * kd, 1.0 + 0.2 * kd};
  }
  return domain;
}

/*******************************************************************************
Integrands (all with exact integrals over arbitrary boxes or the unit cube)
*******************************************************************************/

/// @brief f(x) = prod_k sin(x_k). Exact integral over a box: prod_k (cos a_k - cos b_k).
inline double sinProd(const DataVector& x) {
  double result = 1.0;
  for (size_t k = 0; k < x.size(); k++) result *= std::sin(x[k]);
  return result;
}

inline double sinProdIntegral(const HyperCubeArea& domain) {
  long double result = 1.0L;
  for (const auto& [a, b] : domain)
    result *= std::cos(static_cast<long double>(a)) - std::cos(static_cast<long double>(b));
  return static_cast<double>(result);
}

/// @brief f(x) = exp(0.5 * sum_k x_k) (cheap: one exp per evaluation).
/// Exact integral over a box: prod_k 2 (exp(b_k / 2) - exp(a_k / 2)).
inline double expSum(const DataVector& x) {
  double sum = 0.0;
  for (size_t k = 0; k < x.size(); k++) sum += x[k];
  return std::exp(0.5 * sum);
}

inline double expSumIntegral(const HyperCubeArea& domain) {
  long double result = 1.0L;
  for (const auto& [a, b] : domain) {
    result *= 2.0L * (std::exp(0.5L * static_cast<long double>(b)) -
                      std::exp(0.5L * static_cast<long double>(a)));
  }
  return static_cast<double>(result);
}

/**
 * @brief Integrand of the forward-UQ example of the thesis (Sections 6.3 and 7):
 * f(x) = prod_k 4 (1 - x_k)^3 * exp(-sum_k x_k / d) on [0, 1]^d (one exp per evaluation, as in
 * combigrid/benchmarks/bachelor_thesis/comparison/uq_electron_beam_template.cpp).
 */
inline double uqIntegrand(const DataVector& x) {
  const size_t nDim = x.size();
  double prod = 1.0;
  double sum = 0.0;
  for (size_t k = 0; k < nDim; k++) {
    const double v = 1.0 - x[k];
    prod *= 4.0 * v * v * v;
    sum += x[k];
  }
  return prod * std::exp(-sum / static_cast<double>(nDim));
}

/// @brief Exact integral of @ref uqIntegrand over [0, 1]^d, computed with a stable series
/// (the closed form of the thesis cancels catastrophically for larger d).
inline double uqIntegral(const size_t nDim) {
  // int_0^1 4 (1-x)^3 e^{-a x} dx = 4 e^{-a} int_0^1 t^3 e^{a t} dt
  //                               = 4 e^{-a} sum_{k>=0} a^k / (k! (k + 4)),   a = 1/d
  const long double a = 1.0L / static_cast<long double>(nDim);
  long double term = 1.0L;  // a^k / k!
  long double sum = 0.0L;
  for (int k = 0; k < 60; k++) {
    sum += term / static_cast<long double>(k + 4);
    term *= a / static_cast<long double>(k + 1);
  }
  const long double oneDim = 4.0L * std::exp(-a) * sum;
  return static_cast<double>(std::pow(oneDim, static_cast<long double>(nDim)));
}

/*******************************************************************************
Interpolation targets (exact value = function value)
*******************************************************************************/

/// @brief Smooth, non-polynomial interpolation target: prod_k cos(1.5 x_k + 0.2 k).
inline double cosProd(const DataVector& x) {
  double result = 1.0;
  for (size_t k = 0; k < x.size(); k++) {
    result *= std::cos(1.5 * x[k] + 0.2 * static_cast<double>(k));
  }
  return result;
}

/// @brief @p nPoints deterministic points, uniformly distributed in @p domain.
inline std::vector<DataVector> randomPoints(const HyperCubeArea& domain, const size_t nPoints,
                                            const uint64_t seed) {
  Rng rng(seed);
  std::vector<DataVector> points(nPoints, DataVector(domain.size()));
  for (DataVector& point : points) {
    for (size_t k = 0; k < domain.size(); k++) {
      point[k] = rng.uniform(domain[k].first, domain[k].second);
    }
  }
  return points;
}

/*******************************************************************************
Multi-index sets for MIVecSGGenInstr (adaptive-like, not downwards closed)
*******************************************************************************/

/**
 * @brief @p nMI deterministic random multi-indices of dimension @p nDim, each with a 1-norm of at
 * most @p maxSum, biased towards the first dimensions (anisotropic, like the result of a
 * dimension-adaptive refinement). Duplicates are possible; the set is not downwards closed.
 */
inline LvlMIVec anisotropicMIs(const size_t nDim, const size_t nMI, const LvlType maxSum,
                               const uint64_t seed) {
  Rng rng(seed);
  std::vector<std::vector<LvlType>> raw(nMI, std::vector<LvlType>(nDim, 0));
  for (std::vector<LvlType>& mi : raw) {
    const LvlType sum = static_cast<LvlType>(rng.index(maxSum + 1));
    for (LvlType unit = 0; unit < sum; unit++) {
      // dimension k is chosen with probability ~ 1 / (k + 1)
      size_t dim = 0;
      while (dim + 1 < nDim && rng.uniform() < 0.6) dim += 1 + rng.index(2);
      mi[std::min(dim, nDim - 1)]++;
    }
  }
  return LvlMIVec(raw);
}

}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp
