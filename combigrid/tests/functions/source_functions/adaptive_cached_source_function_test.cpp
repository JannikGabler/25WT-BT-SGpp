// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_cached_source_function_test.cpp
 * @brief Boost.Test cases for the adaptive caching policy (@c CachePolicy::Adaptive) with real
 * cached functions: results bitwise those of the uncached function whatever the mode (all
 * operators, node families, boundary offsets 0 / 1, unit and shifted domains, switches in the
 * middle of operator calls), a cheap function switched off and probed, an expensive one kept,
 * the temporary file once memory is full, concurrent operator calls, misuse. Every evaluation is
 * timed (sampling shift 0); the functions are 100x away from the break-even, so that the
 * decisions do not depend on timing noise (the rules themselves: adaptive_cache_controller_test).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <stdlib.h>

#include <atomic>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_timer.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

using sgpp::base::DataVector;
using sgpp::combigrid::AdaptiveCacheMode;
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
using sgpp::combigrid::misc::adaptive_caching::CostTimer;

namespace {

const SourceFuncCacheConfig ADAPTIVE{.policy = CachePolicy::Adaptive};

double cheapFunc(const DataVector& x) {
  double sum = 0;
  for (size_t k = 0; k < x.size(); k++) sum += static_cast<double>(k + 1) * x[k];
  return std::sin(sum) + std::exp(-x[0]);
}

void cheapVectorFunc(const DataVector& x, const std::span<double> out) {
  out[0] = cheapFunc(x);
  out[1] = x[0] * x[x.size() - 1];
}

/// cheapFunc after 200 us of busy waiting (debug lookups cost a few microseconds).
double expensiveFunc(const DataVector& x) {
  const auto end = std::chrono::steady_clock::now() + std::chrono::microseconds(200);
  while (std::chrono::steady_clock::now() < end) {
  }
  return cheapFunc(x);
}

HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t k = 0; k < nDim; k++) {
    domain[k] = {-0.5 + 0.1 * static_cast<double>(k), 1.0 + 0.2 * static_cast<double>(k)};
  }
  return domain;
}

/// A sparse grid (and deltas sparse grid for nested families) with 4 interpolation points.
struct Problem {
  Problem(NodeGenFunc* nodes, const LvlType boundary, const size_t nDim, const LvlType maxLvl,
          const HyperCubeArea& domain, const bool withQuadrature)
      : genInstr(maxLvl, nDim), quadrature(withQuadrature) {
    genInstr.setNodeGenFunc(nodes);
    genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
    genInstr.setBoundaryLevelOffset(boundary);
    genInstr.setDomain(domain);
    sg = std::make_unique<SparseGrid>(genInstr);
    if (withQuadrature) dsg = std::make_unique<DeltasSparseGrid>(genInstr);
    for (const double t : {0.0, 0.31, 0.77, 1.0}) {
      DataVector point(nDim);
      for (size_t k = 0; k < nDim; k++) {
        const double s = t + 0.07 * static_cast<double>(k);
        point[k] = domain[k].first + (s - std::floor(s)) * (domain[k].second - domain[k].first);
      }
      points.push_back(point);
    }
  }

  /// Results of all operators (scalar or vector-valued function).
  template <typename Func>
  std::vector<double> run(const Func& f) const {
    std::vector<double> results;
    const auto append = [&results](const auto& value) {
      if constexpr (std::is_same_v<std::decay_t<decltype(value)>, double>) {
        results.push_back(value);
      } else {
        results.insert(results.end(), value.begin(), value.end());
      }
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

bool bitwiseEqual(const std::vector<double>& a, const std::vector<double>& b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::bit_cast<uint64_t>(a[i]) != std::bit_cast<uint64_t>(b[i])) return false;
  }
  return true;
}

template <typename Body>
void withThreads(const int nThreads, const Body& body) {
  const int before = omp_get_max_threads();
  omp_set_num_threads(nThreads);
  body();
  omp_set_num_threads(before);
}

/// Times every evaluation of the caches created while it lives (windows of 64 evaluations).
struct SampleEveryEvaluation {
  SampleEveryEvaluation() { CostTimer::overrideSamplingShift(0); }
  ~SampleEveryEvaluation() { CostTimer::overrideSamplingShift(-1); }
};

Problem smallProblem() {
  return Problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 2, 4, shiftedDomain(2), true);
}

}  // namespace

BOOST_FIXTURE_TEST_SUITE(source_function_cache_adaptive, SampleEveryEvaluation)

