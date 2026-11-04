// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-17: ThreadSanitizer driver of the block evaluation of the cached source functions
// (std::thread instead of OpenMP, whose runtime is not instrumented). Built together with the
// cache and evaluator sources, see tsan.sh. 16 threads evaluate overlapping blocks of points
// (misc::PointBlock) and matrices of points of one cached function each (per-point and batch
// callables, scalar and vector-valued; MemoryOnly, TemporaryStorage with memory limit 0, i.e.
// every value spilled to the file, and Adaptive with every evaluation timed), while one thread
// reads the statistics and calls the end-of-operator hook. Afterwards the values are checked.

#include <atomic>
#include <cmath>
#include <cstdio>
#include <memory>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_timer.hpp>
#include <sgpp/combigrid/miscellaneous/source_function_evaluation/point_block.hpp>
#include <span>
#include <thread>
#include <vector>

using sgpp::base::DataMatrix;
using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
using sgpp::combigrid::CachePolicy;
using sgpp::combigrid::GPCntType;
using sgpp::combigrid::SourceFuncCacheConfig;
using sgpp::combigrid::misc::PointBlock;

namespace {

constexpr size_t N_THREADS = 16;
constexpr size_t M = 2;
std::atomic<size_t> errors{0};

double f(const DataVector& x) { return std::exp(x[0]) + 2.0 * x[1] - x[2]; }

void fBatch(const DataMatrix& points, const std::span<double> values) {
  DataVector x(points.getNcols());
  for (size_t row = 0; row < points.getNrows(); row++) {
    points.getRow(row, x);
    values[row] = f(x);
  }
}

void vBatch(const DataMatrix& points, const std::span<double> values) {
  DataVector x(points.getNcols());
  for (size_t row = 0; row < points.getNrows(); row++) {
    points.getRow(row, x);
    values[row * M] = f(x);
    values[row * M + 1] = -f(x);
  }
}

/// Thread t: blocks of 4 x 3 x 3 points whose nodes overlap with those of the other threads.
template <typename Func>
void work(const Func& func, const size_t m, const size_t t) {
  std::vector<double> rowNodes(4), nodes1(3), nodes2(3);
  const size_t outerDims[2] = {1, 2};
  const GPCntType nOuterNodes[2] = {3, 3};
  std::vector<double> values(36 * m);
  for (size_t round = 0; round < 40; round++) {
    for (size_t i = 0; i < 4; i++) rowNodes[i] = 0.01 * static_cast<double>((t + round + i) % 23);
    for (size_t i = 0; i < 3; i++) {
      nodes1[i] = 0.1 * static_cast<double>((round + i) % 7);
      nodes2[i] = 0.2 * static_cast<double>(i);
    }
    const double* outerNodes[2] = {nodes1.data(), nodes2.data()};
    DataVector point(3);
    const PointBlock block{point, 0, rowNodes.data(), 4, 2, outerDims, nOuterNodes, outerNodes};
    func.evaluateBlock(block, values.data());

    DataMatrix matrix(36, 3);
    size_t p = 0;
    sgpp::combigrid::misc::forEachPointOfBlock(block, [&](const DataVector& x) {
      matrix.setRow(p, x);
      if (std::abs(values[p * m] - f(x)) > 1e-14) errors++;
      p++;
    });
    func.evaluate(matrix, std::span<double>(values.data(), 36 * m));
    for (size_t row = 0; row < 36; row++) {
      DataVector x(3);
      matrix.getRow(row, x);
      if (std::abs(values[row * m] - f(x)) > 1e-14) errors++;
    }
  }
}

template <typename Func>
void run(const char* name, const Func& func, const size_t m) {
  std::atomic<bool> done{false};
  std::thread hook([&]() {
    while (!done.load()) {
      func.finishOperatorCall();
      (void)func.statistics();
    }
  });
  std::vector<std::thread> threads;
  for (size_t t = 0; t < N_THREADS; t++) threads.emplace_back([&, t]() { work(func, m, t); });
  for (std::thread& thread : threads) thread.join();
  done = true;
  hook.join();
  std::printf("%s: lookups %zu hits %zu\n", name, func.statistics().lookups,
              func.statistics().hits);
}

}  // namespace

int main() {
  sgpp::combigrid::misc::adaptive_caching::CostTimer::overrideSamplingShift(0);
  const std::vector<std::pair<const char*, SourceFuncCacheConfig>> configs{
      {"memory", SourceFuncCacheConfig{}},
      {"temporary", SourceFuncCacheConfig{.memoryLimit = 0,
                                          .policy = CachePolicy::TemporaryStorage}},
      {"adaptive", SourceFuncCacheConfig{.policy = CachePolicy::Adaptive}}};
  for (const auto& [name, config] : configs) {
    run(name, CachedSourceFunc(f, config), 1);
    run(name, CachedSourceFunc(CachedSourceFunc::BatchFunc(fBatch), config), 1);
    run(name, CachedVectorSourceFunc(CachedVectorSourceFunc::BatchFunc(vBatch), M, config), M);
  }
  std::printf("errors: %zu\n", errors.load());
  return errors.load() == 0 ? 0 : 1;
}
