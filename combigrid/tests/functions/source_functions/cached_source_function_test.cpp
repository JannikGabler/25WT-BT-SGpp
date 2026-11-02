// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cached_source_function_test.cpp
 * @brief Boost.Test cases for the source-function cache (@c SourceFuncCache,
 * @c CachedSourceFunc, @c CachedVectorSourceFunc): cached results equal the uncached ones
 * bitwise for every operator, values are reused across calls and operators, statistics, memory
 * limit, NaN values, sharing, misuse and concurrency (only with the libraries of the cache), and
 * that the constructors throw exactly when SG++ was built without them. The storage policies
 * (temporary / persistent file, only with LMDB): bitwise results, persistence across caches and
 * crashes (child processes killed with SIGKILL), cleanup of temporary files, detection of
 * mismatching / truncated / corrupted / locked files, storage limit, clear, sharing and
 * concurrency.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
using sgpp::combigrid::CachePolicy;
using sgpp::combigrid::CacheStatistics;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SourceFuncCache;
using sgpp::combigrid::SourceFuncCacheConfig;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;

BOOST_AUTO_TEST_SUITE(source_function_cache_availability)

BOOST_AUTO_TEST_CASE(constructors_throw_exactly_without_the_libraries) {
  // The classes are always compiled; without Boost.Unordered and xxHash
  // (tools/defines/dependencies_defines.hpp) every constructor throws, before any evaluation.
  const auto func = [](const DataVector& x) { return x[0]; };
  const auto vectorFunc = [](const DataVector& x, const std::span<double> out) { out[0] = x[0]; };

  // The adaptive policy needs the table only (without LMDB it never uses a file).
  const SourceFuncCacheConfig adaptive{.policy = CachePolicy::Adaptive};
#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
  BOOST_CHECK_NO_THROW(SourceFuncCache());
  BOOST_CHECK_NO_THROW(CachedSourceFunc{func});
  BOOST_CHECK_NO_THROW(CachedVectorSourceFunc(vectorFunc, 1));
  BOOST_CHECK_NO_THROW(CachedSourceFunc(func, adaptive));
#else
  BOOST_CHECK_THROW(CachedSourceFunc(func, adaptive), sgpp::base::dependency_exception);
  BOOST_CHECK_THROW(SourceFuncCache(), sgpp::base::dependency_exception);
  BOOST_CHECK_THROW(CachedSourceFunc{func}, sgpp::base::dependency_exception);
  BOOST_CHECK_THROW(CachedVectorSourceFunc(vectorFunc, 1), sgpp::base::dependency_exception);
#endif
}

BOOST_AUTO_TEST_CASE(storage_policies_throw_exactly_without_lmdb) {
  // The storage policies need the table and LMDB; their constructors throw before any file is
  // touched otherwise.
  const SourceFuncCacheConfig temporary{.policy = CachePolicy::TemporaryStorage};
#if defined(SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE) && \
    defined(SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE)
  BOOST_CHECK_NO_THROW(SourceFuncCache{temporary});
#else
  BOOST_CHECK_THROW(SourceFuncCache{temporary}, sgpp::base::dependency_exception);
  const SourceFuncCacheConfig persistent{.policy = CachePolicy::PersistentStorage,
                                         .file = "/nonexistent-directory/values.cache"};
  BOOST_CHECK_THROW(SourceFuncCache{persistent}, sgpp::base::dependency_exception);
#endif
}

BOOST_AUTO_TEST_SUITE_END()

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

namespace {

constexpr size_t N_OUTPUTS = 3;

double scalarFunc(const DataVector& x) {
  double sumOfSquares = 0;
  double sum = 0;
  for (size_t k = 0; k < x.size(); k++) {
    sumOfSquares += static_cast<double>(k + 1) * x[k] * x[k];
    sum += x[k];
  }
  return std::exp(-sumOfSquares / static_cast<double>(x.size())) + std::sin(sum);
}

void vectorFunc(const DataVector& x, const std::span<double> out) {
  out[0] = scalarFunc(x);
  out[1] = 0;
  out[2] = 1;
  for (size_t k = 0; k < x.size(); k++) {
    out[1] += x[k];
    out[2] *= std::cos(x[k]);
  }
}

/// Dimension k: [-0.5 + 0.1 k, 1.0 + 0.2 k] (as the shifted domain of the perf suite).
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t k = 0; k < nDim; k++) {
    domain[k] = {-0.5 + 0.1 * static_cast<double>(k), 1.0 + 0.2 * static_cast<double>(k)};
  }
  return domain;
}

/// Wraps a scalar function and counts its evaluations (thread-safe).
class Counted {
 public:
  explicit Counted(std::function<double(const DataVector&)> func) : func_(std::move(func)) {}

  std::function<double(const DataVector&)> callable() {
    return [this](const DataVector& x) {
      count_.fetch_add(1, std::memory_order_relaxed);
      return func_(x);
    };
  }

  size_t count() const { return count_.load(); }

 private:
  std::function<double(const DataVector&)> func_;
  std::atomic<size_t> count_{0};
};

/// A sparse grid of one configuration (and its deltas sparse grid for nested node families).
struct Problem {
  Problem(NodeGenFunc* nodes, const LvlType boundary, const size_t nDim, const LvlType maxLvl,
          const HyperCubeArea& domain, const bool withQuadrature)
      : genInstr(maxLvl, nDim), quadrature(withQuadrature) {
    genInstr.setNodeGenFunc(nodes);
    genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
    genInstr.setBoundaryLevelOffset(boundary);
    genInstr.setDomain(domain);
    sg = std::make_unique<SparseGrid>(genInstr);
    if (withQuadrature) {
      dsg = std::make_unique<DeltasSparseGrid>(genInstr);
    }
    for (const double t : {0.0, 0.31, 0.77, 1.0}) {
      DataVector point(nDim);
      for (size_t k = 0; k < nDim; k++) {
        const double shifted = t + 0.07 * static_cast<double>(k);
        point[k] = domain[k].first +
                   (shifted - std::floor(shifted)) * (domain[k].second - domain[k].first);
      }
      points.push_back(point);
    }
  }

