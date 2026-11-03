// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file checked_narrow.hpp
 * @brief Conversion of a size to a narrower unsigned integer type that throws if the size does not
 * fit (for compact index types, e.g. 32-bit indices of the delta quadrature tries).
 */
#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Returns @p value as @c T.
 * @param value   Value to convert.
 * @param message Message of the exception.
 * @return @p value.
 * @throws std::overflow_error with @p message if @p value exceeds the maximum of @c T.
 */
template <std::unsigned_integral T>
T checkedNarrow(const std::size_t value, const char* const message) {
  if (value > std::numeric_limits<T>::max()) {
    throw std::overflow_error(message);
  }

  return static_cast<T>(value);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
