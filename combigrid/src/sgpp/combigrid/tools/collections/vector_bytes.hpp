// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_bytes.hpp
 * @brief @c tools::vectorBytes: the heap memory of a @c std::vector.
 */
#pragma once

#include <cstddef>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {

/// @brief Returns the heap memory of @p vector in bytes (its capacity; elements without heap
/// memory of their own).
template <typename T>
size_t vectorBytes(const std::vector<T>& vector) noexcept {
  return vector.capacity() * sizeof(T);
}

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
