// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_sample.hpp
 * @brief One timed evaluation of a cached function with the adaptive policy.
 */
#pragma once

#include <cstdint>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_kind.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief One timed evaluation of a cached function (adaptive policy), in ticks of
 * @c CostTimer, as read (the costs of the timer reads are not subtracted yet).
 */
struct CostSample {
  CostSampleKind kind = CostSampleKind::MemoryHit;  ///< Path of the evaluation.
  uint64_t totalTicks = 0;     ///< Whole evaluation: hash, lookups, function call, insert.
  uint64_t functionTicks = 0;  ///< The function call alone (misses only, else 0).
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
