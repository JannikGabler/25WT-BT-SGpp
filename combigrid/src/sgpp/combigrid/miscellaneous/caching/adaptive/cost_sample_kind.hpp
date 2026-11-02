// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_sample_kind.hpp
 * @brief Path of a timed evaluation of a cached function with the adaptive policy.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/// @brief Path of a timed evaluation (@c CostSample) of a cached function (adaptive policy).
enum class CostSampleKind : uint8_t {
  MemoryHit,   ///< Answered from memory.
  MemoryMiss,  ///< Not in memory, the disk not asked: the function was evaluated.
  DiskHit,     ///< Not in memory, answered from the temporary file.
  DiskMiss,    ///< Neither in memory nor in the temporary file: the function was evaluated.
};

/// Number of values of @c CostSampleKind.
constexpr size_t COST_SAMPLE_KIND_CNT = 4;

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