BOOST_AUTO_TEST_CASE(adaptive_equals_uncached_bitwise) {
  // At 1 thread the reductions have a fixed order: any difference would come from the cache. The
  // cheap function is switched off within the first operator calls and probed later, so the
  // runs mix all modes, also within operator calls.
  withThreads(1, []() {
    for (const auto& [nodes, withQuadrature] :
         {std::pair{sgpp::combigrid::getEquidistantNodeGenFunc(), true},
          std::pair{sgpp::combigrid::getClenshawCurtisNodeGenFunc(), true},
          std::pair{sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc(), false}}) {
      for (const LvlType boundary : {0u, 1u}) {
        for (const size_t nDim : {1, 2, 3}) {
          for (const bool shifted : {false, true}) {
            const Problem problem(nodes, boundary, nDim, 4,
                                  shifted ? shiftedDomain(nDim) : HyperCubeArea(nDim, {0, 1}),
                                  withQuadrature);
            const std::vector<double> plain = problem.run(SourceFunc(cheapFunc));
            const CachedSourceFunc cached(cheapFunc, ADAPTIVE);
            BOOST_CHECK(bitwiseEqual(plain, problem.run(cached)));
            BOOST_CHECK(bitwiseEqual(plain, problem.run(cached)));

            const std::vector<double> plainVector =
                problem.run(VectorSourceFunc(cheapVectorFunc, 2));
            const CachedVectorSourceFunc cachedVector(cheapVectorFunc, 2, ADAPTIVE);
            BOOST_CHECK(bitwiseEqual(plainVector, problem.run(cachedVector)));
            BOOST_CHECK(bitwiseEqual(plainVector, problem.run(cachedVector)));
          }
        }
      }
    }
  });
}

BOOST_AUTO_TEST_CASE(cheap_function_is_switched_off_and_probed) {
  withThreads(1, []() {
    const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 0, 3, 5,
                          shiftedDomain(3), true);
    const CachedSourceFunc cached(cheapFunc, ADAPTIVE);
    const double plain = sgpp::combigrid::quadrature(SourceFunc(cheapFunc), *problem.sg);
    BOOST_CHECK_EQUAL(sgpp::combigrid::quadrature(cached, *problem.sg), plain);

    CacheStatistics stats = cached.statistics();
    BOOST_CHECK(stats.adaptiveMode == AdaptiveCacheMode::None);  // in the first call
    BOOST_CHECK_EQUAL(stats.modeSwitches, 1u);
    BOOST_CHECK_GT(stats.functionCostNs, 0.0);
    BOOST_CHECK_LT(stats.functionCostNs, 0.8 * stats.memoryMissOverheadNs);
    BOOST_CHECK_LT(stats.lookups, 200u);  // the first window and a few in flight

    // 20 further calls: probes after 1, 2, 4, 8 calls in None, each switched off again.
    for (size_t call = 0; call < 20; call++) {
      BOOST_CHECK_EQUAL(sgpp::combigrid::quadrature(cached, *problem.sg), plain);
    }
    stats = cached.statistics();
    BOOST_CHECK(stats.adaptiveMode == AdaptiveCacheMode::None);
    BOOST_CHECK_EQUAL(stats.operatorCalls, 21u);
    BOOST_CHECK_EQUAL(stats.modeSwitches, 1u + 2 * 4);
    BOOST_CHECK_EQUAL(stats.noneOperatorCalls, 21u);
  });
}

