// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cost_timer.hpp
 * @brief Timer of the cost measurements of the adaptive caching policy.
 */
#pragma once

#include <chrono>
#include <cstdint>

#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
#include <x86intrin.h>
/// Defined if @c CostTimer reads the time-stamp counter (@c rdtscp), else @c std::chrono.
#define SGPP_COMBIGRID_COST_TIMER_RDTSCP 1
#endif

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Timer of the sampled cost measurements of the adaptive caching policy.
 *
 * On x86-64 the time-stamp counter (@c rdtscp, WP-27: 37 ns per read); it also returns the CPU,
 * so that intervals during which the thread moved to another CPU are dropped (time-stamp
 * counters of different CPUs may differ: the laptop of WP-27 has a warp of 1.5 s between CPUs).
 * Elsewhere @c std::chrono::steady_clock (in ns), whose cost depends on the clock source of the
 * system (WP-27: 1.9 us with HPET, ~20 ns with the TSC). The costs are only compared with each
 * other, so the tick rate matters only for the statistics. Calibrated once per process (< 1 ms):
 * the cost of a read, the tick rate, and from them the sampling period.
 */
class CostTimer {
 public:
  /**
   * @brief Reads the timer.
   * @param cpu Output: the CPU of the read (two reads are comparable only if equal; always 0
   * without @c rdtscp).
   * @return The ticks.
   */
  static uint64_t read(uint32_t& cpu) {
#ifdef SGPP_COMBIGRID_COST_TIMER_RDTSCP
    unsigned int aux = 0;
    const uint64_t ticks = __rdtscp(&aux);
    cpu = aux;
    return ticks;
#else
    cpu = 0;
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                     std::chrono::steady_clock::now().time_since_epoch())
                                     .count());
#endif
  }

  /// @brief Returns the cost of one read in ticks (the interval of two back-to-back reads).
  static double readCostTicks();

  /// @brief Returns the ticks per nanosecond.
  static double ticksPerNs();

  /**
   * @brief Returns log2 of the sampling period P: one evaluation in P is timed, so that the
   * timer costs at most @c constants::source_func_cache::ADAPTIVE_TIMER_BUDGET_NS per
   * evaluation (two reads per sample; P in [2^4, 2^16]).
   * @return The shift, or the value set by @ref overrideSamplingShift.
   */
  static unsigned samplingShift();

  /**
   * @brief Sets the sampling shift of caches created afterwards (tests: 0 times every
   * evaluation); a negative value restores the calibrated one.
   * @param shift The shift.
   */
  static void overrideSamplingShift(int shift);
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
