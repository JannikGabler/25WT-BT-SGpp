// TODO: Delete

// // Copyright (C) 2008-today The SG++ project
// // This file is part of the SG++ project. For conditions of distribution and
// // use, please see the copyright notice provided with SG++ or at
// // sgpp.sparsegrids.org
// #include "sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp"
// #include "sgpp/combigrid/tools/os/os.hpp"
// #define BOOST_TEST_DYN_LINK

// #include <boost/test/tools/old/interface.hpp>
// #include <boost/test/unit_test.hpp>
// #include <boost/test/unit_test_log.hpp>
// #include <boost/test/unit_test_suite.hpp>

// #include <memory>
// #include <sgpp/base/tools/RandomNumberGenerator.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/cache_manager.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/in_memory_cache.hpp>
// #include <string>

// using namespace sgpp::combigrid;

// static sgpp::base::RandomNumberGenerator& randGen =
//     sgpp::base::RandomNumberGenerator::getInstance();

// namespace {}  // namespace

// BOOST_AUTO_TEST_CASE(EmptyCache) {
//   const CacheId id = tools::fnv1aHash("EmptyCache");

//   if (tools::isLinux()) {
//     const std::shared_ptr<misc::InMemoryCache<int, int>> cache =
//         misc::CacheManager::createInMemoryCache<int, int>(id);

//     BOOST_CHECK_MESSAGE(cache->size() == 0, "The size of the cache should be 0 but is '" +
//                                                 std::to_string(cache->size()) + "'.");

//     for (int i = -500; i <= 500; i++) {
//       BOOST_REQUIRE_MESSAGE(!cache->contains(i),
//                             "Cache should not contain '" + std::to_string(i) + "'.");
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(SmallSingleItem) {
//   const CacheId id = tools::fnv1aHash("SmallSingleItem");
//   if (tools::isLinux()) {
//     randGen.setSeed();
//     BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
//       const int key = -10 + static_cast<int>(randGen.getUniformIndexRN(21));    // -10...+10
//       const int value = -10 + static_cast<int>(randGen.getUniformIndexRN(21));  // -10...+10

//       const std::shared_ptr<misc::InMemoryCache<int, int>> cache =
//           misc::CacheManager::createInMemoryCache<int, int>(id);

//       cache->put(key, value);

//       BOOST_CHECK_MESSAGE(cache->size() == 1, "The size of the cache should be 1 but is '" +
//                                                   std::to_string(cache->size()) + "'.");

//       int result;
//       for (int i = -500; i <= 500; i++) {
//         if (i == key) {
//           BOOST_REQUIRE(cache->contains(i));
//           BOOST_REQUIRE(cache->get(i, result));
//           BOOST_REQUIRE_MESSAGE(result == value, "The result should have been '" +
//                                                      std::to_string(value) + "' but is '" +
//                                                      std::to_string(result) + "'.");
//         } else {
//           BOOST_REQUIRE_MESSAGE(!cache->contains(i),
//                                 "Cache should not contain '" + std::to_string(i) + "'.");
//           BOOST_REQUIRE(!cache->get(i, result));
//         }
//       }
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(LargeSingleItem) {
//   const CacheId id = tools::fnv1aHash("LargeSingleItem");

//   if (tools::isLinux()) {
//     randGen.setSeed();
//     BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
//       const int key = -1000 + static_cast<int>(randGen.getUniformIndexRN(2001));  //
//       -1000...+1000 const int value = -10 + static_cast<int>(randGen.getUniformIndexRN(2001)); //
//       -1000...+1000

//       const std::shared_ptr<misc::InMemoryCache<int, int>> cache =
//           misc::CacheManager::createInMemoryCache<int, int>(id);

//       cache->put(key, value);

//       BOOST_CHECK_MESSAGE(cache->size() == 1, "The size of the cache should be 1 but is '" +
//                                                   std::to_string(cache->size()) + "'.");

//       int result;
//       for (int i = -5000; i <= 5000; i++) {
//         if (i == key) {
//           BOOST_REQUIRE(cache->contains(i));
//           BOOST_REQUIRE(cache->get(i, result));
//           BOOST_REQUIRE_MESSAGE(result == value, "The result should have been '" +
//                                                      std::to_string(value) + "' but is '" +
//                                                      std::to_string(result) + "'.");
//         } else {
//           BOOST_REQUIRE_MESSAGE(!cache->contains(i),
//                                 "Cache should not contain '" + std::to_string(i) + "'.");
//           BOOST_REQUIRE(!cache->get(i, result));
//         }
//       }
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(ManyItems) {
//   const CacheId id = tools::fnv1aHash("ManyItems");

//   if (tools::isLinux()) {
//     randGen.setSeed();
//     BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
//       const int key = -1000 + static_cast<int>(randGen.getUniformIndexRN(2001));  //
//       -1000...+1000 const int value = -10 + static_cast<int>(randGen.getUniformIndexRN(2001)); //
//       -1000...+1000

//       const std::shared_ptr<misc::InMemoryCache<int, int>> cache =
//           misc::CacheManager::createInMemoryCache<int, int>(id);

//       cache->put(key, value);

//       BOOST_CHECK_MESSAGE(cache->size() == 1, "The size of the cache should be 1 but is '" +
//                                                   std::to_string(cache->size()) + "'.");

//       int result;
//       for (int i = -5000; i <= 5000; i++) {
//         if (i == key) {
//           BOOST_REQUIRE(cache->contains(i));
//           BOOST_REQUIRE(cache->get(i, result));
//           BOOST_REQUIRE_MESSAGE(result == value, "The result should have been '" +
//                                                      std::to_string(value) + "' but is '" +
//                                                      std::to_string(result) + "'.");
//         } else {
//           BOOST_REQUIRE_MESSAGE(!cache->contains(i),
//                                 "Cache should not contain '" + std::to_string(i) + "'.");
//           BOOST_REQUIRE(!cache->get(i, result));
//         }
//       }
//     }
//   }
// }