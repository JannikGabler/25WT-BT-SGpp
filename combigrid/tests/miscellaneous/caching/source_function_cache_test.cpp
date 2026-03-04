// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

double simpleSum(const sgpp::base::DataVector& v) {
  double sum = 0.0;
  for (size_t i = 0; i < v.getSize(); ++i) {
    sum += v[i];
  }
  return sum;
}

}  // namespace
BOOST_AUTO_TEST_CASE(empty_cache) {
  misc::SourceFunctionCache cache;

  const DataVector point{1.0, 2.0};
  double value = 0.0;

  BOOST_CHECK(cache.size() == 0);
  BOOST_CHECK(!cache.find(point, value));
}

BOOST_AUTO_TEST_CASE(test_insert_and_find) {
  misc::SourceFunctionCache cache;

  const DataVector point{1.0, 2.0, 3.0};
  cache.insert(point, 6.0);

  double value = 0.0;
  BOOST_CHECK(cache.find(point, value));
  BOOST_CHECK(value == 6.0);
  BOOST_CHECK(cache.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_insert_overwrite) {
  misc::SourceFunctionCache cache;

  const DataVector point{1.0, 2.0};
  cache.insert(point, 3.0);
  cache.insert(point, 42.0);

  double value = 0.0;
  BOOST_CHECK(cache.find(point, value));
  BOOST_CHECK(value == 42.0);
  BOOST_CHECK(cache.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_move_insert) {
  misc::SourceFunctionCache cache;

  const DataVector point{4.0, 5.0};
  cache.insert(std::move(point), 9.0);

  const DataVector lookup{4.0, 5.0};
  double value = 0.0;

  BOOST_CHECK(cache.find(lookup, value));
  BOOST_CHECK(value == 9.0);
}

BOOST_AUTO_TEST_CASE(test_erase) {
  misc::SourceFunctionCache cache;

  const DataVector point1{1.0};
  const DataVector point2{2.0};

  cache.insert(point1, 1.0);

  BOOST_CHECK(cache.erase(point1));
  BOOST_CHECK(!cache.erase(point2));
  BOOST_CHECK(cache.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_find_or_compute_single_evaluation) {
  misc::SourceFunctionCache cache;

  const SourceFunc sourceFunc = simpleSum;

  const DataVector point{1.0, 2.0, 3.0};

  const double v1 = cache.findOrCompute(point, sourceFunc);
  const double v2 = cache.findOrCompute(point, sourceFunc);

  BOOST_CHECK(v1 == 6.0);
  BOOST_CHECK(v2 == 6.0);
  BOOST_CHECK(cache.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_find_or_compute_multiple_points) {
  misc::SourceFunctionCache cache;

  const SourceFunc sourceFunc = simpleSum;

  const DataVector point1{1.0};
  const DataVector point2{2.0};

  BOOST_CHECK(cache.findOrCompute(point1, sourceFunc) == 1.0);
  BOOST_CHECK(cache.findOrCompute(point2, sourceFunc) == 2.0);
  BOOST_CHECK(cache.size() == 2);
}

BOOST_AUTO_TEST_CASE(test_shard_count) {
  misc::SourceFunctionCache cache(16);

  BOOST_CHECK(cache.shardCnt() == 16);
}

BOOST_AUTO_TEST_CASE(test_multithreaded_find_or_compute) {
  misc::SourceFunctionCache cache(2);

  const SourceFunc sourceFunc = simpleSum;

  const DataVector point{1.0, 2.0, 3.0};

#pragma omp parallel for schedule(static)
  for (int i = 0; i < 8; ++i) {
    double value = cache.findOrCompute(point, sourceFunc);
    BOOST_CHECK(value == 6.0);
  }

  BOOST_CHECK(cache.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_randomized_multithreaded_access_openmp) {
  constexpr size_t numThreads = 8;
  constexpr size_t iterationsPerThread = 5000;
  constexpr size_t dimension = 4;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    misc::SourceFunctionCache cache(64);

    const SourceFunc sourceFunc = simpleSum;

    std::vector<bool> error(numThreads);

// Parallel randomized access
#pragma omp parallel num_threads(numThreads)
    {
      const size_t threadId = static_cast<size_t>(omp_get_thread_num());
      for (size_t it = 0; it < iterationsPerThread; ++it) {
        // Generate random point
        DataVector point(dimension);
        for (size_t d = 0; d < dimension; ++d) {
          point[d] = randGen.getUniformRN(-10.0, 10.0);
        }

        const int action = static_cast<int>(randGen.getUniformRN(0.0, 4.0));

        if (action == 0) {  // findOrCompute
          const double value = cache.findOrCompute(point, sourceFunc);
          const double expected = sourceFunc(point);
          if (value != expected) error[threadId] = true;
        } else if (action == 1) {  // insert
          const double value = sourceFunc(point);
          cache.insert(point, value);
        } else if (action == 2) {  // find
          double value = 0.0;
          if (cache.find(point, value)) {
            const double expected = sourceFunc(point);
            if (value != expected) error[threadId] = true;
          }
        } else {  // Erase
          cache.erase(point);
        }
      }
    }

    for (size_t threadId = 0; threadId < error.size(); threadId++) {
      BOOST_CHECK_MESSAGE(!error[threadId],
                          "An error occured in thread '" + std::to_string(threadId) + "'.");
    }

    // Post-parallel validation using dump()
    const auto shards = cache.dump();

    size_t totalEntries = 0;

    for (const auto& shardMap : shards) {
      for (const auto& kv : shardMap) {
        const sgpp::base::DataVector& point = kv.first;
        const double storedValue = kv.second;

        const double expectedValue = sourceFunc(point);

        // Validate value correctness
        BOOST_CHECK(storedValue == expectedValue);

        ++totalEntries;
      }
    }

    BOOST_CHECK(totalEntries == cache.size());

    // Additional spot-checks after dump
    for (size_t i = 0; i < 100; ++i) {
      DataVector point(dimension);
      for (size_t d = 0; d < dimension; ++d) {
        point[d] = randGen.getUniformRN(-5.0, 5.0);
      }

      const double value = cache.findOrCompute(point, sourceFunc);
      BOOST_CHECK(value == sourceFunc(point));
    }
  }
}