// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <exception>
#include <sgpp/globaldef.hpp>

namespace sgpp {
namespace base {

/**
 * Exception that is thrown in case of missing dependencies.
 */
class dependencies_missing_exception : public std::exception {
 public:
  /**
   * Constructor
   *
   * @param msg the exception message
   */
  explicit dependencies_missing_exception(const char* msg) noexcept : msg(msg) {}

  /**
   * Standard Constructor
   */
  dependencies_missing_exception() noexcept : msg(nullptr) {}

  /**
   * Destructor
   */
  ~dependencies_missing_exception() noexcept override {}

  /**
   * throw method that have to be implemented
   *
   * @return returns the message specified in the constructor otherwise a general text
   */
  const char* what() const noexcept override {
    if (msg) {
      return msg;
    } else {
      return "dependencies_missing_exception: general failure";
    }
  }

 protected:
  /// the exception message
  const char* msg;
};

}  // namespace base
}  // namespace sgpp
