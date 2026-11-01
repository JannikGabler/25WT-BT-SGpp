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

namespace misc {

namespace cache_file_exception {

/// @brief Holds the message; a base class, so that it is constructed before the exception.
struct MessageHolder {
  std::string message;
};

}  // namespace cache_file_exception

/**
 * @brief A @c base::file_exception that owns its message (@c base::file_exception keeps only
 * the pointer it receives, so a composed message would dangle). Caught as
 * @c base::file_exception.
 */
class CacheFileException : private cache_file_exception::MessageHolder,
                           public base::file_exception {
 public:
  /**
   * @brief Creates the exception.
   * @param message Message (copied).
   */
  explicit CacheFileException(std::string message)
      : MessageHolder{std::move(message)}, base::file_exception(this->message.c_str()) {}

  CacheFileException(const CacheFileException& other)
      : MessageHolder(other), base::file_exception(this->message.c_str()) {}

  CacheFileException& operator=(const CacheFileException& other) {
    message = other.message;
    msg = message.c_str();
    return *this;
  }
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