  /// Number of operator calls of @ref run.
  size_t nOperatorCalls() const { return (quadrature ? 2 : 0) + 2 * points.size(); }

  /// Results of all operators: quadrature (SG, DSG), global and linear interpolation.
  std::vector<double> run(const SourceFunc& f) const {
    std::vector<double> results;
    if (quadrature) {
      results.push_back(sgpp::combigrid::quadrature(f, *sg));
      results.push_back(sgpp::combigrid::quadrature(f, *dsg));
    }
    for (const DataVector& point : points) {
      results.push_back(sgpp::combigrid::interpolate(f, point, *sg));
      results.push_back(sgpp::combigrid::interpolateLinear(f, point, *sg));
    }
    return results;
  }

  /// As @ref run for a vector-valued function (the m results of every operator, concatenated).
  std::vector<double> run(const VectorSourceFunc& f) const {
    std::vector<double> results;
    const auto append = [&results](const DataVector& values) {
      results.insert(results.end(), values.begin(), values.end());
    };
    if (quadrature) {
      append(sgpp::combigrid::quadrature(f, *sg));
      append(sgpp::combigrid::quadrature(f, *dsg));
    }
    for (const DataVector& point : points) {
      append(sgpp::combigrid::interpolate(f, point, *sg));
      append(sgpp::combigrid::interpolateLinear(f, point, *sg));
    }
    return results;
  }

  CompleteSGGenInstr genInstr;
  bool quadrature;
  std::unique_ptr<SparseGrid> sg;
  std::unique_ptr<DeltasSparseGrid> dsg;
  std::vector<DataVector> points;
};

/// Calls @p check for every configuration of the bitwise tests (all node families, boundary
/// offsets 0 / 1, unit and shifted domains, d = 1-3).
template <typename Check>
void forEachProblem(const Check& check) {
  const std::vector<std::pair<NodeGenFunc*, bool>> families{
      {sgpp::combigrid::getEquidistantNodeGenFunc(), true},
      {sgpp::combigrid::getClenshawCurtisNodeGenFunc(), true},
      {sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc(), false}};  // no quadrature rule
  for (const auto& [nodes, withQuadrature] : families) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const size_t nDim : {1, 2, 3}) {
        for (const bool shifted : {false, true}) {
          const HyperCubeArea domain = shifted ? shiftedDomain(nDim) : HyperCubeArea(nDim, {0, 1});
          check(Problem(nodes, boundary, nDim, 4, domain, withQuadrature));
        }
      }
    }
  }
}

/// Runs @p body with @p nThreads OpenMP threads (restores the previous number afterwards).
template <typename Body>
void withThreads(const int nThreads, const Body& body) {
  const int before = omp_get_max_threads();
  omp_set_num_threads(nThreads);
  body();
  omp_set_num_threads(before);
}

bool bitwiseEqual(const std::vector<double>& a, const std::vector<double>& b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::bit_cast<uint64_t>(a[i]) != std::bit_cast<uint64_t>(b[i])) return false;
  }
  return true;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(source_function_cache)

BOOST_AUTO_TEST_CASE(cached_equals_uncached_bitwise) {
  // At 1 thread, the operators' reductions have a fixed order: any difference would come from
  // the cache. The second run with the same cache answers every evaluation from the cache.
  withThreads(1, []() {
    forEachProblem([](const Problem& problem) {
      const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
      const CachedSourceFunc cached(scalarFunc);
      const std::vector<double> cold = problem.run(cached);
      const CacheStatistics afterCold = cached.statistics();
      const std::vector<double> warm = problem.run(cached);
      const CacheStatistics afterWarm = cached.statistics();

      BOOST_CHECK(bitwiseEqual(plain, cold));
      BOOST_CHECK(bitwiseEqual(plain, warm));
      BOOST_CHECK_EQUAL(afterWarm.lookups - afterCold.lookups, afterWarm.hits - afterCold.hits);
      BOOST_CHECK_EQUAL(afterWarm.entries, afterCold.entries);
      BOOST_CHECK_EQUAL(afterWarm.operatorCalls, 2 * problem.nOperatorCalls());
    });
  });
}

BOOST_AUTO_TEST_CASE(cached_vector_equals_uncached_bitwise) {
  withThreads(1, []() {
    forEachProblem([](const Problem& problem) {
      const std::vector<double> plain = problem.run(VectorSourceFunc(vectorFunc, N_OUTPUTS));
      const CachedVectorSourceFunc cached(vectorFunc, N_OUTPUTS);
      const std::vector<double> cold = problem.run(cached);
      const std::vector<double> warm = problem.run(cached);

      BOOST_CHECK(bitwiseEqual(plain, cold));
      BOOST_CHECK(bitwiseEqual(plain, warm));
      BOOST_CHECK_EQUAL(cached.cache()->nOutputs(), N_OUTPUTS);
      BOOST_CHECK_EQUAL(cached.statistics().operatorCalls, 2 * problem.nOperatorCalls());
    });
  });
}

BOOST_AUTO_TEST_CASE(cached_results_at_many_threads) {
  // At several threads the reduction order varies from call to call; the values the cache
  // returns are those of the function, so results agree up to round-off and no point is lost.
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 4, 5, shiftedDomain(4),
                        true);
  const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
  const CachedSourceFunc cached(scalarFunc);
  for (size_t round = 0; round < 3; round++) {
    const std::vector<double> results = problem.run(cached);
    BOOST_REQUIRE_EQUAL(results.size(), plain.size());
    for (size_t i = 0; i < plain.size(); i++) {
      BOOST_CHECK_CLOSE(results[i], plain[i], 1e-10);
    }
  }
  const CacheStatistics stats = cached.statistics();
  BOOST_CHECK_EQUAL(stats.entries, stats.inserts);
  BOOST_CHECK_EQUAL(stats.rejectedInserts, 0u);
}

