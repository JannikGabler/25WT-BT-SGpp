// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_sample_window.hpp
 * @brief Lock-free collection of the timed evaluations of a cached function in windows of
 * @c constants::source_func_cache::ADAPTIVE_WINDOW_SAMPLES samples (adaptive policy).
 */
#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <optional>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_slot.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Collects the samples of all threads in consecutive windows of W samples
 * (@c constants::source_func_cache::ADAPTIVE_WINDOW_SAMPLES) without a lock: one atomic counter
 * assigns the slots, the thread that takes the last slot of a window collects it.
 *
 * Samples whose slot is still being written when the window is collected are skipped, as are
 * the rare samples whose slot was already reused by the next window (each slot is a sequence
 * lock); a sample costs one atomic increment and four atomic stores.
 */
class CostSampleWindow {
 public:
  /// Samples per window.
  static constexpr uint64_t SIZE = constants::source_func_cache::ADAPTIVE_WINDOW_SAMPLES;

  /**
   * @brief Adds a sample.
   * @param sample The sample.
   * @return The number of the window it completed (the caller collects it), else nothing.
   */
  std::optional<uint64_t> add(const CostSample& sample);

  /**
   * @brief Returns the samples of a window that are completely written.
   * @param window Number of the window (from @ref add).
   * @return Its samples (at most @ref SIZE).
   */
  std::vector<CostSample> collect(uint64_t window) const;

  /// @brief Discards all samples (the next sample starts a new window).
  void clear();

 private:
  /// Samples added so far (assigns the slots; not in the cache line of the data that is read
  /// per evaluation).
  alignas(64) std::atomic<uint64_t> count_{0};
  std::array<CostSampleSlot, SIZE> slots_{};  ///< The slots of the current window.
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
