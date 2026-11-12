// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file operator_data_cache_test.cpp
 * @brief Boost.Test cases for @c misc::OperatorDataCache (one object per type, built once, also
 * under concurrent first calls; a throwing build stores nothing).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <atomic>
#include <memory>
#include <sgpp/combigrid/miscellaneous/operator_data_cache/operator_data_cache.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using sgpp::combigrid::misc::OperatorDataCache;

namespace {

/// Counts its destructions (the cache must delete what it stores).
struct Tracked {
  explicit Tracked(std::atomic<int>& destroyed) : destroyed(&destroyed) {}
  ~Tracked() { (*destroyed)++; }
  std::atomic<int>* destroyed;
};

}  // namespace

BOOST_AUTO_TEST_SUITE(operator_data_cache)

BOOST_AUTO_TEST_CASE(One_Object_Per_Type) {
  OperatorDataCache cache;
  int nBuilds = 0;
  const auto buildInt = [&]() {
    nBuilds++;
    return std::make_unique<int>(42);
  };

  const int& first = cache.get<int>(buildInt);
  const int& second = cache.get<int>(buildInt);
  BOOST_CHECK_EQUAL(first, 42);
  BOOST_CHECK_EQUAL(&first, &second);
  BOOST_CHECK_EQUAL(nBuilds, 1);

  const std::string& text =
      cache.get<std::string>([]() { return std::make_unique<std::string>("x"); });
  BOOST_CHECK_EQUAL(text, "x");
  BOOST_CHECK_EQUAL(cache.size(), 2u);
  BOOST_CHECK_EQUAL(&cache.get<int>(buildInt), &first);
  BOOST_CHECK_EQUAL(nBuilds, 1);
}

BOOST_AUTO_TEST_CASE(Throwing_Build_Stores_Nothing) {
  OperatorDataCache cache;
  BOOST_CHECK_THROW(cache.get<int>([]() -> std::unique_ptr<int> { throw std::runtime_error("x"); }),
                    std::runtime_error);
  BOOST_CHECK_EQUAL(cache.size(), 0u);
  BOOST_CHECK_EQUAL(cache.get<int>([]() { return std::make_unique<int>(7); }), 7);
}

BOOST_AUTO_TEST_CASE(Concurrent_First_Calls_Build_Once) {
  OperatorDataCache cache;
  std::atomic<int> nBuilds{0};
  std::vector<const int*> results(64);

#pragma omp parallel for num_threads(8) schedule(static, 1)
  for (size_t i = 0; i < results.size(); i++) {
    results[i] = &cache.get<int>([&]() {
      nBuilds++;
      return std::make_unique<int>(3);
    });
  }

  BOOST_CHECK_EQUAL(nBuilds.load(), 1);
  for (const int* result : results) BOOST_CHECK_EQUAL(result, results[0]);
}

BOOST_AUTO_TEST_CASE(Objects_Are_Deleted_With_The_Cache) {
  std::atomic<int> destroyed{0};
  {
    OperatorDataCache cache;
    cache.get<Tracked>([&]() { return std::make_unique<Tracked>(destroyed); });
    BOOST_CHECK_EQUAL(destroyed.load(), 0);
  }
  BOOST_CHECK_EQUAL(destroyed.load(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