BOOST_AUTO_TEST_CASE(values_are_reused_across_operators) {
  // All operators map the grid points with the same arithmetic (tools::mapToDomain), so after a
  // quadrature every point of global interpolation (grid points) is a hit, also on a shifted
  // domain; SG and DSG quadrature evaluate the same points.
  for (NodeGenFunc* nodes : {sgpp::combigrid::getEquidistantNodeGenFunc(),
                             sgpp::combigrid::getClenshawCurtisNodeGenFunc()}) {
    for (const LvlType boundary : {0u, 1u}) {
      const Problem problem(nodes, boundary, 3, 4, shiftedDomain(3), true);
      Counted counted(scalarFunc);
      const CachedSourceFunc cached(counted.callable());

      // Two threads that miss the same point at the same time both evaluate it.
      sgpp::combigrid::quadrature(cached, *problem.sg);
      const size_t evaluations = counted.count();
      BOOST_CHECK_LE(cached.statistics().entries, evaluations);

      sgpp::combigrid::quadrature(cached, *problem.dsg);
      for (const DataVector& point : problem.points) {
        sgpp::combigrid::interpolate(cached, point, *problem.sg);
      }
      BOOST_CHECK_EQUAL(counted.count(), evaluations);

      // Linear interpolation also evaluates points on the domain boundary, which are no grid
      // points in dimensions without boundary nodes (boundary offset 1).
      for (const DataVector& point : problem.points) {
        sgpp::combigrid::interpolateLinear(cached, point, *problem.sg);
      }
      if (boundary == 0) {
        BOOST_CHECK_EQUAL(counted.count(), evaluations);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(statistics_count_lookups_misses_and_operator_calls) {
  const Problem problem(sgpp::combigrid::getEquidistantNodeGenFunc(), 0, 3, 4, shiftedDomain(3),
                        true);
  Counted plainCount(scalarFunc);
  problem.run(SourceFunc(plainCount.callable()));

  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable());
  problem.run(cached);
  const CacheStatistics stats = cached.statistics();

  BOOST_CHECK_EQUAL(stats.lookups, plainCount.count());
  BOOST_CHECK_EQUAL(stats.lookups - stats.hits, counted.count());
  BOOST_CHECK_GE(stats.lookups - stats.hits, stats.inserts);
  BOOST_CHECK_EQUAL(stats.entries, stats.inserts);
  BOOST_CHECK_GT(stats.bytes, stats.entries * 8 * (3 + 1 + 1));
  BOOST_CHECK_EQUAL(stats.operatorCalls, problem.nOperatorCalls());

  // Plain source functions have no hook (nothing to call), cached ones count every call.
  SourceFunc(scalarFunc).finishOperatorCall();
  cached.finishOperatorCall();
  BOOST_CHECK_EQUAL(cached.statistics().operatorCalls, problem.nOperatorCalls() + 1);
}

BOOST_AUTO_TEST_CASE(memory_limit_stops_inserting) {
  // Limit 0: nothing is stored, every miss is rejected, results unchanged.
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 0, 3, 4, shiftedDomain(3),
                        true);
  withThreads(1, [&problem]() {
    const std::vector<double> reference = problem.run(SourceFunc(scalarFunc));
    const CachedSourceFunc cached(scalarFunc, SourceFuncCacheConfig{.memoryLimit = 0});
    const std::vector<double> results = problem.run(cached);
    const CacheStatistics stats = cached.statistics();
    BOOST_CHECK(bitwiseEqual(results, reference));
    BOOST_CHECK_EQUAL(stats.entries, 0u);
    BOOST_CHECK_EQUAL(stats.hits, 0u);
    BOOST_CHECK_EQUAL(stats.rejectedInserts, stats.lookups);
  });

  // Small limit (about 26 000 records of 40 bytes, 100 000 distinct points): the shards stop
  // inserting at their share, the stored values are still hits, all values are correct.
  const size_t limit = size_t{1} << 20;
  const size_t nPoints = 100000;
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable(), SourceFuncCacheConfig{.memoryLimit = limit});
  size_t wrong = 0;
  for (size_t round = 0; round < 2; round++) {
    for (size_t i = 0; i < nPoints; i++) {
      const DataVector point{static_cast<double>(i) / nPoints, 0.5, 0.25};
      wrong += (cached.evaluate(point) != scalarFunc(point)) ? 1 : 0;
    }
  }
  const CacheStatistics stats = cached.statistics();

  BOOST_CHECK_EQUAL(wrong, 0u);
  BOOST_CHECK_GT(stats.entries, nPoints / 8);
  BOOST_CHECK_LT(stats.entries, nPoints / 2);
  BOOST_CHECK_EQUAL(stats.hits, stats.entries);  // second round: the stored points
  BOOST_CHECK_EQUAL(stats.rejectedInserts, 2 * nPoints - stats.entries - stats.hits);
  BOOST_CHECK_EQUAL(counted.count(), 2 * nPoints - stats.hits);
  BOOST_CHECK_LE(stats.bytes, limit + limit / 2);  // soft: one index growth per shard beyond
}

BOOST_AUTO_TEST_CASE(nan_values_are_not_stored) {
  Counted counted([](const DataVector& x) {
    return x[0] < 0.5 ? std::numeric_limits<double>::quiet_NaN() : x[0];
  });
  const CachedSourceFunc cached(counted.callable());
  const DataVector nanPoint{0.25, 0.5};
  const DataVector point{0.75, 0.5};

  for (size_t i = 0; i < 3; i++) {
    BOOST_CHECK(std::isnan(cached.evaluate(nanPoint)));
    BOOST_CHECK_EQUAL(cached.evaluate(point), 0.75);
  }
  BOOST_CHECK_EQUAL(counted.count(), 4u);
  BOOST_CHECK_EQUAL(cached.statistics().entries, 1u);

  // Vector-valued: a NaN in one output keeps all outputs of the point out of the cache.
  std::atomic<size_t> count{0};
  const CachedVectorSourceFunc vectorCached(
      [&count](const DataVector& x, const std::span<double> out) {
        count++;
        out[0] = x[0];
        out[1] = std::numeric_limits<double>::quiet_NaN();
      },
      2);
  std::vector<double> out(2);
  vectorCached.evaluate(point, out);
  vectorCached.evaluate(point, out);
  BOOST_CHECK_EQUAL(count.load(), 2u);
  BOOST_CHECK_EQUAL(vectorCached.statistics().entries, 0u);
}

BOOST_AUTO_TEST_CASE(copies_and_shared_caches_reuse_values) {
  Counted counted(scalarFunc);
  auto cache = std::make_shared<SourceFuncCache>();
  const CachedSourceFunc cached(counted.callable(), cache);
  const DataVector point{0.1, 0.2, 0.3};

  const double value = cached.evaluate(point);
  const CachedSourceFunc copy(cached);
  const CachedSourceFunc sameCache(counted.callable(), cache);

  BOOST_CHECK_EQUAL(copy.evaluate(point), value);
  BOOST_CHECK_EQUAL(sameCache.evaluate(point), value);
  BOOST_CHECK_EQUAL(counted.count(), 1u);
  BOOST_CHECK(copy.cache() == cache);
  BOOST_CHECK(sameCache.cache() == cache);
  BOOST_CHECK_EQUAL(cache->statistics().hits, 2u);

  // The end-of-operator hook is a virtual function of the cached classes; the operators call it
  // through a reference to the base class, copies notify the same cache.
  const SourceFunc& base = copy;
  base.finishOperatorCall();
  cached.finishOperatorCall();
  BOOST_CHECK_EQUAL(cache->statistics().operatorCalls, 2u);

  // The cache lives as long as a function uses it.
  std::weak_ptr<SourceFuncCache> weak = cache;
  cache.reset();
  BOOST_CHECK(!weak.expired());
}

// Copying or moving a cached function into a plain SourceFunc / VectorSourceFunc (slicing: the
// copy would lose finishOperatorCall) does not compile; copies of the base class itself and of the
// cached classes do.
static_assert(!std::is_constructible_v<SourceFunc, const CachedSourceFunc&>);
static_assert(!std::is_constructible_v<SourceFunc, CachedSourceFunc&>);
static_assert(!std::is_constructible_v<SourceFunc, CachedSourceFunc&&>);
static_assert(!std::is_convertible_v<const CachedSourceFunc&, SourceFunc>);
static_assert(!std::is_assignable_v<SourceFunc&, const CachedSourceFunc&>);
static_assert(!std::is_assignable_v<SourceFunc&, CachedSourceFunc&&>);
static_assert(!std::is_constructible_v<VectorSourceFunc, const CachedVectorSourceFunc&>);
static_assert(!std::is_constructible_v<VectorSourceFunc, CachedVectorSourceFunc&&>);
static_assert(!std::is_assignable_v<VectorSourceFunc&, const CachedVectorSourceFunc&>);
static_assert(std::is_copy_constructible_v<SourceFunc> && std::is_copy_assignable_v<SourceFunc>);
static_assert(std::is_nothrow_move_constructible_v<SourceFunc>);
static_assert(std::is_copy_constructible_v<CachedSourceFunc> &&
              std::is_copy_assignable_v<CachedSourceFunc>);
static_assert(std::is_copy_constructible_v<VectorSourceFunc> &&
              std::is_nothrow_move_constructible_v<VectorSourceFunc>);
static_assert(std::is_copy_constructible_v<CachedVectorSourceFunc>);

BOOST_AUTO_TEST_CASE(misuse_is_rejected_or_bypasses_the_cache) {
  auto scalarCache = std::make_shared<SourceFuncCache>();
  const CachedSourceFunc scalar(scalarFunc, scalarCache);
  BOOST_CHECK_EQUAL(scalarCache->nOutputs(), 1u);

  // A cache serves one function: another number of outputs, no cache, no outputs.
  BOOST_CHECK_THROW(CachedVectorSourceFunc(vectorFunc, N_OUTPUTS, scalarCache),
                    std::invalid_argument);
  BOOST_CHECK_THROW(CachedSourceFunc(scalarFunc, std::shared_ptr<SourceFuncCache>()),
                    std::invalid_argument);
  BOOST_CHECK_THROW(CachedVectorSourceFunc(vectorFunc, 0), std::invalid_argument);

  // The number of shards must be a power of two in [1, 64] (checked at construction).
  for (const size_t nShards : {0, 3, 48, 128}) {
    BOOST_CHECK_THROW(SourceFuncCache(SourceFuncCacheConfig{.nShards = nShards}),
                      std::invalid_argument);
  }

  // Points of another dimension than the first one are evaluated without the cache.
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable());
  const DataVector point2{0.1, 0.2};
  const DataVector point3{0.1, 0.2, 0.3};
  cached.evaluate(point2);
  for (size_t i = 0; i < 3; i++) {
    BOOST_CHECK_EQUAL(cached.evaluate(point3), scalarFunc(point3));
  }
  cached.evaluate(point2);
  BOOST_CHECK_EQUAL(counted.count(), 4u);
  BOOST_CHECK_EQUAL(cached.statistics().entries, 1u);
}

