// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_sample_slot.hpp
 * @brief Slot of a @c CostSampleWindow.
 */
#pragma once

#include <atomic>
#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Slot of a @c CostSampleWindow, written by one thread and read by the thread that
 * completes the window (a sequence lock: @c stamp is 0 while the slot is written).
 */
struct CostSampleSlot {
  /// 0 while written, else ((window + 1) << 8) | kind.
  std::atomic<uint64_t> stamp{0};
  std::atomic<uint64_t> totalTicks{0};     ///< @c CostSample::totalTicks.
  std::atomic<uint64_t> functionTicks{0};  ///< @c CostSample::functionTicks.
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
