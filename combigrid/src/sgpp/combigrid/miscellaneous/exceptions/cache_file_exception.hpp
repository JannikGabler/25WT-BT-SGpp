// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cache_file_exception.hpp
 * @brief @c base::file_exception with a message it owns (file errors of the source-function
 * cache name the file).
 */
#pragma once

#include <sgpp/base/exception/file_exception.hpp>
#include <string>
#include <utility>

namespace sgpp {
namespace combigrid {

/**
 * @brief A @c base::file_exception that owns its message (@c base::file_exception keeps only
 * the pointer it receives, so a composed message would dangle). Caught as
 * @c base::file_exception.
 *
 * The base class is constructed before the member that holds the message, so every constructor
 * points the base's message at the member afterwards.
 */
class CacheFileException : public base::file_exception {
 public:
  /**
   * @brief Creates the exception.
   * @param message Message (copied).
   */
  explicit CacheFileException(std::string message) : message_(std::move(message)) {
    msg = message_.c_str();
  }

  CacheFileException(const CacheFileException& other)
      : base::file_exception(other), message_(other.message_) {
    msg = message_.c_str();
  }

  CacheFileException& operator=(const CacheFileException& other) {
    message_ = other.message_;
    msg = message_.c_str();
    return *this;
  }

 private:
  std::string message_;  ///< The message (@c msg points to it).
};

}  // namespace combigrid
}  // namespace sgpp
