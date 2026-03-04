// TODO: Delete

// // Copyright (C) 2008-today The SG++ project
// // This file is part of the SG++ project. For conditions of distribution and
// // use, please see the copyright notice provided with SG++ or at
// // sgpp.sparsegrids.org
// #include "sgpp/combigrid/miscellaneous/caching/persistent_cache.hpp"
// #include "sgpp/combigrid/tools/caching/default_serializer.hpp"
// #define BOOST_TEST_DYN_LINK

// #include <boost/test/tools/old/interface.hpp>
// #include <boost/test/unit_test.hpp>
// #include <boost/test/unit_test_log.hpp>
// #include <boost/test/unit_test_suite.hpp>

// #include <memory>
// #include <sgpp/base/tools/RandomNumberGenerator.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/base_cache.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/cache_manager.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/in_memory_cache.hpp>
// #include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <string>

// using namespace sgpp::combigrid;

// static sgpp::base::RandomNumberGenerator& randGen =
//     sgpp::base::RandomNumberGenerator::getInstance();

// BOOST_AUTO_TEST_SUITE(op_createInMemoryCache)

// BOOST_AUTO_TEST_CASE(CacheCreationAndRetrieving) {
//   const CacheId id = tools::fnv1aHash("op_createInMemoryCache/CacheCreationAndRetrieving");

//   BOOST_REQUIRE((misc::CacheManager::getCacheBase<int, int>(id)) == nullptr);

//   const std::shared_ptr<misc::InMemoryCache<int, int>> cache =
//       misc::CacheManager::createInMemoryCache<int, int>(id);

//   const std::shared_ptr<misc::BaseCache> queriedCache =
//       misc::CacheManager::getCacheBase<int, int>(id);

//   BOOST_CHECK(cache != nullptr);
//   BOOST_CHECK(queriedCache != nullptr);
//   BOOST_CHECK(cache == queriedCache);
// }

// BOOST_AUTO_TEST_SUITE(op_createPersistentCache)

// BOOST_AUTO_TEST_CASE(CacheCreationAndRetrieving) {
//   const CacheId id = tools::fnv1aHash("op_createPersistentCache/CacheCreationAndRetrieving");

//   BOOST_REQUIRE((misc::CacheManager::getCacheBase<int, int>(id)) == nullptr);

//   const std::shared_ptr<misc::PersistentLookupCache<int, int>> cache =
//       misc::CacheManager::createPersistentCache(id);

//   const std::shared_ptr<misc::BaseCache> queriedCache =
//       misc::CacheManager::getCacheBase<int, int>(id);

//   BOOST_CHECK(cache != nullptr);
//   BOOST_CHECK(queriedCache != nullptr);
//   BOOST_CHECK(cache == queriedCache);
// }

// BOOST_AUTO_TEST_SUITE_END()