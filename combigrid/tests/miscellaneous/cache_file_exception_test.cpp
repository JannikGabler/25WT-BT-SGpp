// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cache_file_exception_test.cpp
 * @brief Boost.Test cases for @c CacheFileException: it owns its message (composed messages do
 * not dangle), copies and assignments carry their own copy, and it is caught as
 * @c base::file_exception and @c std::exception (also through an @c std::exception_ptr).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <exception>
#include <memory>
#include <optional>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/miscellaneous/exceptions/cache_file_exception.hpp>
#include <string>

using sgpp::combigrid::CacheFileException;

BOOST_AUTO_TEST_SUITE(cache_file_exception)

BOOST_AUTO_TEST_CASE(owns_a_composed_message) {
  std::optional<CacheFileException> e;
  {
    std::string message = "SourceFuncCache: '" + std::string("/some/file") + "' is truncated";
    e.emplace(message);
    message.assign(message.size(), 'x');  // overwrite the buffer the message came from
  }
  BOOST_CHECK_EQUAL(std::string(e->what()), "SourceFuncCache: '/some/file' is truncated");
}

BOOST_AUTO_TEST_CASE(copies_own_their_message) {
  auto original = std::make_unique<CacheFileException>(std::string(100, 'a'));
  const CacheFileException copied(*original);
  CacheFileException assigned(std::string("short"));
  assigned = *original;
  original.reset();
  BOOST_CHECK_EQUAL(std::string(copied.what()), std::string(100, 'a'));
  BOOST_CHECK_EQUAL(std::string(assigned.what()), std::string(100, 'a'));

  CacheFileException self(std::string("self"));
  const CacheFileException& alias = self;
  self = alias;
  BOOST_CHECK_EQUAL(std::string(self.what()), "self");
}

BOOST_AUTO_TEST_CASE(is_caught_as_file_exception) {
  BOOST_CHECK_THROW(throw CacheFileException("a"), sgpp::base::file_exception);
  BOOST_CHECK_THROW(throw CacheFileException("a"), std::exception);

  std::exception_ptr pointer;
  try {
    throw CacheFileException("SourceFuncCache: rethrown " + std::to_string(42));
  } catch (...) {
    pointer = std::current_exception();
  }
  try {
    std::rethrow_exception(pointer);
  } catch (const sgpp::base::file_exception& e) {
    BOOST_CHECK_EQUAL(std::string(e.what()), "SourceFuncCache: rethrown 42");
  }
}

BOOST_AUTO_TEST_SUITE_END()