BOOST_AUTO_TEST_CASE(any_shard_count_gives_the_same_results) {
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 3, 4,
                        shiftedDomain(3), true);
  withThreads(1, [&problem]() {
    const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
    size_t entries = 0;
    for (const size_t nShards : {1, 2, 8, 64}) {
      const CachedSourceFunc cached(scalarFunc, SourceFuncCacheConfig{.nShards = nShards});
      problem.run(cached);
      BOOST_CHECK(bitwiseEqual(problem.run(cached), plain));
      BOOST_CHECK_EQUAL(cached.cache()->config().nShards, nShards);
      if (entries == 0) entries = cached.statistics().entries;
      BOOST_CHECK_EQUAL(cached.statistics().entries, entries);
    }
  });
}

BOOST_AUTO_TEST_CASE(clear_removes_values_and_statistics) {
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable());
  const DataVector point{0.4, 0.6};

  cached.evaluate(point);
  cached.finishOperatorCall();
  cached.cache()->clear();
  BOOST_CHECK_EQUAL(cached.statistics().entries, 0u);
  BOOST_CHECK_EQUAL(cached.statistics().lookups, 0u);
  BOOST_CHECK_EQUAL(cached.statistics().operatorCalls, 0u);

  cached.evaluate(point);
  cached.evaluate(point);
  BOOST_CHECK_EQUAL(counted.count(), 2u);
  BOOST_CHECK_EQUAL(cached.statistics().hits, 1u);
  BOOST_CHECK_EQUAL(cached.cache()->nOutputs(), 1u);
}

