// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_interp_sol.hpp
 * @brief Speed-of-light loops of the interpolation operators on one tensor grid, shared by the
 * micro cases micro/sol/interp_tg..., micro/sol/interplin_tg... (perf_micro.cpp) and the
 * exploration driver perf_interp_explore.cpp.
 *
 * They do the work of the production per-TG kernels (same source-function calls at bitwise the
 * same points; global: the production 1D rules on the same slices) with the per-point work
 * around the integrand reduced to a minimum:
 * - global interpolation: nodes mapped onto the domain once per tensor grid
 *   (@c tools::mapToDomain), grid points assembled incrementally (odometer, dimension 0
 *   fastest, as the grid-point index order of the operator), dimension 0 interpolated slice by
 *   slice while evaluating, scratch reused (no allocation per tensor grid); the 1D rule is a
 *   parameter (the production methods, or the generic formula with cached weights);
 * - multilinear interpolation: corners visited in Gray-code order (one coordinate changes per
 *   corner), corner weights from a table built by doubling (2^d products instead of d 2^d;
 *   every weight is the product of the same factors in the same order as the operator's).
 */
#pragma once

#include <omp.h>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace perf {
namespace interp_sol {

/// Per-dimension coordinates of a tensor grid (nodes on the domain or normalized nodes).
using NodesPerDim = std::vector<std::vector<double>>;

/// @brief Normalized nodes of @p tg per dimension (@p unit) and their images in @p domain.
inline void tgNodes(const TensorGrid& tg, const HyperCubeArea& domain, NodesPerDim& unit,
                    NodesPerDim& mapped) {
  const size_t nDim = tg.nDim();
  unit.assign(nDim, {});
  mapped.assign(nDim, {});
  size_t offset = 0;
  for (size_t k = 0; k < nDim; k++) {
    const GPCntType n = tg.getGPCntPerDim()[k];
    const double lower = domain[k].first, width = domain[k].second - domain[k].first;
    for (GPCntType i = 0; i < n; i++) {
      const double node = tg.getNodesPerDim()[offset + i];
      unit[k].push_back(node);
      mapped[k].push_back(tools::mapToDomain(node, lower, width));
    }
    offset += n;
  }
}

/// Scratch of one thread for the global interpolation loops (reused across tensor grids).
struct GlobalScratch {
  base::DataVector gp;
  std::vector<size_t> idx;
  std::vector<double> slice, work, values;
  explicit GlobalScratch(const size_t nDim) : gp(nDim), idx(nDim) {}
};

/// @brief Calls visit(i, f(x_i)) for the points of a tensor grid in index order (dimension 0
/// fastest); @p x: nodes on the domain per dimension.
template <typename Visit>
void forEachValue(const SourceFunc& f, const NodesPerDim& x, GlobalScratch& s, Visit&& visit) {
  const size_t nDim = x.size();
  for (size_t k = 0; k < nDim; k++) {
    s.idx[k] = 0;
    s.gp[k] = x[k][0];
  }
  const size_t n0 = x[0].size();
  size_t start = 0;
  while (true) {
    for (size_t i = 0; i < n0; i++) {
      s.gp[0] = x[0][i];
      visit(start + i, f.evaluate(s.gp));
    }
    start += n0;
    size_t k = 1;
    for (; k < nDim; k++) {
      if (++s.idx[k] < x[k].size()) {
        s.gp[k] = x[k][s.idx[k]];
        break;
      }
      s.idx[k] = 0;
      s.gp[k] = x[k][0];
    }
    if (k >= nDim) break;
  }
}

/// @brief Collapses dimensions 1 ... d-1 of @c s.work (the @p cnt results of dimension 0) in
/// place, as the operator; @p rule(k, pos, nodes, values) is the 1D rule of dimension k.
template <typename Rule>
double collapseLaterDims(const NodesPerDim& nodes, const Rule& rule, const base::DataVector& q,
                         size_t cnt, GlobalScratch& s) {
  for (size_t k = 1; k < nodes.size(); k++) {
    const size_t nk = nodes[k].size();
    cnt /= nk;
    s.slice.resize(nk);
    for (size_t j = 0; j < cnt; j++) {
      std::copy_n(s.work.data() + j * nk, nk, s.slice.begin());
      s.work[j] = rule(k, q[k], nodes[k], s.slice);
    }
  }
  return s.work[0];
}

/// @brief Global interpolation of @p f on one tensor grid at the normalized point @p q
/// (@p unit / @p mapped from @ref tgNodes, @p rule as in @ref collapseLaterDims).
template <typename Rule>
double globalKernelTG(const SourceFunc& f, const NodesPerDim& unit, const NodesPerDim& mapped,
                      const Rule& rule, const size_t nGP, const base::DataVector& q,
                      GlobalScratch& s) {
  const size_t n0 = unit[0].size();
  const size_t cnt = nGP / n0;
  s.slice.resize(n0);
  s.work.resize(cnt);
  forEachValue(f, mapped, s, [&](const size_t i, const double v) {
    const size_t j = i % n0;
    s.slice[j] = v;
    if (j == n0 - 1) s.work[i / n0] = rule(0, q[0], unit[0], s.slice);
  });
  return collapseLaterDims(unit, rule, q, cnt, s);
}

/// 1D rules of the operator: the interpolation methods of the node generation functions.
class ProductionRule {
 public:
  explicit ProductionRule(const std::vector<const InterpolationMethod*>& methods)
      : methods(&methods) {}
  double operator()(const size_t k, const double pos, const std::vector<double>& nodes,
                    const std::vector<double>& values) const {
    return (*methods)[k]->interpolate(pos, nodes, values);
  }

 private:
  const std::vector<const InterpolationMethod*>* methods;
};

/**
 * Generic barycentric formula with the weights computed once per tensor grid and dimension: the
 * arithmetic of @c BarycentricFormula::interpolate (trivial results, then
 * @f$\sum_i w_i v_i / (x - x_i) / \sum_i w_i / (x - x_i)@f$) with precomputed weights.
 */
class CachedWeightsRule {
 public:
  /// @brief Weights of the normalized nodes @p unit (none for dimensions with < 3 nodes).
  explicit CachedWeightsRule(const NodesPerDim& unit) {
    for (const std::vector<double>& x : unit) {
      weights.push_back(x.size() >= 3 ? tools::barycentricFormulaWeights(x)
                                      : std::vector<double>());
    }
  }
  double operator()(const size_t k, const double pos, const std::vector<double>& nodes,
                    const std::vector<double>& values) const {
    const size_t n = nodes.size();
    if (n == 0) return 0.0;
    if (n == 1) return values[0];
    if (n == 2) {
      return (values[1] - values[0]) / (nodes[1] - nodes[0]) * (pos - nodes[0]) + values[0];
    }
    for (size_t i = 0; i < n; i++) {
      if (tools::nearlyEqual(pos, nodes[i])) return values[i];
    }
    double numerator = 0.0, denominator = 0.0;
    for (size_t i = 0; i < n; i++) {
      const double factor = weights[k][i] / (pos - nodes[i]);
      numerator += factor * values[i];
      denominator += factor;
    }
    return numerator / denominator;
  }

 private:
  std::vector<std::vector<double>> weights;
};

/// Per-thread scratch of the corner loops.
struct LinScratch {
  std::vector<std::pair<double, double>> nb;
  std::vector<double> lo, hi, weights;
  base::DataVector gp;
  explicit LinScratch(const size_t nDim)
      : nb(nDim), lo(nDim), hi(nDim), weights(size_t{1} << nDim), gp(nDim) {}
};

/// @brief Brackets the normalized point @p q in @p tg (as the operator) and maps the bracketing
/// coordinates onto @p domain (@c s.lo / @c s.hi).
inline void bracket(const TensorGrid& tg, const base::DataVector& q, const HyperCubeArea& domain,
                    LinScratch& s) {
  tg.getNeighborsForLinInterpolation(q, s.nb);
  for (size_t k = 0; k < tg.nDim(); k++) {
    const double lower = domain[k].first, width = domain[k].second - domain[k].first;
    s.lo[k] = tools::mapToDomain(s.nb[k].first, lower, width);
    s.hi[k] = tools::mapToDomain(s.nb[k].second, lower, width);
  }
}

/// @brief Weights of all 2^d corners by doubling (bit k of the index: upper neighbor in dim k).
inline void cornerWeights(const base::DataVector& q, const size_t nDim, LinScratch& s) {
  s.weights[0] = 1.0;
  for (size_t k = 0; k < nDim; k++) {
    const double a = s.nb[k].first, b = s.nb[k].second, length = b - a;
    const double wLo = (length == 0.0) ? 0.5 : (b - q[k]) / length;
    const double wHi = (length == 0.0) ? 0.5 : (q[k] - a) / length;
    const size_t half = size_t{1} << k;
    for (size_t j = 0; j < half; j++) {
      s.weights[j + half] = s.weights[j] * wHi;
      s.weights[j] *= wLo;
    }
  }
}

/// @brief Calls visit(c, f(corner c)) for the 2^d corners in Gray-code order.
template <typename Visit>
void forEachCorner(const SourceFunc& f, const size_t nDim, LinScratch& s, Visit&& visit) {
  for (size_t k = 0; k < nDim; k++) s.gp[k] = s.lo[k];
  visit(size_t{0}, f.evaluate(s.gp));
  const size_t nCorners = size_t{1} << nDim;
  for (size_t i = 1; i < nCorners; i++) {
    const size_t k = static_cast<size_t>(std::countr_zero(i));
    const size_t gray = i ^ (i >> 1);
    s.gp[k] = ((gray >> k) & 1) ? s.hi[k] : s.lo[k];
    visit(gray, f.evaluate(s.gp));
  }
}

/// @brief Multilinear interpolation of @p f on one tensor grid at the normalized point @p q
/// (weights: false = integrand only, the sum of the corner values).
inline double linearKernelTG(const SourceFunc& f, const TensorGrid& tg, const base::DataVector& q,
                             const HyperCubeArea& domain, const bool weights, LinScratch& s) {
  bracket(tg, q, domain, s);
  double sum = 0.0;
  if (weights) {
    cornerWeights(q, tg.nDim(), s);
    forEachCorner(f, tg.nDim(), s,
                  [&](const size_t c, const double v) { sum += s.weights[c] * v; });
  } else {
    forEachCorner(f, tg.nDim(), s, [&](size_t, const double v) { sum += v; });
  }
  return sum;
}

}  // namespace interp_sol
}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp
