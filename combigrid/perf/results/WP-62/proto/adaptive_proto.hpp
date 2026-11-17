// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_proto.hpp
 * @brief Exploration prototype of a dimension-adaptive combination technique (quadrature).
 *
 * Not production code: one header, public members, no input validation. It uses the library's
 * node families, quadrature rules and source functions, and keeps its own incremental data
 * structures (sparse multi-index set, 1D rule table, value store).
 */

#pragma once

#include <omp.h>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <queue>
#include <span>
#include <utility>
#include <vector>

namespace wp62 {

using sgpp::base::DataVector;
using sgpp::combigrid::GPCntType;
using sgpp::combigrid::Lvl2GPCntFunc;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::SourceFunc;

inline double now() {
  return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

/// One level of a 1D rule: nodes, weights and, for nested families, the hierarchical layout.
struct Level1D {
  std::vector<double> nodes;       ///< All nodes of the level.
  std::vector<double> weights;     ///< Quadrature weights of the level.
  std::vector<double> newNodes;    ///< Nodes that the previous level does not have.
  std::vector<uint32_t> newPos;    ///< Their positions in @c nodes.
  std::vector<uint32_t> posOfPrev; ///< Position in @c nodes of each node of the previous level.
  /// Per origin level m <= this level: difference weights (this level minus the previous one)
  /// of the nodes that are new at level m.
  std::vector<std::vector<double>> diffWeights;
  /// Per origin level m: positions in @c nodes of the nodes that are new at level m.
  std::vector<std::vector<uint32_t>> posOfOrigin;
};

/// Lazily extended table of the levels of one 1D node family (shared by all dimensions).
struct Rule1D {
  NodeGenFunc* gen = nullptr;
  Lvl2GPCntFunc cnt = nullptr;
  LvlType boundaryOffset = 0;
  bool nested = true;
  std::vector<std::unique_ptr<Level1D>> levels;

  size_t count(const LvlType lvl) const { return levels[lvl]->nodes.size(); }
  size_t newCount(const LvlType lvl) const { return levels[lvl]->newNodes.size(); }

  /// Extends the table up to @p maxLvl (serial sections only).
  void ensure(const LvlType maxLvl) {
    while (levels.size() <= maxLvl) addLevel();
  }

  void addLevel() {
    const LvlType lvl = static_cast<LvlType>(levels.size());
    auto level = std::make_unique<Level1D>();
    const bool boundary = lvl >= boundaryOffset;
    const GPCntType nNodes = cnt(lvl) + (boundary ? 2 : 0);
    DataVector nodes(nNodes);
    gen->genNodes(nNodes, nodes, boundary);
    level->nodes.assign(nodes.begin(), nodes.end());
    level->weights.resize(level->nodes.size());
    gen->getQuadRule(static_cast<GPCntType>(level->nodes.size()))
        ->genWeightsInplace(level->weights);
    if (lvl == 0) {
      level->newNodes = level->nodes;
      for (uint32_t i = 0; i < level->nodes.size(); i++) level->newPos.push_back(i);
    } else {
      layOutHierarchy(*level, *levels[lvl - 1]);
    }
    fillOrigins(*level, lvl);
    levels.push_back(std::move(level));
  }

  /// Finds the nodes of the previous level in the new one (nested families) and the new nodes.
  void layOutHierarchy(Level1D& level, const Level1D& prev) {
    std::vector<char> isOld(level.nodes.size(), 0);
    for (const double node : prev.nodes) {  // node lists are ascending
      uint32_t found = UINT32_MAX;
      const auto it = std::ranges::lower_bound(level.nodes, node - 1e-13);
      if (it != level.nodes.end() && std::abs(*it - node) <= 1e-13) {
        found = static_cast<uint32_t>(it - level.nodes.begin());
      }
      if (found == UINT32_MAX) {
        nested = false;
      } else {
        isOld[found] = 1;
      }
      level.posOfPrev.push_back(found);
    }
    for (uint32_t i = 0; i < level.nodes.size(); i++) {
      if (!isOld[i]) {
        level.newPos.push_back(i);
        level.newNodes.push_back(level.nodes[i]);
      }
    }
  }

  void fillOrigins(Level1D& level, const LvlType lvl) {
    if (!nested) return;
    level.posOfOrigin.resize(lvl + 1);
    level.diffWeights.resize(lvl + 1);
    for (LvlType m = 0; m < lvl; m++) {
      const Level1D& prev = *levels[lvl - 1];
      for (size_t j = 0; j < prev.posOfOrigin[m].size(); j++) {
        const uint32_t prevPos = prev.posOfOrigin[m][j];
        const uint32_t pos = level.posOfPrev[prevPos];
        level.posOfOrigin[m].push_back(pos);
        level.diffWeights[m].push_back(level.weights[pos] - prev.weights[prevPos]);
      }
    }
    level.posOfOrigin[lvl] = level.newPos;
    for (const uint32_t pos : level.newPos) level.diffWeights[lvl].push_back(level.weights[pos]);
  }
};

/// Entry of a sparse multi-index: (dimension << 16) | level, level > 0, sorted by dimension.
inline uint32_t entry(const uint32_t dim, const uint32_t lvl) { return (dim << 16) | lvl; }
inline uint32_t dimOf(const uint32_t e) { return e >> 16; }
inline uint32_t lvlOf(const uint32_t e) { return e & 0xFFFFu; }

/// Append-only set of sparse multi-indices with a hash index (open addressing).
struct MISet {
  std::vector<uint32_t> pool;
  std::vector<uint32_t> offsets{0};
  std::vector<uint64_t> hashes;
  std::vector<uint32_t> table = std::vector<uint32_t>(1024, 0);

  size_t size() const { return hashes.size(); }
  std::span<const uint32_t> key(const uint32_t idx) const {
    return {pool.data() + offsets[idx], offsets[idx + 1] - offsets[idx]};
  }

  static uint64_t hashKey(const std::span<const uint32_t> key) {
    uint64_t h = 0x9E3779B97F4A7C15ull;
    for (const uint32_t e : key) {
      h ^= e;
      h *= 0xFF51AFD7ED558CCDull;
      h ^= h >> 32;
    }
    return h;
  }

  int64_t find(const std::span<const uint32_t> key) const {
    const uint64_t h = hashKey(key);
    const size_t mask = table.size() - 1;
    for (size_t slot = h & mask;; slot = (slot + 1) & mask) {
      const uint32_t stored = table[slot];
      if (stored == 0) return -1;
      const uint32_t idx = stored - 1;
      if (hashes[idx] == h && std::ranges::equal(this->key(idx), key)) return idx;
    }
  }

  uint32_t insert(const std::span<const uint32_t> key) {
    if (2 * (size() + 1) > table.size()) grow();
    const uint32_t idx = static_cast<uint32_t>(size());
    pool.insert(pool.end(), key.begin(), key.end());
    offsets.push_back(static_cast<uint32_t>(pool.size()));
    hashes.push_back(hashKey(key));
    place(idx);
    return idx;
  }

  void place(const uint32_t idx) {
    const size_t mask = table.size() - 1;
    size_t slot = hashes[idx] & mask;
    while (table[slot] != 0) slot = (slot + 1) & mask;
    table[slot] = idx + 1;
  }

  void grow() {
    table.assign(table.size() * 4, 0);
    for (uint32_t idx = 0; idx < size(); idx++) place(idx);
  }
};

/// Chunked arena of doubles with stable addresses.
struct ValueArena {
  static constexpr size_t CHUNK = size_t{1} << 18;
  std::vector<std::unique_ptr<double[]>> chunks;
  size_t used = CHUNK;
  size_t total = 0;

  double* allocate(const size_t n) {
    total += n;
    if (n > CHUNK / 4) {
      chunks.push_back(std::make_unique_for_overwrite<double[]>(n));
      double* const ptr = chunks.back().get();
      // Keep the current chunk in use: swap it back to the end.
      if (chunks.size() >= 2 && used < CHUNK) std::swap(chunks.back(), chunks[chunks.size() - 2]);
      return ptr;
    }
    if (used + n > CHUNK) {
      chunks.push_back(std::make_unique_for_overwrite<double[]>(CHUNK));
      used = 0;
    }
    double* const ptr = chunks.back().get() + used;
    used += n;
    return ptr;
  }
};

enum class State : uint8_t { Predicted, Active, Old };
enum class Store { DeltaValues, TensorGridResults };
enum class Selection { GerstnerGriebel, Lazy };
enum class Predictor { PairGeoMean, PairMax, PairMin };

struct Options {
  Selection selection = Selection::GerstnerGriebel;
  Store store = Store::DeltaValues;
  bool profit = true;         ///< Indicator divided by the work.
  double workWeight = 1.0;    ///< w of the Gerstner-Griebel safeguard (1 = error only).
  size_t maxEvals = 100000;   ///< Budget of function evaluations (work model of the store).
  double tolerance = 0.0;     ///< Stop when the sum of the active indicators is below.
  size_t buffer = 0;          ///< > 0: explore only this many non-activated dimensions.
  double bulkTheta = 1.0;     ///< Accept all candidates with priority >= theta * best.
  size_t maxBatch = 1;        ///< Largest number of candidates accepted / evaluated per step.
  Predictor predictor = Predictor::PairGeoMean;
  double safety = 1.0;        ///< Lazy: factor on predicted priorities.
  double explore = 0.0;       ///< Lazy: share of the evaluations spent on the cheapest
                              ///< unevaluated candidates regardless of their prediction.
  double traceGrowth = 1.05;  ///< A trace point whenever the evaluations grew by this factor.
  bool parallel = true;
};

struct TracePoint {
  size_t nIndices, nOld, evals, tgPoints, nPredicted, rounds;
  double eta, wall, evalTime;
  std::vector<double> estimate;
};

/// Per-thread scratch of the evaluation kernels.
struct Scratch {
  DataVector point;
  std::vector<uint32_t> dims, lvls, sizes, idx, key, mLvl;
  std::vector<const double*> ptrs;
  std::vector<double> prod;
};

/**
 * @brief Dimension-adaptive quadrature driver.
 *
 * All dimensions share one 1D rule on [0, 1]. With nested nodes and Store::DeltaValues every
 * grid point is evaluated once and kept (one block per multi-index: the points that are new in
 * every dimension); the increment of a multi-index is the sum over the blocks of all smaller
 * multi-indices with tensor products of 1D difference weights. Store::TensorGridResults
 * evaluates whole tensor grids, keeps their results and forms increments by inclusion-exclusion
 * (works for every node family).
 */
struct AdaptiveQuadrature {
  size_t nDim;
  Rule1D rule;
  std::vector<const SourceFunc*> funcs;  ///< One scalar function per output.
  Options opt;
  bool trivialLevel0 = false;  ///< Level 0 has a single node: untouched dimensions do not iterate.

  MISet mis;
  std::vector<State> state;
  std::vector<double> delta;       ///< nOut per multi-index: increment.
  std::vector<double> norm;        ///< Indicator norm of the increment.
  std::vector<double> predicted;   ///< Lazy: predicted norm (NaN if evaluated without one).
  std::vector<size_t> work;        ///< Evaluations charged for the multi-index.
  std::vector<size_t> tgPoints;    ///< Points of its tensor grid.
  std::vector<int> coeff;          ///< Combination coefficients (incremental).
  std::vector<double*> blocks;     ///< DeltaValues: nOut blocks per multi-index.
  std::vector<double> tgResults;   ///< TensorGridResults: nOut per multi-index.
  ValueArena arena;

  using HeapEntry = std::pair<double, uint32_t>;
  std::priority_queue<HeapEntry> heap;
  /// Lazy exploration: unevaluated candidates, cheapest first.
  std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> exploreHeap;
  size_t exploredWork = 0, nSurprises = 0, nVerified = 0, rounds = 0;
  double maxSurprise = 0;
  std::vector<double> estimate;
  std::vector<double> scale;  ///< Per output: |increment of the first multi-index|.
  double eta = 0, etaPredicted = 0;
  size_t evals = 0, evalTGPoints = 0, nOld = 0, nEvaluated = 0, nPredictedOpen = 0;
  size_t nCurr = 0;
  std::vector<char> activated, axisOld;
  bool budgetExhausted = false;
  double startTime = 0, evalTime = 0;
  size_t lastTraceEvals = 0;
  std::vector<TracePoint> trace;
  std::vector<Scratch> scratches;

  AdaptiveQuadrature(const size_t nDim, NodeGenFunc* gen, const Lvl2GPCntFunc cnt,
                     const LvlType boundaryOffset, std::vector<const SourceFunc*> funcs,
                     const Options& opt)
      : nDim(nDim), funcs(std::move(funcs)), opt(opt) {
    rule.gen = gen;
    rule.cnt = cnt;
    rule.boundaryOffset = boundaryOffset;
    rule.ensure(1);
    if (!rule.nested) this->opt.store = Store::TensorGridResults;
    trivialLevel0 = rule.count(0) == 1;
    estimate.assign(nOut(), 0.0);
    scale.assign(nOut(), 1.0);
    activated.assign(nDim, 0);
    axisOld.assign(nDim, 0);
    startTime = now();
    scratches.resize(static_cast<size_t>(omp_get_max_threads()));
    for (Scratch& s : scratches) {
      s.point.resize(nDim);
      for (size_t k = 0; k < nDim; k++) s.point[k] = rule.levels[0]->nodes[0];
    }
  }

  size_t nOut() const { return funcs.size(); }

  // ---------------------------------------------------------------- multi-index helpers

  uint32_t addIndex(const std::span<const uint32_t> key, const State st) {
    const uint32_t idx = mis.insert(key);
    LvlType maxLvl = 0;
    size_t nNew = 1, nTG = 1;
    for (const uint32_t e : key) maxLvl = std::max<LvlType>(maxLvl, lvlOf(e));
    rule.ensure(maxLvl);
    for (const uint32_t e : key) {
      nNew *= rule.newCount(lvlOf(e));
      nTG *= rule.count(lvlOf(e));
    }
    if (!trivialLevel0) {
      for (size_t k = key.size(); k < nDim; k++) {
        nNew *= rule.count(0);
        nTG *= rule.count(0);
      }
    }
    state.push_back(st);
    delta.resize(delta.size() + nOut(), 0.0);
    norm.push_back(0.0);
    predicted.push_back(std::numeric_limits<double>::quiet_NaN());
    work.push_back(opt.store == Store::DeltaValues ? nNew : nTG);
    tgPoints.push_back(nTG);
    coeff.push_back(0);
    blocks.resize(blocks.size() + nOut(), nullptr);
    tgResults.resize(tgResults.size() + nOut(), 0.0);
    return idx;
  }

  /// key with the level of @p dim changed by @p step (+1 / -1), written to @p out.
  static void shifted(const std::span<const uint32_t> key, const uint32_t dim, const int step,
                      std::vector<uint32_t>& out) {
    out.clear();
    bool done = false;
    for (const uint32_t e : key) {
      if (!done && dimOf(e) >= dim) {
        done = true;
        if (dimOf(e) == dim) {
          const uint32_t lvl = lvlOf(e) + static_cast<uint32_t>(step);
          if (lvl > 0) out.push_back(entry(dim, lvl));
          continue;
        }
        out.push_back(entry(dim, 1));
      }
      out.push_back(e);
    }
    if (!done) out.push_back(entry(dim, 1));
  }

  double priority(const uint32_t idx, const double value) const {
    const double relWork = static_cast<double>(work[idx]) / static_cast<double>(work[0]);
    const double errorTerm = opt.profit ? value / relWork : value;
    const double w = opt.workWeight;
    if (w >= 1.0) return errorTerm;
    const double relPoints = static_cast<double>(tgPoints[idx]) / static_cast<double>(tgPoints[0]);
    return std::max(w * errorTerm, (1.0 - w) / relPoints);
  }

  // ---------------------------------------------------------------- evaluation kernels

  /// Sum over a tensor block of values times the tensor product of per-dimension weights.
  static double tensorDot(const double* values, const size_t n, const uint32_t* sizes,
                          const double* const* weights, Scratch& s) {
    if (n == 0) return values[0];
    const uint32_t n0 = sizes[0];
    const double* const w0 = weights[0];
    s.idx.assign(n, 0);
    s.prod.resize(n + 1);
    s.prod[n] = 1.0;
    for (size_t k = n - 1; k >= 1; k--) s.prod[k] = s.prod[k + 1] * weights[k][0];
    double sum = 0;
    while (true) {
      double row = 0;
      for (uint32_t i = 0; i < n0; i++) row += w0[i] * values[i];
      sum += s.prod[1] * row;
      values += n0;
      size_t k = 1;
      for (; k < n; k++) {
        if (++s.idx[k] < sizes[k]) break;
        s.idx[k] = 0;
      }
      if (k == n) return sum;
      for (size_t j = k + 1; j-- > 1;) s.prod[j] = s.prod[j + 1] * weights[j][s.idx[j]];
    }
  }

  /// Evaluates one function at the tensor product of the given 1D node lists.
  static void evaluateTensor(const SourceFunc& func, const size_t n, const uint32_t* dims,
                             const uint32_t* sizes, const double* const* nodes, double* out,
                             Scratch& s) {
    DataVector& point = s.point;
    s.idx.assign(n, 0);
    for (size_t k = 0; k < n; k++) point[dims[k]] = nodes[k][0];
    if (n == 0) {
      out[0] = func.evaluate(point);
      return;
    }
    const uint32_t d0 = dims[0], n0 = sizes[0];
    const double* const nodes0 = nodes[0];
    while (true) {
      for (uint32_t i = 0; i < n0; i++) {
        point[d0] = nodes0[i];
        *out++ = func.evaluate(point);
      }
      size_t k = 1;
      for (; k < n; k++) {
        if (++s.idx[k] < sizes[k]) {
          point[dims[k]] = nodes[k][s.idx[k]];
          break;
        }
        s.idx[k] = 0;
        point[dims[k]] = nodes[k][0];
      }
      if (k == n) return;
    }
  }

  /// Dimensions a multi-index iterates over and their levels (into the scratch).
  void iterDims(const std::span<const uint32_t> key, Scratch& s) const {
    s.dims.clear();
    s.lvls.clear();
    if (trivialLevel0) {
      for (const uint32_t e : key) {
        s.dims.push_back(dimOf(e));
        s.lvls.push_back(lvlOf(e));
      }
      return;
    }
    size_t pos = 0;
    for (uint32_t dim = 0; dim < nDim; dim++) {
      s.dims.push_back(dim);
      if (pos < key.size() && dimOf(key[pos]) == dim) {
        s.lvls.push_back(lvlOf(key[pos++]));
      } else {
        s.lvls.push_back(0);
      }
    }
  }

  void resetPoint(Scratch& s) const {
    for (const uint32_t dim : s.dims) s.point[dim] = rule.levels[0]->nodes[0];
  }

  /// Nested nodes: evaluates the new points of the multi-index, then forms its increment from
  /// the stored blocks of all smaller multi-indices.
  void evaluateDeltaValues(const uint32_t idx, Scratch& s) {
    const std::span<const uint32_t> key = mis.key(idx);
    iterDims(key, s);
    const size_t n = s.dims.size();
    s.sizes.resize(n);
    s.ptrs.resize(n);
    for (size_t k = 0; k < n; k++) {
      s.sizes[k] = static_cast<uint32_t>(rule.newCount(s.lvls[k]));
      s.ptrs[k] = rule.levels[s.lvls[k]]->newNodes.data();
    }
    for (size_t o = 0; o < nOut(); o++) {
      evaluateTensor(*funcs[o], n, s.dims.data(), s.sizes.data(), s.ptrs.data(),
                     blocks[idx * nOut() + o], s);
    }
    resetPoint(s);
    // Odometer over all m <= levels.
    s.mLvl.assign(n, 0);
    std::vector<double> sums(nOut(), 0.0);
    while (true) {
      s.key.clear();
      double constant = 1.0;
      size_t nIter = 0;
      for (size_t k = 0; k < n; k++) {
        if (s.mLvl[k] > 0) s.key.push_back(entry(s.dims[k], s.mLvl[k]));
        const std::vector<double>& dw = rule.levels[s.lvls[k]]->diffWeights[s.mLvl[k]];
        if (dw.size() == 1) {
          constant *= dw[0];
        } else {
          s.sizes[nIter] = static_cast<uint32_t>(dw.size());
          s.ptrs[nIter++] = dw.data();
        }
      }
      const int64_t m = mis.find(s.key);
      for (size_t o = 0; o < nOut(); o++) {
        sums[o] += constant * tensorDot(blocks[static_cast<size_t>(m) * nOut() + o], nIter,
                                        s.sizes.data(), s.ptrs.data(), s);
      }
      size_t k = 0;
      for (; k < n; k++) {
        if (++s.mLvl[k] <= s.lvls[k]) break;
        s.mLvl[k] = 0;
      }
      if (k == n) break;
    }
    for (size_t o = 0; o < nOut(); o++) delta[idx * nOut() + o] = sums[o];
  }

  /// Any nodes: evaluates the whole tensor grid; the increment follows by inclusion-exclusion
  /// from the stored results of the backward neighbourhood.
  void evaluateTensorGridResult(const uint32_t idx, Scratch& s) {
    const std::span<const uint32_t> key = mis.key(idx);
    iterDims(key, s);
    const size_t n = s.dims.size();
    s.sizes.resize(n);
    s.ptrs.resize(n);
    std::vector<const double*> weights(n);
    size_t nPoints = 1;
    for (size_t k = 0; k < n; k++) {
      s.sizes[k] = static_cast<uint32_t>(rule.count(s.lvls[k]));
      s.ptrs[k] = rule.levels[s.lvls[k]]->nodes.data();
      weights[k] = rule.levels[s.lvls[k]]->weights.data();
      nPoints *= s.sizes[k];
    }
    std::vector<double> values(nPoints);
    for (size_t o = 0; o < nOut(); o++) {
      evaluateTensor(*funcs[o], n, s.dims.data(), s.sizes.data(), s.ptrs.data(), values.data(), s);
      tgResults[idx * nOut() + o] = tensorDot(values.data(), n, s.sizes.data(), weights.data(), s);
    }
    resetPoint(s);
    // Inclusion-exclusion over the subsets of the non-zero dimensions.
    const size_t nnz = key.size();
    std::vector<double> sums(nOut(), 0.0);
    for (size_t mask = 0; mask < (size_t{1} << nnz); mask++) {
      s.key.clear();
      for (size_t k = 0; k < nnz; k++) {
        const uint32_t lvl = lvlOf(key[k]) - ((mask >> k) & 1u);
        if (lvl > 0) s.key.push_back(entry(dimOf(key[k]), lvl));
      }
      const int64_t m = mis.find(s.key);
      const double sign = (__builtin_popcountll(mask) & 1) ? -1.0 : 1.0;
      for (size_t o = 0; o < nOut(); o++) {
        sums[o] += sign * tgResults[static_cast<size_t>(m) * nOut() + o];
      }
    }
    for (size_t o = 0; o < nOut(); o++) delta[idx * nOut() + o] = sums[o];
  }

  /// Evaluates a batch of new multi-indices (in parallel) within the budget; returns the number
  /// of evaluated ones (a prefix of the batch).
  size_t evaluateBatch(const std::vector<uint32_t>& batch) {
    size_t nFit = 0;
    size_t batchWork = 0;
    for (; nFit < batch.size(); nFit++) {
      if (evals + batchWork + work[batch[nFit]] > opt.maxEvals) {
        budgetExhausted = true;
        break;
      }
      batchWork += work[batch[nFit]];
    }
    if (opt.store == Store::DeltaValues) {
      for (size_t b = 0; b < nFit; b++) {
        for (size_t o = 0; o < nOut(); o++) {
          blocks[batch[b] * nOut() + o] = arena.allocate(work[batch[b]]);
        }
      }
    }
    rounds += nFit > 0;
    const double t0 = now();
#pragma omp parallel for schedule(dynamic) if (opt.parallel && batchWork >= 4096 && nFit > 1)
    for (size_t b = 0; b < nFit; b++) {
      Scratch& s = scratches[static_cast<size_t>(omp_get_thread_num())];
      if (opt.store == Store::DeltaValues) {
        evaluateDeltaValues(batch[b], s);
      } else {
        evaluateTensorGridResult(batch[b], s);
      }
    }
    evalTime += now() - t0;
    for (size_t b = 0; b < nFit; b++) finishEvaluation(batch[b]);
    return nFit;
  }

  /// Serial part after the evaluation of a multi-index: norm, estimate, coefficients, heap.
  void finishEvaluation(const uint32_t idx) {
    const double* const d = &delta[idx * nOut()];
    if (nEvaluated == 0) {
      for (size_t o = 0; o < nOut(); o++) scale[o] = d[o] != 0 ? std::abs(d[o]) : 1.0;
    }
    double value = 0;
    for (size_t o = 0; o < nOut(); o++) {
      estimate[o] += d[o];
      value = std::max(value, std::abs(d[o]) / scale[o]);
    }
    norm[idx] = value;
    if (state[idx] == State::Predicted) nPredictedOpen--;
    if (!std::isnan(predicted[idx])) {
      etaPredicted -= predicted[idx];
      nVerified++;
      const double ratio = value / std::max(predicted[idx], 1e-300);
      maxSurprise = std::max(maxSurprise, ratio);
      nSurprises += ratio > 10.0;
    }
    state[idx] = State::Active;
    evals += work[idx];
    evalTGPoints += tgPoints[idx];
    nEvaluated++;
    eta += value;
    updateCoefficients(idx);
    heap.emplace(priority(idx, value), idx);
  }

  /// Adding a multi-index changes the coefficients of its backward box only.
  void updateCoefficients(const uint32_t idx) {
    const std::span<const uint32_t> key = mis.key(idx);
    std::vector<uint32_t>& sub = scratches[0].key;
    for (size_t mask = 0; mask < (size_t{1} << key.size()); mask++) {
      sub.clear();
      for (size_t k = 0; k < key.size(); k++) {
        const uint32_t lvl = lvlOf(key[k]) - ((mask >> k) & 1u);
        if (lvl > 0) sub.push_back(entry(dimOf(key[k]), lvl));
      }
      coeff[static_cast<size_t>(mis.find(sub))] += (__builtin_popcountll(mask) & 1) ? -1 : 1;
    }
  }

  // ---------------------------------------------------------------- selection

  bool isOld(const std::span<const uint32_t> key) const {
    const int64_t idx = mis.find(key);
    return idx >= 0 && state[static_cast<size_t>(idx)] == State::Old;
  }

  /// Admissible forward neighbours of an accepted multi-index that are not in the set yet.
  void forwardNeighbours(const uint32_t idx, std::vector<uint32_t>& out) {
    std::vector<uint32_t> fwd, back;
    const std::vector<uint32_t> key(mis.key(idx).begin(), mis.key(idx).end());
    const size_t nExplored = opt.buffer > 0 ? nCurr : nDim;
    for (uint32_t dim = 0; dim < nExplored; dim++) {
      const bool inKey = std::ranges::any_of(key, [dim](uint32_t e) { return dimOf(e) == dim; });
      if (!inKey && !axisOld[dim] && !key.empty()) continue;  // e_dim is not old: inadmissible
      shifted(key, dim, +1, fwd);
      if (mis.find(fwd) >= 0) continue;
      bool admissible = true;
      for (const uint32_t e : fwd) {
        if (dimOf(e) == dim) continue;  // fwd - e_dim is the accepted index
        shifted(fwd, dimOf(e), -1, back);
        if (!isOld(back)) {
          admissible = false;
          break;
        }
      }
      if (admissible) out.push_back(addIndex(fwd, State::Predicted));
    }
  }

  /// Predicted indicator norm of a new candidate from its evaluated backward neighbourhood:
  /// the product extrapolation |d(j-e_p)| |d(j-e_q)| / |d(j-e_p-e_q)| over the pairs of its
  /// non-zero dimensions (exact for products of 1D functions); along an axis the last ratio.
  /// Infinity if no information is available.
  double predict(const uint32_t idx) {
    const std::vector<uint32_t> key(mis.key(idx).begin(), mis.key(idx).end());
    const double inf = std::numeric_limits<double>::infinity();
    std::vector<uint32_t> a, b, c;
    if (key.size() == 1) {
      if (lvlOf(key[0]) < 2) return inf;
      shifted(key, dimOf(key[0]), -1, a);
      shifted(a, dimOf(key[0]), -1, b);
      const double na = norm[static_cast<size_t>(mis.find(a))];
      const double nb = norm[static_cast<size_t>(mis.find(b))];
      return nb > 0 ? std::min(na, na * na / nb) : inf;
    }
    double logSum = 0, lo = inf, hi = 0;
    size_t nPairs = 0;
    for (size_t p = 0; p < key.size(); p++) {
      shifted(key, dimOf(key[p]), -1, a);
      const double na = norm[static_cast<size_t>(mis.find(a))];
      for (size_t q = p + 1; q < key.size(); q++) {
        shifted(key, dimOf(key[q]), -1, b);
        shifted(a, dimOf(key[q]), -1, c);
        const double nb = norm[static_cast<size_t>(mis.find(b))];
        const double nc = norm[static_cast<size_t>(mis.find(c))];
        if (nc <= 0) return inf;
        const double est = na * nb / nc;
        logSum += std::log(std::max(est, 1e-300));
        lo = std::min(lo, est);
        hi = std::max(hi, est);
        nPairs++;
      }
    }
    if (opt.predictor == Predictor::PairMax) return hi;
    if (opt.predictor == Predictor::PairMin) return lo;
    return std::exp(logSum / static_cast<double>(nPairs));
  }

  void accept(const uint32_t idx, std::vector<uint32_t>& candidates) {
    if (state[idx] == State::Old) return;
    state[idx] = State::Old;
    nOld++;
    eta -= norm[idx];
    const std::span<const uint32_t> key = mis.key(idx);
    if (key.size() == 1 && lvlOf(key[0]) == 1) {
      const uint32_t dim = dimOf(key[0]);
      axisOld[dim] = 1;
      if (opt.buffer > 0 && !activated[dim]) {
        activated[dim] = 1;
        if (nCurr < nDim) {
          const uint32_t e = entry(static_cast<uint32_t>(nCurr++), 1);
          candidates.push_back(addIndex(std::span<const uint32_t>(&e, 1), State::Predicted));
        }
      }
    }
    forwardNeighbours(idx, candidates);
  }

  void record() {
    trace.push_back({mis.size() - nPredictedOpen, nOld, evals, evalTGPoints, nPredictedOpen, rounds,
                     eta + etaPredicted, now() - startTime, evalTime, estimate});
    lastTraceEvals = evals;
  }

  void maybeRecord() {
    if (static_cast<double>(evals) >= opt.traceGrowth * static_cast<double>(lastTraceEvals)) {
      record();
    }
  }

  /// New candidates: evaluated at once (Gerstner-Griebel, or no prediction available) or queued
  /// with their predicted priority (lazy).
  void handleCandidates(const std::vector<uint32_t>& candidates) {
    std::vector<uint32_t> toEvaluate;
    for (const uint32_t idx : candidates) {
      nPredictedOpen++;
      if (opt.selection == Selection::GerstnerGriebel) {
        toEvaluate.push_back(idx);
        continue;
      }
      const double pred = predict(idx);
      if (std::isinf(pred)) {
        toEvaluate.push_back(idx);
      } else {
        predicted[idx] = pred;
        etaPredicted += pred;
        heap.emplace(opt.safety * priority(idx, pred), idx);
        if (opt.explore > 0) exploreHeap.emplace(static_cast<double>(work[idx]), idx);
      }
    }
    evaluateBatch(toEvaluate);
  }

  void run() {
    const uint32_t root = addIndex(std::span<const uint32_t>(), State::Predicted);
    nPredictedOpen++;
    nCurr = std::min(opt.buffer, nDim);
    evaluateBatch({root});
    record();
    std::vector<uint32_t> batch, candidates;
    while (!heap.empty() && !budgetExhausted) {
      if (nOld > 0 && eta + etaPredicted <= opt.tolerance) break;
      if (opt.explore > 0 && !exploreHeap.empty() &&
          static_cast<double>(exploredWork) < opt.explore * static_cast<double>(evals)) {
        const uint32_t idx = exploreHeap.top().second;
        exploreHeap.pop();
        if (state[idx] == State::Predicted) {
          exploredWork += work[idx];
          evaluateBatch({idx});
        }
        continue;
      }
      const double best = heap.top().first;
      const State kind = state[heap.top().second];
      if (kind == State::Old) {  // stale entry of a candidate that the exploration evaluated
        heap.pop();
        continue;
      }
      batch.clear();
      while (!heap.empty() && batch.size() < opt.maxBatch &&
             state[heap.top().second] == kind &&
             (batch.empty() || heap.top().first >= opt.bulkTheta * best)) {
        batch.push_back(heap.top().second);
        heap.pop();
      }
      if (kind == State::Predicted) {
        const size_t nDone = evaluateBatch(batch);
        for (size_t b = nDone; b < batch.size(); b++) heap.emplace(0.0, batch[b]);
      } else {
        candidates.clear();
        for (const uint32_t idx : batch) accept(idx, candidates);
        handleCandidates(candidates);
      }
      maybeRecord();
    }
    record();
  }

  // ---------------------------------------------------------------- a-priori sets

  /// Evaluates the given multi-indices (sparse keys, concatenated; parents first) in chunks.
  void runOrdered(const std::vector<std::vector<uint32_t>>& keys,
                  const std::vector<size_t>& chunkEnds) {
    size_t next = 0;
    std::vector<uint32_t> batch;
    for (const size_t end : chunkEnds) {
      // Layers of equal level sum are antichains: their members can be evaluated in parallel.
      std::vector<std::pair<size_t, size_t>> order;
      for (; next < end; next++) {
        if (mis.find(keys[next]) >= 0) continue;
        size_t sum = 0;
        for (const uint32_t e : keys[next]) sum += lvlOf(e);
        order.emplace_back(sum, next);
      }
      std::ranges::stable_sort(order);
      for (size_t first = 0; first < order.size() && !budgetExhausted;) {
        batch.clear();
        size_t last = first;
        for (; last < order.size() && order[last].first == order[first].first; last++) {
          batch.push_back(addIndex(keys[order[last].second], State::Predicted));
          nPredictedOpen++;
        }
        first = last;
        const size_t nDone = evaluateBatch(batch);
        for (size_t b = 0; b < nDone; b++) {
          state[batch[b]] = State::Old;
          nOld++;
          eta -= norm[batch[b]];
        }
      }
      if (budgetExhausted) break;
      record();
    }
  }
};

}  // namespace wp62