BOOST_AUTO_TEST_CASE(concurrent_evaluations) {
  // Many threads evaluate overlapping points (each distinct point 64 times, interleaved): every
  // value is correct, every distinct point is stored once.
  const size_t nDistinct = 5000;
  const size_t nRepeats = 64;
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable());
  std::atomic<size_t> wrong{0};

#pragma omp parallel for schedule(dynamic, 64)
  for (size_t i = 0; i < nDistinct * nRepeats; i++) {
    const size_t idx = (i * 7919) % nDistinct;
    const DataVector point{static_cast<double>(idx) / nDistinct, 0.5, -0.25};
    if (cached.evaluate(point) != scalarFunc(point)) {
      wrong++;
    }
  }

  const CacheStatistics stats = cached.statistics();
  BOOST_CHECK_EQUAL(wrong.load(), 0u);
  BOOST_CHECK_EQUAL(stats.entries, nDistinct);
  BOOST_CHECK_EQUAL(stats.lookups, nDistinct * nRepeats);
  BOOST_CHECK_EQUAL(stats.lookups - stats.hits, counted.count());
  BOOST_CHECK_GE(counted.count(), nDistinct);
}

BOOST_AUTO_TEST_SUITE_END()

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

namespace {

/// A fresh directory for the files of one test case (removed with all files at the end).
class TestDir {
 public:
  TestDir() {
    std::string pattern =
        (std::filesystem::temp_directory_path() / "sgpp-cache-test-XXXXXX").string();
    BOOST_REQUIRE(mkdtemp(pattern.data()) != nullptr);
    path_ = pattern;
  }
  ~TestDir() { std::filesystem::remove_all(path_); }

  std::string file(const std::string& name) const { return (path_ / name).string(); }

  size_t nEntries() const {
    return static_cast<size_t>(std::distance(std::filesystem::directory_iterator(path_),
                                             std::filesystem::directory_iterator()));
  }

 private:
  std::filesystem::path path_;
};

/// Sets TMPDIR while it lives (the directory of temporary cache files).
class ScopedTmpDir {
 public:
  explicit ScopedTmpDir(const std::string& dir) {
    const char* const before = std::getenv("TMPDIR");
    hadBefore_ = before != nullptr;
    if (hadBefore_) before_ = before;
    setenv("TMPDIR", dir.c_str(), 1);
  }
  ~ScopedTmpDir() {
    if (hadBefore_) {
      setenv("TMPDIR", before_.c_str(), 1);
    } else {
      unsetenv("TMPDIR");
    }
  }

