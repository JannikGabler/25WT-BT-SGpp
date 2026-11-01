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
 * that the constructors throw exactly when SG++ was built without them.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>

#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
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
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
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

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
  BOOST_CHECK_NO_THROW(SourceFuncCache());
  BOOST_CHECK_NO_THROW(CachedSourceFunc{func});
  BOOST_CHECK_NO_THROW(CachedVectorSourceFunc(vectorFunc, 1));
#else
  BOOST_CHECK_THROW(SourceFuncCache(), sgpp::base::dependency_exception);
  BOOST_CHECK_THROW(CachedSourceFunc{func}, sgpp::base::dependency_exception);
  BOOST_CHECK_THROW(CachedVectorSourceFunc(vectorFunc, 1), sgpp::base::dependency_exception);
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

#endif  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
