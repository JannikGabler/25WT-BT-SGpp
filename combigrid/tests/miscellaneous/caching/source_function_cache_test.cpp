// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <algorithm>
#include <limits>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

double simpleSum(const DataVector& point) {
  double sum = 0;

  for (const double v : point) {
    sum += v;
  }

  return sum;
}

bool isDataVectorContained(const std::vector<DataVector>& vec, const DataVector& dataVec) {
  for (const DataVector& v : vec) {
    BOOST_REQUIRE_EQUAL(v.size(), dataVec.size());

    bool equals = true;

    for (size_t i = 0; i < v.size(); i++) {
      if (v[i] != dataVec[i]) {
        equals = false;
        break;
      }
    }

    if (equals) {
      return true;
    }
  }

  return false;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(source_function_cache)

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

BOOST_AUTO_TEST_CASE(test_double_insert) {
  misc::SourceFunctionCache cache;

  const DataVector point{1.0, 2.0, 3.0};

  cache.insert(point, 6.0);

  double v1 = -1;
  cache.find(point, v1);

  cache.insert(point, 6.0);

  double v2 = -1;
  cache.find(point, v2);

  BOOST_CHECK(v1 == 6.0);
  BOOST_CHECK(v2 == 6.0);
  BOOST_CHECK(cache.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_insert_and_find_multiple_points) {
  misc::SourceFunctionCache cache;

  const DataVector point1{1.0};
  const DataVector point2{2.0};

  cache.insert(point1, 1.0);
  cache.insert(point2, 2.0);

  double v1 = -1;
  cache.find(point1, v1);

  double v2 = -1;
  cache.find(point2, v2);

  BOOST_CHECK(v1 == 1.0);
  BOOST_CHECK(v2 == 2.0);
  BOOST_CHECK(cache.size() == 2);
}

BOOST_AUTO_TEST_CASE(test_shard_count) {
  misc::SourceFunctionCache cache(16);

  BOOST_CHECK(cache.shardCnt() == 16);
}

BOOST_AUTO_TEST_CASE(test_multithreaded_insert_and_find) {
  const size_t n = 20;

  misc::SourceFunctionCache cache(2);

  const DataVector point{1.0, 2.0, 3.0};
  std::vector<bool> errors(n, false);

#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    cache.insert(point, 6);

    double v = -1;
    cache.find(point, v);

    if (v != 6.0) {
      errors[i] = true;
    }
  }

  for (size_t i = 0; i < n; i++) {
    BOOST_CHECK_MESSAGE(!errors[i], "Iteration '" + std::to_string(i) + "' has reported an error.");
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

        const size_t action = randGen.getUniformIndexRN(3);

        if (action == 0) {  // insert
          const double value = simpleSum(point);
          cache.insert(point, value);
        } else if (action == 1) {  // find
          double value = 0.0;
          if (cache.find(point, value)) {
            const double expected = simpleSum(point);
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

        const double expectedValue = simpleSum(point);

        // Validate value correctness
        BOOST_CHECK(storedValue == expectedValue);

        ++totalEntries;
      }
    }

    BOOST_CHECK(totalEntries == cache.size());
  }
}

BOOST_AUTO_TEST_CASE(number_of_evaluations_test) {
  constexpr size_t nPoints = 500;
  constexpr size_t nRetrievals = 500;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(64);
    size_t nEvaluations = 0;

    const SourceFunc sourceFunc([&](const DataVector point) {
      nEvaluations++;
      double s = -std::numeric_limits<double>::max();
      for (const double v : point) {
        s = std::max(s, v);
      }
      return s;
    });

    std::vector<DataVector> points(nPoints, DataVector(nDim));
    for (size_t i = 0; i < nPoints; i++) {
      for (size_t dim = 0; dim < nDim; dim++) {
        points[i][dim] = randGen.getUniformRN(0, +1);
      }
    }

    const HyperCubeArea area(nDim, {0, 1});
    std::vector<DataVector> evaluatedPoints;

    for (size_t i = 0; i < nRetrievals; i++) {
      const size_t idx = randGen.getUniformIndexRN(nPoints);
      const DataVector point = points[idx];
      const size_t nEvaluationsBefore = nEvaluations;
      const double result = sourceFunc.evaluateNormalized(point, area);

      if (isDataVectorContained(points, point)) {  // Point has not been evaluated before
        BOOST_REQUIRE_EQUAL(nEvaluations, nEvaluationsBefore + 1);
        BOOST_REQUIRE_EQUAL(result, *std::max_element(point.begin(), point.end()));
        evaluatedPoints.push_back(point);
      } else {  // Point has been evaluated before
        BOOST_REQUIRE_EQUAL(nEvaluations, nEvaluationsBefore);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()