 private:
  bool hadBefore_;
  std::string before_;
};

SourceFuncCacheConfig persistent(const std::string& file,
                                 const size_t memoryLimit = SourceFuncCacheConfig().memoryLimit) {
  return SourceFuncCacheConfig{
      .memoryLimit = memoryLimit, .policy = CachePolicy::PersistentStorage, .file = file};
}

SourceFuncCacheConfig temporary(const size_t memoryLimit = SourceFuncCacheConfig().memoryLimit) {
  return SourceFuncCacheConfig{.memoryLimit = memoryLimit,
                               .policy = CachePolicy::TemporaryStorage};
}

/// Runs @p child in a forked process that ends with SIGKILL and checks that it was killed. The
/// child must kill itself (@c raise(SIGKILL)) while its objects live: returning would run their
/// destructors (which flush). It uses the OpenMP runtime at 1 thread only.
template <typename Child>
void runAndKill(const Child& child) {
  std::cout.flush();
  std::cerr.flush();
  const pid_t pid = fork();
  BOOST_REQUIRE(pid >= 0);
  if (pid == 0) {
    omp_set_num_threads(1);
    child();
    raise(SIGKILL);
    _exit(1);  // not reached
  }
  int status = 0;
  BOOST_REQUIRE_EQUAL(waitpid(pid, &status, 0), pid);
  BOOST_REQUIRE(WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL);
}

std::vector<char> readFile(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  return std::vector<char>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void writeFile(const std::string& path, const std::vector<char>& bytes) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

/// Offset of the first occurrence of @p pattern in @p bytes (bytes.size() if none).
size_t findBytes(const std::vector<char>& bytes, const void* pattern, const size_t size) {
  const char* const p = static_cast<const char*>(pattern);
  return static_cast<size_t>(std::search(bytes.begin(), bytes.end(), p, p + size) - bytes.begin());
}

/// The test grid of the storage cases (3D, Clenshaw-Curtis, boundary, shifted domain).
Problem storageProblem() {
  return Problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 3, 4, shiftedDomain(3), true);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(source_function_cache_storage)

BOOST_AUTO_TEST_CASE(file_must_fit_the_policy) {
  const TestDir dir;
  BOOST_CHECK_THROW(SourceFuncCache(SourceFuncCacheConfig{.file = dir.file("a")}),
                    std::invalid_argument);
  BOOST_CHECK_THROW(SourceFuncCache(SourceFuncCacheConfig{.policy = CachePolicy::TemporaryStorage,
                                                          .file = dir.file("a")}),
                    std::invalid_argument);
  BOOST_CHECK_THROW(
      SourceFuncCache(SourceFuncCacheConfig{.policy = CachePolicy::PersistentStorage}),
      std::invalid_argument);
  BOOST_CHECK_THROW(SourceFuncCache(persistent(dir.file("missing-directory/a"))),
                    sgpp::base::file_exception);
  BOOST_CHECK_EQUAL(dir.nEntries(), 0u);
}

BOOST_AUTO_TEST_CASE(storage_policies_equal_uncached_bitwise) {
  // Memory limit 0: every value goes to the file, every warm lookup is a disk hit; default
  // limit: the file is a write-behind copy (persistent) or unused (temporary).
  const TestDir dir;
  const ScopedTmpDir tmp(dir.file(""));
  withThreads(1, [&dir]() {
    size_t problemIdx = 0;
    forEachProblem([&](const Problem& problem) {
      const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
      for (const size_t limit : {size_t{0}, SourceFuncCacheConfig().memoryLimit}) {
        const std::string file = dir.file("p" + std::to_string(problemIdx++));
        for (const SourceFuncCacheConfig& config : {temporary(limit), persistent(file, limit)}) {
          const CachedSourceFunc cached(scalarFunc, config);
          const std::vector<double> cold = problem.run(cached);
          const CacheStatistics afterCold = cached.statistics();
          const std::vector<double> warm = problem.run(cached);
          const CacheStatistics afterWarm = cached.statistics();

          BOOST_CHECK(bitwiseEqual(plain, cold));
          BOOST_CHECK(bitwiseEqual(plain, warm));
          BOOST_CHECK_EQUAL(afterWarm.lookups - afterCold.lookups, afterWarm.hits - afterCold.hits);
          BOOST_CHECK_EQUAL(afterCold.rejectedInserts, 0u);
          if (limit == 0) {
            BOOST_CHECK_EQUAL(afterWarm.entries, 0u);
            BOOST_CHECK_EQUAL(afterWarm.diskEntries, afterCold.inserts);
            // Warm: every hit is a disk hit (cold hits may come from the batch buffers).
            BOOST_CHECK_EQUAL(afterWarm.diskHits - afterCold.diskHits,
                              afterWarm.hits - afterCold.hits);
          } else {
            BOOST_CHECK_EQUAL(afterWarm.diskHits, 0u);
            const bool isPersistent = config.policy == CachePolicy::PersistentStorage;
            BOOST_CHECK_EQUAL(afterWarm.diskEntries, isPersistent ? afterWarm.entries : 0u);
          }
        }
      }
    });
  });
}

BOOST_AUTO_TEST_CASE(temporary_file_is_unnamed_and_disappears) {
  const TestDir dir;
  const ScopedTmpDir tmp(dir.file(""));
  {
    const CachedSourceFunc cached(scalarFunc, temporary(0));
    const std::string file = cached.cache()->config().file;
    BOOST_CHECK_EQUAL(file.rfind("/proc/self/fd/", 0), 0u);
    BOOST_CHECK(std::filesystem::exists(file));
    storageProblem().run(cached);
    BOOST_CHECK_GT(cached.statistics().fileBytes, 0u);
    BOOST_CHECK_EQUAL(dir.nEntries(), 0u);  // no name (and no lock file) in the directory
  }
  BOOST_CHECK_EQUAL(dir.nEntries(), 0u);

  // Also when the process is killed while its cache writes the file.
  runAndKill([]() {
    const CachedSourceFunc cached(scalarFunc, temporary(0));
    for (size_t i = 0; i < 20000; i++) {
      cached.evaluate(DataVector{static_cast<double>(i), 0.5});
    }
    raise(SIGKILL);
  });
  BOOST_CHECK_EQUAL(dir.nEntries(), 0u);
}

BOOST_AUTO_TEST_CASE(persistent_file_round_trip) {
  // A new cache on the file of an earlier one evaluates nothing and returns the same results
  // bitwise, with all records in memory or (memory limit 0) all on disk.
  const TestDir dir;
  const Problem problem = storageProblem();
  withThreads(1, [&]() {
    const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
    const std::vector<double> plainVector = problem.run(VectorSourceFunc(vectorFunc, N_OUTPUTS));
    for (const size_t limit : {size_t{0}, SourceFuncCacheConfig().memoryLimit}) {
      const std::string file = dir.file("scalar" + std::to_string(limit));
      const std::string vectorFile = dir.file("vector" + std::to_string(limit));
      size_t stored = 0;
      {
        const CachedSourceFunc cached(scalarFunc, persistent(file, limit));
        const CachedVectorSourceFunc vectorCached(vectorFunc, N_OUTPUTS, persistent(vectorFile));
        BOOST_CHECK(bitwiseEqual(problem.run(cached), plain));
        BOOST_CHECK(bitwiseEqual(problem.run(vectorCached), plainVector));
        stored = cached.statistics().diskEntries;
        BOOST_CHECK_EQUAL(stored, cached.statistics().inserts);
      }

      Counted counted(scalarFunc);
      std::atomic<size_t> vectorCount{0};
      const CachedSourceFunc reloaded(counted.callable(), persistent(file, limit));
      const CachedVectorSourceFunc vectorReloaded(
          [&vectorCount](const DataVector& x, const std::span<double> out) {
            vectorCount++;
            vectorFunc(x, out);
          },
          N_OUTPUTS, persistent(vectorFile));
      BOOST_CHECK_EQUAL(reloaded.statistics().loadedEntries, stored);
      BOOST_CHECK_EQUAL(reloaded.statistics().entries, (limit == 0) ? 0u : stored);
      BOOST_CHECK_EQUAL(vectorReloaded.cache()->nOutputs(), N_OUTPUTS);
      BOOST_CHECK(bitwiseEqual(problem.run(reloaded), plain));
      BOOST_CHECK(bitwiseEqual(problem.run(vectorReloaded), plainVector));
      BOOST_CHECK_EQUAL(counted.count(), 0u);
      BOOST_CHECK_EQUAL(vectorCount.load(), 0u);
      BOOST_CHECK_EQUAL(reloaded.statistics().diskEntries, stored);
    }
  });
}

BOOST_AUTO_TEST_CASE(persistent_file_holds_every_finished_operator_call) {
  // The child is killed after its operator calls (no destructor): their values are in the file.
  // Values of direct evaluations reach it only through flush().
  const TestDir dir;
  const std::string file = dir.file("values");
  const Problem problem = storageProblem();
  const CachedSourceFunc reference(scalarFunc);
  problem.run(reference);

  runAndKill([&]() {
    const CachedSourceFunc cached(scalarFunc, persistent(file));
    problem.run(cached);
    for (size_t i = 0; i < 150; i++) {
      cached.evaluate(DataVector{10.0 + static_cast<double>(i), 0.5, 0.5});
      if (i == 99) cached.cache()->flush();
    }
    raise(SIGKILL);
  });

  Counted counted(scalarFunc);
  const CachedSourceFunc reloaded(counted.callable(), persistent(file));
  BOOST_CHECK_EQUAL(reloaded.statistics().loadedEntries, reference.statistics().entries + 100);
  problem.run(reloaded);
  BOOST_CHECK_EQUAL(counted.count(), 0u);
}

BOOST_AUTO_TEST_CASE(crash_inside_an_operator_call_keeps_the_file_valid) {
  // The child finishes one quadrature, then is killed in the middle of a second one, while it
  // writes batches beyond the memory limit (limit 0). The file stays valid and holds the first
  // call; values of the interrupted call may be lost.
  const TestDir dir;
  const std::string file = dir.file("values");
  const Problem small = storageProblem();
  const Problem large(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 3, 7, shiftedDomain(3),
                      true);
  runAndKill([&]() {
    std::atomic<size_t> evaluations{0};
    const CachedSourceFunc cached(
        [&evaluations](const DataVector& x) {
          if (++evaluations == 5000) raise(SIGKILL);
          return scalarFunc(x);
        },
        persistent(file, 0));
    sgpp::combigrid::quadrature(cached, *small.sg);
    sgpp::combigrid::quadrature(cached, *large.sg);
  });

  const CachedSourceFunc reference(scalarFunc);
  sgpp::combigrid::quadrature(reference, *small.sg);

  Counted counted(scalarFunc);
  std::unique_ptr<CachedSourceFunc> reloaded;
  BOOST_REQUIRE_NO_THROW(
      reloaded = std::make_unique<CachedSourceFunc>(counted.callable(), persistent(file)));
  BOOST_CHECK_GE(reloaded->statistics().loadedEntries, reference.statistics().entries);
  sgpp::combigrid::quadrature(*reloaded, *small.sg);
  BOOST_CHECK_EQUAL(counted.count(), 0u);
}

BOOST_AUTO_TEST_CASE(mismatching_or_damaged_files_throw) {
  const TestDir dir;
  const std::string file = dir.file("values");
  const double marker = 0.123456789;
  {
    const CachedSourceFunc cached(scalarFunc, persistent(file));
    for (size_t i = 0; i < 1000; i++) {
      cached.evaluate(DataVector{marker, static_cast<double>(i)});
    }
    cached.cache()->flush();

    // Locked while the cache lives.
    BOOST_CHECK_THROW(SourceFuncCache(persistent(file)), sgpp::base::file_exception);
  }
  const std::vector<char> valid = readFile(file);

  // Another number of outputs (at the first cached function) or dimension (at the end of the
  // operator call that evaluated such points; they are not cached).
  {
    auto cache = std::make_shared<SourceFuncCache>(persistent(file));
    BOOST_CHECK_EQUAL(cache->nOutputs(), 1u);
    BOOST_CHECK_THROW(CachedVectorSourceFunc(vectorFunc, N_OUTPUTS, cache),
                      sgpp::base::file_exception);
    const CachedSourceFunc cached(scalarFunc, cache);
    BOOST_CHECK_THROW(sgpp::combigrid::quadrature(cached, *storageProblem().sg),
                      sgpp::base::file_exception);
    const DataVector stored{marker, 3.0};
    BOOST_CHECK_EQUAL(cached.evaluate(stored), scalarFunc(stored));
    BOOST_CHECK_EQUAL(cached.statistics().hits, 1u);
  }

  // Not a cache file, truncated, a corrupted record, another format version.
  const std::string damaged = dir.file("damaged");
  writeFile(damaged, std::vector<char>(20000, 'x'));
  BOOST_CHECK_THROW(SourceFuncCache(persistent(damaged)), sgpp::base::file_exception);
  BOOST_CHECK(!std::filesystem::exists(damaged + "-lock"));  // nothing left behind

  writeFile(damaged, std::vector<char>(valid.begin(), valid.begin() + valid.size() / 2));
  BOOST_CHECK_THROW(SourceFuncCache(persistent(damaged)), sgpp::base::file_exception);

  // Flip a bit of every copy of a coordinate (LMDB keeps stale copies in free pages).
  std::vector<char> bytes = valid;
  size_t nFlipped = 0;
  for (size_t offset = findBytes(bytes, &marker, sizeof(marker)); offset < bytes.size();
       offset = findBytes(bytes, &marker, sizeof(marker))) {
    bytes[offset] ^= 0x01;
    nFlipped++;
  }
  BOOST_REQUIRE_GE(nFlipped, 1000u);
  writeFile(damaged, bytes);
  BOOST_CHECK_THROW(SourceFuncCache(persistent(damaged)), sgpp::base::file_exception);

  bytes = valid;
  const size_t magicOffset = findBytes(bytes, "SGPPCGSF", 8);
  BOOST_REQUIRE_LT(magicOffset, bytes.size());
  bytes[magicOffset + 8] = 99;  // version (little endian)
  writeFile(damaged, bytes);
  BOOST_CHECK_THROW(SourceFuncCache(persistent(damaged)), sgpp::base::file_exception);

  // The valid file is still accepted.
  writeFile(damaged, valid);
  BOOST_CHECK_EQUAL(SourceFuncCache(persistent(damaged)).statistics().loadedEntries, 1000u);
}

BOOST_AUTO_TEST_CASE(storage_space_limit_warns_once_and_stops_storing) {
  const TestDir dir;
  const ScopedTmpDir tmp(dir.file(""));
  for (const SourceFuncCacheConfig& base : {temporary(0), persistent(dir.file("values"), 0)}) {
    SourceFuncCacheConfig config = base;
    config.nShards = 1;
    config.storageSpaceLimit = 64 * 1024;
    std::ostringstream captured;
    std::streambuf* const cerrBefore = std::cerr.rdbuf(captured.rdbuf());

    const size_t nPoints = 20000;
    size_t wrong = 0;
    {
      const CachedSourceFunc cached(scalarFunc, config);
      for (size_t round = 0; round < 2; round++) {
        for (size_t i = 0; i < nPoints; i++) {
          const DataVector point{static_cast<double>(i), 0.5, 0.25};
          wrong += (cached.evaluate(point) != scalarFunc(point)) ? 1 : 0;
        }
        cached.cache()->flush();
      }
      const CacheStatistics stats = cached.statistics();
      BOOST_CHECK_GT(stats.diskEntries, 100u);
      BOOST_CHECK_LT(stats.diskEntries, nPoints / 4);
      BOOST_CHECK_GT(stats.rejectedInserts, nPoints);
      BOOST_CHECK_GE(stats.diskHits, stats.diskEntries);
      BOOST_CHECK_LE(stats.fileBytes, 64u * 1024u);
    }
    std::cerr.rdbuf(cerrBefore);

    const std::string messages = captured.str();
    BOOST_CHECK_EQUAL(wrong, 0u);
    BOOST_CHECK_NE(messages.find("is full"), std::string::npos);
    BOOST_CHECK_EQUAL(messages.find("is full"), messages.rfind("is full"));
  }
}

BOOST_AUTO_TEST_CASE(clear_keeps_or_empties_the_persistent_file) {
  // 1 thread: no duplicate evaluations of concurrent misses.
  const int threadsBefore = omp_get_max_threads();
  omp_set_num_threads(1);
  const TestDir dir;
  const std::string file = dir.file("values");
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable(), persistent(file));
  const Problem problem = storageProblem();
  sgpp::combigrid::quadrature(cached, *problem.sg);
  const size_t evaluations = counted.count();
  const size_t stored = cached.statistics().diskEntries;

  // Memory only: the values are still found, now in the file.
  cached.cache()->clear();
  BOOST_CHECK_EQUAL(cached.statistics().entries, 0u);
  BOOST_CHECK_EQUAL(cached.statistics().diskEntries, stored);
  sgpp::combigrid::quadrature(cached, *problem.sg);
  BOOST_CHECK_EQUAL(counted.count(), evaluations);
  BOOST_CHECK_EQUAL(cached.statistics().diskHits, cached.statistics().lookups);

  // With the file: evaluated again, the file holds them anew.
  cached.cache()->clear(true);
  BOOST_CHECK_EQUAL(cached.statistics().diskEntries, 0u);
  sgpp::combigrid::quadrature(cached, *problem.sg);
  BOOST_CHECK_EQUAL(counted.count(), 2 * evaluations);
  BOOST_CHECK_EQUAL(cached.statistics().diskEntries, stored);

  // Temporary: clear empties the file in any case.
  const ScopedTmpDir tmp(dir.file(""));
  const CachedSourceFunc temp(scalarFunc, temporary(0));
  sgpp::combigrid::quadrature(temp, *problem.sg);
  BOOST_CHECK_GT(temp.statistics().diskEntries, 0u);
  temp.cache()->clear();
  BOOST_CHECK_EQUAL(temp.statistics().diskEntries, 0u);
  omp_set_num_threads(threadsBefore);
}

BOOST_AUTO_TEST_CASE(shared_cache_and_concurrent_operator_calls) {
  // Two functions on one persistent cache with a small memory limit (records spill to the file),
  // used by concurrent operator calls of 4 threads with 4 OpenMP threads each.
  const TestDir dir;
  const std::string file = dir.file("values");
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 4, 5, shiftedDomain(4),
                        true);
  const double reference = sgpp::combigrid::quadrature(SourceFunc(scalarFunc), *problem.sg);
  {
    auto cache = std::make_shared<SourceFuncCache>(persistent(file, size_t{1} << 19));
    const CachedSourceFunc first(scalarFunc, cache);
    const CachedSourceFunc second(scalarFunc, cache);
    std::atomic<size_t> wrong{0};
    std::vector<std::thread> threads;
    for (size_t t = 0; t < 4; t++) {
      threads.emplace_back([&, t]() {
        omp_set_num_threads(4);
        for (size_t round = 0; round < 3; round++) {
          const double value = sgpp::combigrid::quadrature((t % 2 == 0) ? first : second,
                                                           *problem.sg);
          if (std::abs(value - reference) > 1e-10 * std::abs(reference)) wrong++;
        }
      });
    }
    for (std::thread& thread : threads) thread.join();
    BOOST_CHECK_EQUAL(wrong.load(), 0u);
    BOOST_CHECK_EQUAL(cache->statistics().operatorCalls, 12u);
    BOOST_CHECK_GT(cache->statistics().diskHits, 0u);
  }