BOOST_AUTO_TEST_CASE(expensive_function_stays_cached) {
  const Problem problem = smallProblem();
  const std::vector<double> plain = problem.run(SourceFunc(cheapFunc));  // same values
  const CachedSourceFunc cached(expensiveFunc, ADAPTIVE);
  const std::vector<double> cold = problem.run(cached);
  const CacheStatistics afterCold = cached.statistics();
  const std::vector<double> warm = problem.run(cached);
  const CacheStatistics afterWarm = cached.statistics();

  for (size_t i = 0; i < plain.size(); i++) {
    BOOST_CHECK_CLOSE(cold[i], plain[i], 1e-10);  // several threads: reduction order varies
    BOOST_CHECK_CLOSE(warm[i], plain[i], 1e-10);
  }
  BOOST_CHECK(afterWarm.adaptiveMode == AdaptiveCacheMode::MemoryOnly);
  BOOST_CHECK_EQUAL(afterWarm.modeSwitches, 0u);
  BOOST_CHECK_EQUAL(afterWarm.lookups - afterCold.lookups, afterWarm.hits - afterCold.hits);
  BOOST_CHECK_GT(afterWarm.functionCostNs, 100'000.0);
}

BOOST_AUTO_TEST_CASE(misuse_and_other_dimensions) {
  // A file is only for PersistentStorage.
  BOOST_CHECK_THROW(SourceFuncCache(SourceFuncCacheConfig{.policy = CachePolicy::Adaptive,
                                                          .file = "values.cache"}),
                    std::invalid_argument);

  // Points of another dimension are evaluated without the cache, without an error.
  const CachedSourceFunc cached(cheapFunc, ADAPTIVE);
  BOOST_CHECK_EQUAL(cached.evaluate(DataVector{0.25, 0.5}), cheapFunc(DataVector{0.25, 0.5}));
  BOOST_CHECK_EQUAL(cached.evaluate(DataVector{0.25, 0.5, 0.75}),
                    cheapFunc(DataVector{0.25, 0.5, 0.75}));
  BOOST_CHECK_NO_THROW(cached.finishOperatorCall());
  BOOST_CHECK_EQUAL(cached.statistics().lookups, 1u);

  // clear() starts over in MemoryOnly.
  withThreads(1, [&cached]() { sgpp::combigrid::quadrature(cached, *smallProblem().sg); });
  cached.cache()->clear();
  const CacheStatistics stats = cached.statistics();
  BOOST_CHECK(stats.adaptiveMode == AdaptiveCacheMode::MemoryOnly);
  BOOST_CHECK_EQUAL(stats.modeSwitches, 0u);
  BOOST_CHECK_EQUAL(stats.lookups, 0u);
  BOOST_CHECK_EQUAL(stats.functionCostNs, 0.0);
}

BOOST_AUTO_TEST_CASE(concurrent_operator_calls_on_one_function) {
  // 4 threads with 4 OpenMP threads each run operators on one adaptive function while its mode
  // switches (cheap function: off in the first window, probes at the hooks of all threads).
  const Problem problem(sgpp::combigrid::getClenshawCurtisNodeGenFunc(), 1, 4, 5, shiftedDomain(4),
                        true);
  const double reference = sgpp::combigrid::quadrature(SourceFunc(cheapFunc), *problem.sg);
  const CachedSourceFunc cached(cheapFunc, ADAPTIVE);
  std::atomic<size_t> wrong{0};
  std::vector<std::thread> threads;
  for (size_t t = 0; t < 4; t++) {
    threads.emplace_back([&]() {
      omp_set_num_threads(4);
      for (size_t round = 0; round < 5; round++) {
        const double value = sgpp::combigrid::quadrature(cached, *problem.sg);
        if (std::abs(value - reference) > 1e-10 * std::abs(reference)) wrong++;
      }
    });
  }
  for (std::thread& thread : threads) thread.join();
  BOOST_CHECK_EQUAL(wrong.load(), 0u);
  BOOST_CHECK_EQUAL(cached.statistics().operatorCalls, 20u);
  BOOST_CHECK_GE(cached.statistics().modeSwitches, 1u);
}

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

BOOST_AUTO_TEST_CASE(temporary_storage_once_memory_is_full) {
  // Nothing fits into memory (limit 0): the expensive function goes to TemporaryStorage in the
  // first window, which creates the temporary file at the next miss (in TMPDIR).
  std::string dir = (std::filesystem::temp_directory_path() / "sgpp-adaptive-XXXXXX").string();
  BOOST_REQUIRE(mkdtemp(dir.data()) != nullptr);
  const char* const tmpBefore = std::getenv("TMPDIR");
  const std::string tmpRestore = (tmpBefore != nullptr) ? tmpBefore : "";
  setenv("TMPDIR", dir.c_str(), 1);

  withThreads(1, []() {
    const Problem problem = smallProblem();
    const std::vector<double> plain = problem.run(SourceFunc(cheapFunc));
    const CachedSourceFunc cached(
        expensiveFunc, SourceFuncCacheConfig{.memoryLimit = 0, .policy = CachePolicy::Adaptive});
    BOOST_CHECK(bitwiseEqual(plain, problem.run(cached)));
    const CacheStatistics cold = cached.statistics();
    BOOST_CHECK(bitwiseEqual(plain, problem.run(cached)));
    const CacheStatistics warm = cached.statistics();

    BOOST_CHECK(warm.adaptiveMode == AdaptiveCacheMode::TemporaryStorage);
    BOOST_CHECK_EQUAL(warm.entries, 0u);
    BOOST_CHECK_GT(cold.diskEntries, 0u);
    BOOST_CHECK_GT(warm.diskHits, cold.diskHits);
    BOOST_CHECK_GT(warm.diskHitCostNs, 0.0);
    BOOST_CHECK(cached.cache()->config().file.empty());  // unnamed, not exposed
  });

  if (tmpBefore != nullptr) {
    setenv("TMPDIR", tmpRestore.c_str(), 1);
  } else {
    unsetenv("TMPDIR");
  }
  std::filesystem::remove_all(dir);
}

BOOST_AUTO_TEST_CASE(failing_temporary_file_is_reported_once) {
  // TMPDIR does not exist: the creation fails inside the operator, the cache goes on in memory
  // (MemoryOnly) and the end of that operator call throws once.
  const char* const tmpBefore = std::getenv("TMPDIR");
  const std::string tmpRestore = (tmpBefore != nullptr) ? tmpBefore : "";
  setenv("TMPDIR", "/nonexistent-directory-of-sgpp-tests", 1);

  withThreads(1, []() {
    const Problem problem = smallProblem();
    const CachedSourceFunc cached(
        expensiveFunc, SourceFuncCacheConfig{.memoryLimit = 0, .policy = CachePolicy::Adaptive});
    BOOST_CHECK_THROW(sgpp::combigrid::quadrature(cached, *problem.sg), sgpp::base::file_exception);
    BOOST_CHECK_NO_THROW(sgpp::combigrid::quadrature(cached, *problem.sg));
    BOOST_CHECK(cached.statistics().adaptiveMode == AdaptiveCacheMode::MemoryOnly);
  });

  if (tmpBefore != nullptr) {
    setenv("TMPDIR", tmpRestore.c_str(), 1);
  } else {
    unsetenv("TMPDIR");
  }
}

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

BOOST_AUTO_TEST_SUITE_END()

#endif  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
