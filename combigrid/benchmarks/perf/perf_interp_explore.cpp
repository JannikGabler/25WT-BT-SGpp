// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_interp_explore.cpp
 * @brief Exploration driver of the interpolation operators on the e2e problems: structure and
 * evaluation counts, speed-of-light floors and prototypes of alternative evaluation schemes (not
 * part of the judged suite; it measures, it changes nothing in the library).
 *
 * For every interp / interplin configuration of perf_e2e.cpp (same grids, shifted domains, query
 * points and integrand perf::cosProd) the driver prints one Markdown row per table:
 * - structure: tensor grids (TGs), sum of their grid points, the largest TG relative to an even
 *   share of the threads (bound of the load balance of one parallel region over the TGs),
 *   evaluations of the p single-point calls and of a multi-point variant: global interpolation
 *   evaluates every TG once; multilinear interpolation evaluates the distinct corners of every TG
 *   over all p query points (counted per TG with a 64-bit hash of the corner coordinates); for
 *   global interpolation also the nodes regenerated per call vs. the integrand's coordinates;
 * - timings (median over the repetitions after one warm-up; all threads of the OpenMP runtime;
 *   the variants run interleaved):
 *   - api: the e2e operator phase (p single-point calls of interpolate / interpolateLinear);
 *   - kernel: the same evaluations and 1D rules (resp. corner weights) with the per-point work
 *     around the integrand reduced (perf_interp_sol.hpp); one parallel region per query point as
 *     the operator; "kernel cached weights": the same with the generic barycentric formula's
 *     weights computed once per TG (equidistant and Chebyshev-1 nodes);
 *   - sol_api: the integrand alone at every point the single-point calls evaluate (floor of any
 *     single-point implementation);
 *   - multi (global only): prototype of a multi-point interpolation: per TG (parallel over the
 *     TGs) the values at all its points once, then per query point the d nested 1D interpolations
 *     with the production 1D rules (nodes from the node generation function, as the operator);
 *     "multi cached weights" as above;
 *   - sol_multi (global only): the integrand alone at every TG point once (floor of multi);
 *   - api + cache: the p single-point calls with a CachedSourceFunc (default configuration),
 *     emptied before every repetition, and its hit rate;
 *   - dedup (global, nested node families): every distinct sparse-grid point evaluated once per
 *     call (values stored per delta tensor grid, see DeltaIndex), then per TG the values gathered
 *     and the 1D collapses; "multi + dedup": the distinct points once for all query points.
 *   The results are compared with api (largest absolute difference; they differ only by the
 *   summation order over the TGs, and kernel's corner order in multilinear interpolation).
 *
 * Usage: perf_interp_explore [--reps R] [--filter SUBSTRING] [--points N] [--alloc-probe N]
 * - --points N: only the first N query points of every case (1-thread runs of the large cases);
 * - --alloc-probe N: setup plus N single-point calls per selected case, no timing (for a heap
 *   profiler: the difference of two runs with different N gives the allocations per call).
 */

#include <omp.h>

#include <algorithm>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "perf_interp_sol.hpp"
#include "perf_problems.hpp"

namespace cg = sgpp::combigrid;
namespace perf = sgpp::combigrid::perf;
namespace sol = sgpp::combigrid::perf::interp_sol;
using cg::CompleteSGGenInstr;
using cg::GPCntType;
using cg::HyperCubeArea;
using cg::LvlType;
using cg::SourceFunc;
using cg::SparseGrid;
using cg::TensorGridCTData;
using perf::DataVector;

namespace {

/// One e2e interpolation configuration (perf_e2e.cpp, addInterpolation).
struct Config {
  bool linear;
  std::string nodes;
  LvlType boundary;
  size_t nDim;
  LvlType maxLvl;
  size_t nPoints;

  std::string name() const {
    return std::string(linear ? "interplin" : "interp") + "/sg/" + nodes + "/b" +
           std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" + std::to_string(maxLvl) +
           "/p" + std::to_string(nPoints);
  }
};

const std::vector<Config> CONFIGS = {
    {false, "cc", 0, 2, 12, 64},    {false, "cc", 0, 3, 9, 64},     {false, "cc", 0, 5, 6, 64},
    {false, "cc", 0, 7, 4, 16},     {false, "cc", 0, 10, 2, 8},     {false, "cheb1", 0, 3, 6, 64},
    {false, "cheb1", 0, 5, 4, 64},  {false, "equi", 0, 3, 5, 64},   {true, "equi", 1, 2, 16, 16384},
    {true, "equi", 1, 5, 8, 1024},  {true, "equi", 1, 8, 5, 256},   {true, "equi", 1, 10, 5, 32}};

/// A configuration with its grid, query points and per-TG data (setup, not timed).
struct Problem {
  Config cfg;
  HyperCubeArea domain;
  CompleteSGGenInstr genInstr;
  std::unique_ptr<SparseGrid> grid;
  SourceFunc func{perf::cosProd};
  std::vector<DataVector> points;
  std::vector<DataVector> normPoints;
  std::vector<const TensorGridCTData*> tgs;
  std::vector<sol::NodesPerDim> mapped;  ///< Per TG: nodes on the domain (tools::mapToDomain).
  std::vector<sol::NodesPerDim> unit;    ///< Per TG: normalized nodes (as stored in the TG).
  std::unique_ptr<cg::CachedSourceFunc> cached;  ///< Null without the cache's libraries.

  Problem(const Config& c, const size_t nPoints)
      : cfg(c), domain(perf::shiftedDomain(c.nDim)), genInstr(c.maxLvl, c.nDim) {
    perf::configure(genInstr, c.nodes, c.boundary, domain);
    grid = std::make_unique<SparseGrid>(genInstr);
    points = perf::randomPoints(domain, c.nPoints, 1000 + c.nDim);
    points.resize(std::min(nPoints, points.size()));
    for (const DataVector& x : points) {
      normPoints.push_back(cg::tools::normalizeDataVector(x, domain));
    }
    for (const TensorGridCTData& tgData : *grid) {
      tgs.push_back(&tgData);
      addNodes(tgData.tensorGrid);
    }
    try {
      cached = std::make_unique<cg::CachedSourceFunc>(perf::cosProd);
    } catch (const sgpp::base::dependency_exception&) {
      cached.reset();
    }
  }

  void addNodes(const cg::TensorGrid& tg) {
    sol::NodesPerDim u, m;
    sol::tgNodes(tg, domain, u, m);
    unit.push_back(std::move(u));
    mapped.push_back(std::move(m));
  }
};

double seconds(const std::chrono::steady_clock::time_point start) {
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

double median(std::vector<double> v) {
  std::sort(v.begin(), v.end());
  const size_t n = v.size();
  return (n % 2 == 1) ? v[n / 2] : 0.5 * (v[n / 2 - 1] + v[n / 2]);
}

/*******************************************************************************
Production API
*******************************************************************************/

/// The p single-point calls with the source-function cache (default configuration: memory
/// only), emptied before every repetition: values are reused within and across the calls.
double runApiCached(const Problem& pr, std::vector<double>& out, double& hitRate) {
  if (!pr.cached) return 0.0;
  pr.cached->cache()->clear();
  const auto start = std::chrono::steady_clock::now();
  for (size_t q = 0; q < pr.points.size(); q++) {
    out[q] = pr.cfg.linear ? cg::interpolateLinear(*pr.cached, pr.points[q], *pr.grid)
                           : cg::interpolate(*pr.cached, pr.points[q], *pr.grid);
  }
  const double t = seconds(start);
  const cg::CacheStatistics stats = pr.cached->statistics();
  hitRate = static_cast<double>(stats.hits) / static_cast<double>(stats.lookups);
  return t;
}

std::vector<double> runApi(const Problem& pr, const size_t nCalls) {
  std::vector<double> result(nCalls);
  for (size_t q = 0; q < nCalls; q++) {
    result[q] = pr.cfg.linear ? cg::interpolateLinear(pr.func, pr.points[q], *pr.grid)
                              : cg::interpolate(pr.func, pr.points[q], *pr.grid);
  }
  return result;
}

/*******************************************************************************
Global interpolation
*******************************************************************************/

std::vector<const cg::InterpolationMethod*> methodsOf(const Problem& pr, const size_t t) {
  std::vector<const cg::InterpolationMethod*> methods;
  for (size_t k = 0; k < pr.cfg.nDim; k++) {
    methods.push_back(pr.genInstr.getNodeGenFuncForDim(k)->getInterpolationMethod(
        static_cast<GPCntType>(pr.unit[t][k].size())));
  }
  return methods;
}

/// kernel: the p single-point calls, one parallel region per query point (as the operator);
/// cachedWeights: the generic barycentric formula with weights computed once per TG (setup).
double runGlobalKernel(const Problem& pr, const bool cachedWeights, std::vector<double>& out) {
  std::vector<std::vector<const cg::InterpolationMethod*>> methods;
  std::vector<sol::CachedWeightsRule> cached;
  for (size_t t = 0; t < pr.tgs.size(); t++) {
    methods.push_back(methodsOf(pr, t));
    cached.emplace_back(pr.unit[t]);
  }
  const auto start = std::chrono::steady_clock::now();
  for (size_t q = 0; q < pr.points.size(); q++) {
    double sum = 0.0;
#pragma omp parallel reduction(+ : sum)
    {
      sol::GlobalScratch s(pr.cfg.nDim);
#pragma omp for schedule(guided) nowait
      for (size_t t = 0; t < pr.tgs.size(); t++) {
        const size_t nGP = pr.tgs[t]->tensorGrid.nGP();
        const double v =
            cachedWeights
                ? sol::globalKernelTG(pr.func, pr.unit[t], pr.mapped[t], cached[t], nGP,
                                      pr.normPoints[q], s)
                : sol::globalKernelTG(pr.func, pr.unit[t], pr.mapped[t],
                                      sol::ProductionRule(methods[t]), nGP, pr.normPoints[q], s);
        sum += pr.tgs[t]->coefficient * v;
      }
    }
    out[q] = sum;
  }
  return seconds(start);
}

/// The 1D methods of a TG as the operator gets them (nodes regenerated by the node generation
/// function).
void rulesOf(const Problem& pr, const TensorGridCTData& tgData, sol::NodesPerDim& nodes,
             std::vector<const cg::InterpolationMethod*>& methods) {
  for (size_t k = 0; k < pr.cfg.nDim; k++) {
    const GPCntType n = tgData.tensorGrid.getGPCntPerDim()[k];
    const cg::NodeGenFunc* ngf = pr.genInstr.getNodeGenFuncForDim(k);
    const DataVector gen = ngf->genNodes(n, tgData.mi[k] >= pr.genInstr.getBoundaryLevelOffset());
    nodes[k].assign(gen.begin(), gen.end());
    methods[k] = ngf->getInterpolationMethod(n);
  }
}

/// The p query points on one TG whose values are in s.values (multi-point prototype).
template <typename Rule>
void interpolateAllPoints(const Problem& pr, const TensorGridCTData& tgData,
                          const sol::NodesPerDim& nodes, const Rule& rule, sol::GlobalScratch& s,
                          std::vector<double>& local) {
  const size_t n0 = nodes[0].size();
  const size_t cnt = s.values.size() / n0;
  for (size_t q = 0; q < pr.points.size(); q++) {
    s.slice.resize(n0);
    s.work.resize(cnt);
    for (size_t j = 0; j < cnt; j++) {
      std::copy_n(s.values.data() + j * n0, n0, s.slice.begin());
      s.work[j] = rule(0, pr.normPoints[q][0], nodes[0], s.slice);
    }
    local[q] += tgData.coefficient * sol::collapseLaterDims(nodes, rule, pr.normPoints[q], cnt, s);
  }
}

/// multi: every TG evaluated once, then the 1D collapses per query point (cachedWeights: the
/// generic formula's weights computed once per TG, inside the timed region).
double runGlobalMulti(const Problem& pr, const bool cachedWeights, std::vector<double>& out) {
  const size_t p = pr.points.size();
  std::fill(out.begin(), out.end(), 0.0);
  const auto start = std::chrono::steady_clock::now();
#pragma omp parallel
  {
    sol::GlobalScratch s(pr.cfg.nDim);
    std::vector<double> local(p, 0.0);
    sol::NodesPerDim nodes(pr.cfg.nDim);
    std::vector<const cg::InterpolationMethod*> methods(pr.cfg.nDim);
#pragma omp for schedule(dynamic, 1) nowait
    for (size_t t = 0; t < pr.tgs.size(); t++) {
      const TensorGridCTData& tgData = *pr.tgs[t];
      s.values.resize(tgData.tensorGrid.nGP());
      sol::forEachValue(pr.func, pr.mapped[t], s,
                        [&](const size_t i, const double v) { s.values[i] = v; });
      rulesOf(pr, tgData, nodes, methods);
      if (cachedWeights) {
        interpolateAllPoints(pr, tgData, nodes, sol::CachedWeightsRule(nodes), s, local);
      } else {
        interpolateAllPoints(pr, tgData, nodes, sol::ProductionRule(methods), s, local);
      }
    }
#pragma omp critical
    for (size_t q = 0; q < p; q++) out[q] += local[q];
  }
  return seconds(start);
}

/// sol_api (repeat = p, one region per query point) / sol_multi (repeat = 1): integrand only.
double runGlobalSol(const Problem& pr, const size_t repeat, double& sink) {
  const auto start = std::chrono::steady_clock::now();
  for (size_t r = 0; r < repeat; r++) {
    double sum = 0.0;
#pragma omp parallel reduction(+ : sum)
    {
      sol::GlobalScratch s(pr.cfg.nDim);
#pragma omp for schedule(guided) nowait
      for (size_t t = 0; t < pr.tgs.size(); t++) {
        sol::forEachValue(pr.func, pr.mapped[t], s, [&](size_t, const double v) { sum += v; });
      }
    }
    sink += sum;
  }
  return seconds(start);
}

/*******************************************************************************
Global interpolation: every distinct point once per call (nested node families)
*******************************************************************************/

/**
 * Index of the distinct points of a complete sparse grid with a nested node family (as a deltas
 * sparse grid): per dimension and level, every 1D node is mapped to the level where it appears
 * first (its delta level) and its rank among the new nodes of that level; the values of the
 * delta tensor grid of a level multi-index lambda (all lambda with |lambda| <= L) are stored
 * contiguously (dimension 0 fastest) at offset[dense(lambda)].
 */
struct DeltaIndex {
  size_t nDim = 0, radix = 0, nValues = 0;
  std::vector<std::vector<std::vector<std::pair<size_t, size_t>>>> nodeToDelta;  ///< [j][l][i]
  std::vector<std::vector<std::vector<double>>> newMapped;  ///< [j][l]: new nodes on the domain
  std::vector<size_t> offset;               ///< Dense over (L+1)^d; SIZE_MAX: no delta grid.
  std::vector<std::vector<size_t>> deltas;  ///< The lambdas with |lambda| <= L.
};

void addLevelNodes(const Problem& pr, std::vector<std::vector<std::vector<double>>>& unitPerLvl,
                   std::vector<std::vector<std::vector<double>>>& mappedPerLvl) {
  for (size_t t = 0; t < pr.tgs.size(); t++) {
    for (size_t j = 0; j < pr.cfg.nDim; j++) {
      unitPerLvl[j][pr.tgs[t]->mi[j]] = pr.unit[t][j];
      mappedPerLvl[j][pr.tgs[t]->mi[j]] = pr.mapped[t][j];
    }
  }
}

void indexDimension(DeltaIndex& ix, const size_t j, const std::vector<std::vector<double>>& unit,
                    const std::vector<std::vector<double>>& mapped) {
  std::vector<std::pair<uint64_t, std::pair<size_t, size_t>>> seen;
  ix.newMapped[j].assign(ix.radix, {});
  for (size_t l = 0; l < ix.radix; l++) {
    for (size_t i = 0; i < unit[l].size(); i++) {
      const uint64_t bits = std::bit_cast<uint64_t>(unit[l][i]);
      auto it =
          std::find_if(seen.begin(), seen.end(), [&](const auto& e) { return e.first == bits; });
      if (it == seen.end()) {
        seen.push_back({bits, {l, ix.newMapped[j][l].size()}});
        ix.newMapped[j][l].push_back(mapped[l][i]);
        it = seen.end() - 1;
      }
      ix.nodeToDelta[j][l].push_back(it->second);
    }
  }
}

void enumerateDeltas(DeltaIndex& ix, std::vector<size_t>& lambda, const size_t j,
                     const size_t budget) {
  if (j == ix.nDim) {
    size_t dense = 0, size = 1;
    for (size_t k = ix.nDim; k-- > 0;) {
      dense = dense * ix.radix + lambda[k];
      size *= ix.newMapped[k][lambda[k]].size();
    }
    ix.offset[dense] = ix.nValues;
    ix.nValues += size;
    ix.deltas.push_back(lambda);
    return;
  }
  for (size_t l = 0; l <= budget; l++) {
    lambda[j] = l;
    enumerateDeltas(ix, lambda, j + 1, budget - l);
  }
}

DeltaIndex buildDeltaIndex(const Problem& pr) {
  DeltaIndex ix;
  ix.nDim = pr.cfg.nDim;
  ix.radix = pr.cfg.maxLvl + 1;
  std::vector<std::vector<std::vector<double>>> unit(ix.nDim,
                                                     std::vector<std::vector<double>>(ix.radix));
  std::vector<std::vector<std::vector<double>>> mapped = unit;
  addLevelNodes(pr, unit, mapped);
  ix.nodeToDelta.assign(ix.nDim, std::vector<std::vector<std::pair<size_t, size_t>>>(ix.radix));
  ix.newMapped.assign(ix.nDim, {});
  for (size_t j = 0; j < ix.nDim; j++) indexDimension(ix, j, unit[j], mapped[j]);
  size_t denseSize = 1;
  for (size_t j = 0; j < ix.nDim; j++) denseSize *= ix.radix;
  ix.offset.assign(denseSize, SIZE_MAX);
  std::vector<size_t> lambda(ix.nDim);
  enumerateDeltas(ix, lambda, 0, pr.cfg.maxLvl);
  return ix;
}

/// Phase 1: the values at all distinct points (delta tensor grids in parallel).
void evaluateDistinct(const Problem& pr, const DeltaIndex& ix, std::vector<double>& values) {
#pragma omp parallel
  {
    sol::GlobalScratch s(pr.cfg.nDim);
    sol::NodesPerDim x(pr.cfg.nDim);
#pragma omp for schedule(dynamic, 1) nowait
    for (size_t d = 0; d < ix.deltas.size(); d++) {
      size_t dense = 0;
      for (size_t k = ix.nDim; k-- > 0;) {
        dense = dense * ix.radix + ix.deltas[d][k];
        x[k] = ix.newMapped[k][ix.deltas[d][k]];
      }
      double* const out = values.data() + ix.offset[dense];
      sol::forEachValue(pr.func, x, s, [&](const size_t i, const double v) { out[i] = v; });
    }
  }
}

/// Phase 2: the values of TG t in index order (dimension 0 fastest), gathered from @p values.
void gatherTG(const Problem& pr, const DeltaIndex& ix, const size_t t,
              const std::vector<double>& values, std::vector<size_t>& idx,
              std::vector<double>& out) {
  const TensorGridCTData& tg = *pr.tgs[t];
  const size_t nDim = ix.nDim;
  const auto& dim0 = ix.nodeToDelta[0][tg.mi[0]];
  out.resize(tg.tensorGrid.nGP());
  std::fill(idx.begin(), idx.end(), 0);
  size_t pos = 0;
  while (true) {
    size_t lambdaRest = 0, rest = 0;  // dense lambda of dims >= 1, local index of dims >= 1
    for (size_t k = nDim; k-- > 1;) {
      const std::pair<size_t, size_t>& e = ix.nodeToDelta[k][tg.mi[k]][idx[k]];
      lambdaRest = lambdaRest * ix.radix + e.first;
      rest = rest * ix.newMapped[k][e.first].size() + e.second;
    }
    for (const std::pair<size_t, size_t>& e : dim0) {
      const size_t off = ix.offset[e.first + ix.radix * lambdaRest];
      out[pos++] = values[off + e.second + ix.newMapped[0][e.first].size() * rest];
    }
    size_t k = 1;
    for (; k < nDim; k++) {
      if (++idx[k] < pr.unit[t][k].size()) break;
      idx[k] = 0;
    }
    if (k >= nDim) break;
  }
}

/// dedup: per call (multi = false) or once for all query points (multi = true) the distinct
/// points evaluated once, then per TG the values gathered and the 1D collapses.
double runGlobalDedup(const Problem& pr, const DeltaIndex& ix, const bool multi,
                      std::vector<double>& values, std::vector<double>& out) {
  const size_t p = pr.points.size();
  std::fill(out.begin(), out.end(), 0.0);
  const auto start = std::chrono::steady_clock::now();
  for (size_t call = 0; call < (multi ? 1 : p); call++) {
    evaluateDistinct(pr, ix, values);
#pragma omp parallel
    {
      sol::GlobalScratch s(pr.cfg.nDim);
      std::vector<double> local(p, 0.0);
      std::vector<size_t> idx(pr.cfg.nDim);
      sol::NodesPerDim nodes(pr.cfg.nDim);
      std::vector<const cg::InterpolationMethod*> methods(pr.cfg.nDim);
#pragma omp for schedule(dynamic, 1) nowait
      for (size_t t = 0; t < pr.tgs.size(); t++) {
        gatherTG(pr, ix, t, values, idx, s.values);
        rulesOf(pr, *pr.tgs[t], nodes, methods);
        const size_t n0 = nodes[0].size(), cnt = s.values.size() / n0;
        for (size_t q = multi ? 0 : call; q < (multi ? p : call + 1); q++) {
          s.slice.resize(n0);
          s.work.resize(cnt);
          for (size_t j = 0; j < cnt; j++) {
            std::copy_n(s.values.data() + j * n0, n0, s.slice.begin());
            s.work[j] = methods[0]->interpolate(pr.normPoints[q][0], nodes[0], s.slice);
          }
          local[q] += pr.tgs[t]->coefficient *
                      sol::collapseLaterDims(nodes, sol::ProductionRule(methods), pr.normPoints[q],
                                             cnt, s);
        }
      }
#pragma omp critical
      for (size_t q = 0; q < p; q++) out[q] += local[q];
    }
  }
  return seconds(start);
}

/*******************************************************************************
Multilinear interpolation
*******************************************************************************/

/// kernel (weights) or sol_api (integrand only) of the p single-point calls.
double runLinear(const Problem& pr, const bool weights, std::vector<double>& out) {
  const auto start = std::chrono::steady_clock::now();
  for (size_t q = 0; q < pr.points.size(); q++) {
    double sum = 0.0;
#pragma omp parallel reduction(+ : sum)
    {
      sol::LinScratch s(pr.cfg.nDim);
#pragma omp for schedule(static) nowait
      for (size_t t = 0; t < pr.tgs.size(); t++) {
        const double v = sol::linearKernelTG(pr.func, pr.tgs[t]->tensorGrid, pr.normPoints[q],
                                             pr.domain, weights, s);
        sum += weights ? pr.tgs[t]->coefficient * v : v;
      }
    }
    out[q] = sum;
  }
  return seconds(start);
}

uint64_t hashCorner(const sol::LinScratch& s, const size_t corner, const size_t nDim) {
  uint64_t h = 0x9E3779B97F4A7C15ULL;
  for (size_t k = 0; k < nDim; k++) {
    const double x = ((corner >> k) & 1) ? s.nb[k].second : s.nb[k].first;
    h ^= std::bit_cast<uint64_t>(x) + 0x9E3779B97F4A7C15ULL + (h << 6) + (h >> 2);
    h *= 0xBF58476D1CE4E5B9ULL;
    h ^= h >> 31;
  }
  return h;
}

/// Distinct corners per TG over all query points, summed over the TGs.
size_t distinctCornersPerTG(const Problem& pr) {
  size_t total = 0;
#pragma omp parallel reduction(+ : total)
  {
    sol::LinScratch s(pr.cfg.nDim);
    std::unordered_set<uint64_t> seen;
#pragma omp for schedule(dynamic, 1) nowait
    for (size_t t = 0; t < pr.tgs.size(); t++) {
      seen.clear();
      for (const DataVector& q : pr.normPoints) {
        pr.tgs[t]->tensorGrid.getNeighborsForLinInterpolation(q, s.nb);
        for (size_t c = 0; c < (size_t{1} << pr.cfg.nDim); c++) {
          seen.insert(hashCorner(s, c, pr.cfg.nDim));
        }
      }
      total += seen.size();
    }
  }
  return total;
}

/*******************************************************************************
Driver
*******************************************************************************/

double maxAbsDiff(const std::vector<double>& a, const std::vector<double>& b) {
  double m = 0.0;
  for (size_t i = 0; i < a.size(); i++) m = std::max(m, std::abs(a[i] - b[i]));
  return m;
}

/// Nodes the operator regenerates per call (genNodes per TG and dimension) and the coordinates
/// the integrand consumes per call (d per evaluation): for cc / cheb1 both cost one cos each.
std::pair<size_t, size_t> nodeWork(const Problem& pr) {
  size_t regenerated = 0, coordinates = 0;
  for (const TensorGridCTData* tg : pr.tgs) {
    for (size_t k = 0; k < pr.cfg.nDim; k++) regenerated += tg->tensorGrid.getGPCntPerDim()[k];
    coordinates += pr.cfg.nDim * tg->tensorGrid.nGP();
  }
  return {regenerated, coordinates};
}

void printStructure(const Problem& pr) {
  const size_t nTG = pr.tgs.size(), p = pr.points.size();
  size_t sumGP = 0, maxGP = 0;
  for (const TensorGridCTData* tg : pr.tgs) {
    sumGP += tg->tensorGrid.nGP();
    maxGP = std::max(maxGP, tg->tensorGrid.nGP());
  }
  const double threads = omp_get_max_threads();
  const double share = static_cast<double>(maxGP) / (static_cast<double>(sumGP) / threads);
  std::printf("| %s | %zu | %zu | %.2f | ", pr.cfg.name().c_str(), nTG, sumGP, share);
  if (pr.cfg.linear) {
    const size_t perCall = nTG << pr.cfg.nDim;
    const size_t distinct = distinctCornersPerTG(pr);
    std::printf("%zu | %zu | %zu | %.2f | - |\n", perCall, perCall * p, distinct,
                static_cast<double>(perCall * p) / static_cast<double>(distinct));
  } else {
    const std::pair<size_t, size_t> work = nodeWork(pr);
    std::printf("%zu | %zu | %zu | %.2f | %zu / %zu = %.4f |\n", sumGP, sumGP * p, sumGP,
                static_cast<double>(p), work.first, work.second,
                static_cast<double>(work.first) / static_cast<double>(work.second));
  }
  std::fflush(stdout);
}

/// Rows of the deduplication table (printed after the timing table).
std::vector<std::string> dedupRows;

/// Median times of the variants (one entry per repetition).
struct Times {
  std::vector<double> api, kernel, kernelW, solApi, multi, multiW, solMulti, cached, dedup,
      multiDedup;
};

/// One interleaved round of all variants; lin: multilinear (kernel and sol_api only).
void timeRound(const Problem& pr, const DeltaIndex* ix, Times& t,
               std::vector<std::vector<double>>& res, std::vector<double>& values, double& hit,
               double& sink) {
  const bool lin = pr.cfg.linear, generic = !lin && pr.cfg.nodes != "cc";
  const size_t p = pr.points.size();
  const auto start = std::chrono::steady_clock::now();
  res[0] = runApi(pr, p);
  t.api.push_back(seconds(start));
  t.kernel.push_back(lin ? runLinear(pr, true, res[1]) : runGlobalKernel(pr, false, res[1]));
  t.kernelW.push_back(generic ? runGlobalKernel(pr, true, res[2]) : 0.0);
  t.solApi.push_back(lin ? runLinear(pr, false, res[5]) : runGlobalSol(pr, p, sink));
  t.multi.push_back(lin ? 0.0 : runGlobalMulti(pr, false, res[3]));
  t.multiW.push_back(generic ? runGlobalMulti(pr, true, res[4]) : 0.0);
  t.solMulti.push_back(lin ? 0.0 : runGlobalSol(pr, 1, sink));
  t.cached.push_back(runApiCached(pr, res[6], hit));
  t.dedup.push_back(ix ? runGlobalDedup(pr, *ix, false, values, res[7]) : 0.0);
  t.multiDedup.push_back(ix ? runGlobalDedup(pr, *ix, true, values, res[8]) : 0.0);
}

std::string fmtTime(const std::vector<double>& t, const double api, const bool show) {
  if (!show || t.empty()) return "-";
  char buf[64];
  std::snprintf(buf, sizeof(buf), "%.4f (%.2f)", median(t), api / median(t));
  return buf;
}

std::string fmtDiff(const std::vector<double>& a, const std::vector<double>& b, const bool show) {
  if (!show) return "-";
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.1e", maxAbsDiff(a, b));
  return buf;
}

void timeCase(const Problem& pr, const int reps) {
  const size_t p = pr.points.size();
  const bool lin = pr.cfg.linear, generic = !lin && pr.cfg.nodes != "cc";
  std::vector<std::vector<double>> res(9, std::vector<double>(p));
  const bool nested = !lin && pr.cfg.nodes != "cheb1";
  const std::optional<DeltaIndex> ix =
      nested ? std::optional<DeltaIndex>(buildDeltaIndex(pr)) : std::nullopt;
  std::vector<double> values(ix ? ix->nValues : 0);
  Times warm, t;
  double hit = 0.0, sink = 0.0;
  timeRound(pr, ix ? &*ix : nullptr, warm, res, values, hit, sink);
  for (int r = 0; r < reps; r++) timeRound(pr, ix ? &*ix : nullptr, t, res, values, hit, sink);
  const double api = median(t.api);
  std::printf("| %s | %zu | %.4f | %s | %s | %.4f (%.2f) | %s | %s | %s | %s [%.3f] | %s / %s / %s "
              "/ %s |\n",
              pr.cfg.name().c_str(), p, api, fmtTime(t.kernel, api, true).c_str(),
              fmtTime(t.kernelW, api, generic).c_str(), median(t.solApi), median(t.solApi) / api,
              fmtTime(t.multi, api, !lin).c_str(), fmtTime(t.multiW, api, generic).c_str(),
              lin ? "-" : std::to_string(median(t.solMulti)).substr(0, 6).c_str(),
              fmtTime(t.cached, api, pr.cached != nullptr).c_str(), hit,
              fmtDiff(res[0], res[1], true).c_str(), fmtDiff(res[0], res[2], generic).c_str(),
              fmtDiff(res[0], res[3], !lin).c_str(), fmtDiff(res[0], res[6], true).c_str());
  if (ix) {
    dedupRows.push_back(pr.cfg.name() + " | " + std::to_string(ix->nValues) + " | " +
                        fmtTime(t.dedup, api, true) + " | " + fmtTime(t.multiDedup, api, true) +
                        " | " + fmtDiff(res[0], res[7], true) + " / " +
                        fmtDiff(res[0], res[8], true));
  }
  std::fflush(stdout);
  if (sink == 42.0) std::printf("#\n");
}

}  // namespace

int main(int argc, char** argv) {
  int reps = 5;
  size_t nPoints = SIZE_MAX, allocProbe = 0;
  std::string filter;
  for (int i = 1; i + 1 < argc; i += 2) {
    const std::string arg = argv[i];
    if (arg == "--reps") reps = std::atoi(argv[i + 1]);
    if (arg == "--filter") filter = argv[i + 1];
    if (arg == "--points") nPoints = std::strtoull(argv[i + 1], nullptr, 10);
    if (arg == "--alloc-probe") allocProbe = std::strtoull(argv[i + 1], nullptr, 10);
  }

  std::vector<std::unique_ptr<Problem>> problems;
  for (const Config& c : CONFIGS) {
    if (c.name().find(filter) == std::string::npos) continue;
    problems.push_back(std::make_unique<Problem>(c, allocProbe > 0 ? allocProbe : nPoints));
  }

  if (allocProbe > 0) {
    double sum = 0.0;
    for (const auto& pr : problems) {
      for (const double v : runApi(*pr, pr->points.size())) sum += v;
    }
    std::printf("alloc probe: %zu calls per case, sum %.6f\n", allocProbe, sum);
    return 0;
  }

  std::printf("threads: %d, repetitions: %d (median, after 1 warm-up)\n\n", omp_get_max_threads(),
              reps);
  std::printf(
      "| case | TGs | sum nGP | max nGP / (sum/threads) | evals per call | evals p calls | "
      "evals multi-point | p calls / multi | regenerated nodes / integrand coordinates per call "
      "|\n|---|---|---|---|---|---|---|---|---|\n");
  for (const auto& pr : problems) printStructure(*pr);

  std::printf(
      "\n| case | p | api [s] | kernel [s] (api/x) | kernel cached weights | sol_api [s] (share "
      "of api) | multi | multi cached weights | sol_multi [s] | api + cache [hit rate] | max diff "
      "kernel / kernel w / multi / cache |\n|---|---|---|---|---|---|---|---|---|---|---|\n");
  for (const auto& pr : problems) timeCase(*pr, reps);
  std::printf(
      "\n| case (nested nodes) | distinct points per call | dedup [s] (api/x) | multi + dedup [s] "
      "(api/x) | max diff dedup / multi + dedup |\n|---|---|---|---|---|\n");
  for (const std::string& row : dedupRows) std::printf("| %s |\n", row.c_str());
  return 0;
}