  Counted counted(scalarFunc);
  const CachedSourceFunc reloaded(counted.callable(), persistent(file));
  BOOST_CHECK_CLOSE(sgpp::combigrid::quadrature(reloaded, *problem.sg), reference, 1e-10);
  BOOST_CHECK_EQUAL(counted.count(), 0u);
}

BOOST_AUTO_TEST_CASE(spilling_at_many_threads) {
  const TestDir dir;
  const ScopedTmpDir tmp(dir.file(""));
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 4, 5, shiftedDomain(4),
                        true);
  const std::vector<double> plain = problem.run(SourceFunc(scalarFunc));
  Counted counted(scalarFunc);
  const CachedSourceFunc cached(counted.callable(), temporary(size_t{1} << 19));
  for (size_t round = 0; round < 3; round++) {
    const std::vector<double> results = problem.run(cached);
    BOOST_REQUIRE_EQUAL(results.size(), plain.size());
    for (size_t i = 0; i < plain.size(); i++) {
      BOOST_CHECK_CLOSE(results[i], plain[i], 1e-10);
    }
  }
  const CacheStatistics stats = cached.statistics();
  BOOST_CHECK_EQUAL(stats.lookups - stats.hits, counted.count());
  BOOST_CHECK_GT(stats.diskEntries, 0u);
  BOOST_CHECK_GT(stats.diskHits, 0u);
  BOOST_CHECK_EQUAL(stats.rejectedInserts, 0u);
}

BOOST_AUTO_TEST_SUITE_END()

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#endif  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
