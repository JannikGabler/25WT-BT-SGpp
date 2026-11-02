// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_estimate.hpp
 * @brief Decaying mean of a measured cost (adaptive caching policy).
 */
#pragma once

#include <cstddef>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Mean of the measured cost of one path (ticks), over all windows with weights halving
 * from window to window: follows drifting costs (a growing table, contention) and needs no
 * minimum number of samples per window.
 */
class CostEstimate {
 public:
  /**
   * @brief Adds the samples of a window (decays the earlier ones first).
   * @param mean Mean of the window's samples of this path.
   * @param count Number of these samples (0: only decay).
   */
  void add(const double mean, const size_t count) {
    sum_ = sum_ * DECAY + mean * static_cast<double>(count);
    weight_ = weight_ * DECAY + static_cast<double>(count);
  }

  /// @brief Whether any sample was added.
  bool known() const { return weight_ > 0; }

  /// @brief Returns the mean (0 if unknown).
  double mean() const { return known() ? sum_ / weight_ : 0.0; }

 private:
  static constexpr double DECAY = 0.5;  ///< Weight of the earlier samples per window.
  double sum_ = 0;                      ///< Weighted sum of the samples.
  double weight_ = 0;                   ///< Sum of the weights.
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
