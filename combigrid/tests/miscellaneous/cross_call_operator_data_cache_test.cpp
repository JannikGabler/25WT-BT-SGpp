// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_cache_test.cpp
 * @brief Boost.Test cases for @c misc::CrossCallOperatorDataCache (one object per (type, key),
 * built once, also under concurrent first calls; a throwing build stores nothing; clear keeps the
 * objects held by callers; entries describe the stored objects; concurrent get, clear and view).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <atomic>
#include <cstddef>
#include <memory>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_cache.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_descriptor.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using sgpp::combigrid::CrossCallOperatorDataInfo;
using sgpp::combigrid::misc::CrossCallOperatorDataCache;
using sgpp::combigrid::misc::CrossCallOperatorDataDescriptor;

namespace {

constexpr CrossCallOperatorDataDescriptor DESCRIPTOR{
    .operatorName = "op", .description = "data", .key = ""};
constexpr CrossCallOperatorDataDescriptor OTHER_KEY{
    .operatorName = "op", .description = "data", .key = "other"};

/// Counts its destructions (the cache must delete what it stores).
struct Tracked {
  explicit Tracked(std::atomic<int>& destroyed) : destroyed(&destroyed) {}
  ~Tracked() { (*destroyed)++; }
  Tracked(const Tracked&) = delete;
  Tracked& operator=(const Tracked&) = delete;
  std::atomic<int>* destroyed;
};

/// Reports a memory of its own.
struct Sized {
  size_t memoryBytes() const noexcept { return 1234; }
};

}  // namespace

BOOST_AUTO_TEST_SUITE(cross_call_operator_data_cache)

BOOST_AUTO_TEST_CASE(One_Object_Per_Type_And_Key) {
  CrossCallOperatorDataCache cache;
  int nBuilds = 0;
  const auto buildInt = [&]() {
    nBuilds++;
    return std::make_unique<int>(42);
  };

  const std::shared_ptr<const int> first = cache.getOrBuild<int>(DESCRIPTOR, buildInt);
  const std::shared_ptr<const int> second = cache.getOrBuild<int>(DESCRIPTOR, buildInt);
  BOOST_CHECK_EQUAL(*first, 42);
  BOOST_CHECK_EQUAL(first.get(), second.get());
  BOOST_CHECK_EQUAL(nBuilds, 1);

  const std::shared_ptr<const int> otherKey = cache.getOrBuild<int>(OTHER_KEY, buildInt);
  BOOST_CHECK_NE(otherKey.get(), first.get());
  BOOST_CHECK_EQUAL(nBuilds, 2);

  const std::shared_ptr<const std::string> text = cache.getOrBuild<std::string>(
      DESCRIPTOR, []() { return std::make_unique<std::string>("x"); });
  BOOST_CHECK_EQUAL(*text, "x");
  BOOST_CHECK_EQUAL(cache.size(), 3u);
  BOOST_CHECK_EQUAL(cache.getOrBuild<int>(DESCRIPTOR, buildInt).get(), first.get());
  BOOST_CHECK_EQUAL(nBuilds, 2);
}

BOOST_AUTO_TEST_CASE(Throwing_Build_Stores_Nothing) {
  CrossCallOperatorDataCache cache;
  BOOST_CHECK_THROW(
      cache.getOrBuild<int>(DESCRIPTOR,
                            []() -> std::unique_ptr<int> { throw std::runtime_error("x"); }),
      std::runtime_error);
  BOOST_CHECK_EQUAL(cache.size(), 0u);
  BOOST_CHECK(cache.entries().empty());
  BOOST_CHECK_EQUAL(cache.memoryBytes(), 0u);
  BOOST_CHECK_EQUAL(*cache.getOrBuild<int>(DESCRIPTOR, []() { return std::make_unique<int>(7); }),
                    7);
  BOOST_CHECK_EQUAL(cache.size(), 1u);
}

BOOST_AUTO_TEST_CASE(Concurrent_First_Calls_Build_Once) {
  CrossCallOperatorDataCache cache;
  std::atomic<int> nBuilds{0};
  std::vector<const int*> results(64);

#pragma omp parallel for num_threads(8) schedule(static, 1)
  for (size_t i = 0; i < results.size(); i++) {
    results[i] = cache
                     .getOrBuild<int>(DESCRIPTOR,
                                      [&]() {
                                        nBuilds++;
                                        return std::make_unique<int>(3);
                                      })
                     .get();
  }

  BOOST_CHECK_EQUAL(nBuilds.load(), 1);
  for (const int* result : results) BOOST_CHECK_EQUAL(result, results[0]);
}

BOOST_AUTO_TEST_CASE(Objects_Are_Deleted_With_The_Cache) {
  std::atomic<int> destroyed{0};
  {
    CrossCallOperatorDataCache cache;
    cache.getOrBuild<Tracked>(DESCRIPTOR, [&]() { return std::make_unique<Tracked>(destroyed); });
    BOOST_CHECK_EQUAL(destroyed.load(), 0);
  }
  BOOST_CHECK_EQUAL(destroyed.load(), 1);
}

BOOST_AUTO_TEST_CASE(Clear_Keeps_Objects_Held_By_Callers) {
  std::atomic<int> destroyed{0};
  CrossCallOperatorDataCache cache;
  const auto build = [&]() { return std::make_unique<Tracked>(destroyed); };

  std::shared_ptr<const Tracked> held = cache.getOrBuild<Tracked>(DESCRIPTOR, build);
  cache.getOrBuild<Tracked>(OTHER_KEY, build);
  cache.clear();
  BOOST_CHECK_EQUAL(cache.size(), 0u);
  BOOST_CHECK(cache.entries().empty());
  BOOST_CHECK_EQUAL(destroyed.load(), 1);  // The object nobody holds

  // The next request builds anew; the held object is still valid
  const std::shared_ptr<const Tracked> rebuilt = cache.getOrBuild<Tracked>(DESCRIPTOR, build);
  BOOST_CHECK_NE(rebuilt.get(), held.get());
  BOOST_CHECK_EQUAL(held->destroyed, &destroyed);
  held.reset();
  BOOST_CHECK_EQUAL(destroyed.load(), 2);
}

BOOST_AUTO_TEST_CASE(Entries_Describe_The_Objects) {
  CrossCallOperatorDataCache cache;
  BOOST_CHECK(cache.entries().empty());
  cache.getOrBuild<Sized>(DESCRIPTOR, []() { return std::make_unique<Sized>(); });
  cache.getOrBuild<double>(OTHER_KEY, []() { return std::make_unique<double>(1.0); });

  const std::vector<CrossCallOperatorDataInfo> entries = cache.entries();
  BOOST_REQUIRE_EQUAL(entries.size(), 2u);
  BOOST_CHECK_EQUAL(entries[0].operatorName, "op");
  BOOST_CHECK_EQUAL(entries[0].description, "data");
  BOOST_CHECK_EQUAL(entries[0].key, "");
  BOOST_CHECK_EQUAL(entries[0].bytes, 1234u);
  BOOST_CHECK_EQUAL(entries[1].key, "other");
  BOOST_CHECK_EQUAL(entries[1].bytes, sizeof(double));
  BOOST_CHECK_EQUAL(cache.memoryBytes(), 1234u + sizeof(double));
}

BOOST_AUTO_TEST_CASE(Concurrent_Get_Clear_And_View) {
  // Readers, clears and views at the same time: every reader sees a valid object of its key, no
  // object is deleted while held (checked by the sanitizers), and the views are consistent.
  CrossCallOperatorDataCache cache;
  std::atomic<int> nBuilds{0};
  std::atomic<int> nWrong{0};

#pragma omp parallel num_threads(8)
  for (int i = 0; i < 2000; i++) {
    if (i % 97 == 0) {
      cache.clear();
    } else if (i % 31 == 0) {
      for (const CrossCallOperatorDataInfo& info : cache.entries()) {
        if (info.bytes != sizeof(std::vector<int>) || info.operatorName != "op") nWrong++;
      }
    } else {
      const CrossCallOperatorDataDescriptor& descriptor = (i % 2 == 0) ? DESCRIPTOR : OTHER_KEY;
      const std::shared_ptr<const std::vector<int>> object =
          cache.getOrBuild<std::vector<int>>(descriptor, [&]() {
            nBuilds++;
            return std::make_unique<std::vector<int>>(100, (i % 2 == 0) ? 1 : 2);
          });
      if (object->size() != 100 || (*object)[99] != ((i % 2 == 0) ? 1 : 2)) nWrong++;
    }
  }

  BOOST_CHECK_EQUAL(nWrong.load(), 0);
  BOOST_CHECK_GE(nBuilds.load(), 2);
  BOOST_CHECK_LE(cache.size(), 2u);
}

BOOST_AUTO_TEST_SUITE_END()
