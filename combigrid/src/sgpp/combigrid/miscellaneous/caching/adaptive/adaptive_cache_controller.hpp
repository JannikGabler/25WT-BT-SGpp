// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_cache_controller.hpp
 * @brief State machine of the adaptive caching policy (@c CachePolicy::Adaptive).
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_counters.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rule_input.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_estimate.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_window.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/hit_rate_tracker.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief State machine of @c CachePolicy::Adaptive: picks the @c AdaptiveCacheMode (None,
 * MemoryOnly, TemporaryStorage) of a @c SourceFuncCache from sampled cost measurements.
 *
 * - Measuring: the cached function times one evaluation in P (points whose hash has its bits
 *   32.. 32 + log2 P zero; P from @c CostTimer::samplingShift) and reports it with @ref
 *   addSample: hits, misses (whole evaluation and the function alone), per tier.
 * - Deciding: every @c ADAPTIVE_WINDOW_SAMPLES samples, the thread that completes the window
 *   calls @ref decide (skipped if another decision runs): trimmed means per path update the
 *   decaying cost estimates, the exact counters of the table the marginal hit rate (used from
 *   @c ADAPTIVE_WARMUP_LOOKUPS lookups on, over the last @c ADAPTIVE_HIT_RATE_LOOKUPS), then
 *   @c adaptive_caching::nextAdaptiveMode. Switches take effect at once, also in the middle of
 *   an operator call (results are bitwise the same in every mode).
 * - None measures nothing (the function is called directly); it is left by probes at the
 *   end-of-operator hook (@ref finishOperatorCall): after k operator calls in None (k = 1, 2,
 *   4, ... @c ADAPTIVE_MAX_PROBE_INTERVAL; reset by a probe that stays), the next call runs in
 *   MemoryOnly; it stays only if caching wins by @c ADAPTIVE_FROM_NONE_FACTOR (in its windows,
 *   and at its end with the exact hit rate of the call).
 *
 * Thread-safe: @ref mode and @ref addSample are lock-free (called per evaluation), the rest
 * takes a mutex.
 */
class AdaptiveCacheController {
 public:
  /**
   * @brief Creates the controller in mode MemoryOnly.
   * @param diskAvailable Whether TemporaryStorage can be used (LMDB).
   * @param samplingShift log2 of the sampling period P.
   * @param readCostTicks Cost of a timer read (subtracted from the samples).
   */
  AdaptiveCacheController(bool diskAvailable, unsigned samplingShift, double readCostTicks);

  /// @brief Returns the current mode (per evaluation; relaxed).
  AdaptiveCacheMode mode() const { return mode_.load(std::memory_order_relaxed); }

  /// @brief Whether the evaluation of a point with this hash is timed.
  bool sampled(const uint64_t hash) const { return ((hash >> 32) & samplingMask_) == 0; }

  /**
   * @brief Records a timed evaluation.
   * @param sample The sample (raw ticks).
   * @return The number of the window it completed: the caller then calls @ref decide.
   */
  std::optional<uint64_t> addSample(const adaptive_caching::CostSample& sample) {
    return window_.add(sample);
  }

  /**
   * @brief Window decision: updates the estimates and applies the rules (skipped if another
   * decision or hook runs).
   * @param window   Number of the completed window.
   * @param counters Exact counters of the table.
   */
  void decide(uint64_t window, const adaptive_caching::AdaptiveCounters& counters);

  /**
   * @brief End-of-operator hook: counts calls in None, starts and finishes probes.
   * @param counters Exact counters of the table.
   */
  void finishOperatorCall(const adaptive_caching::AdaptiveCounters& counters);

  /// @brief No temporary file can be used (its creation failed): leaves TemporaryStorage.
  void disableDisk();

  /// @brief Starts over (cache cleared): MemoryOnly, no measurements, statistics 0.
  void reset();

  /**
   * @brief Fills the adaptive fields of @p stats.
   * @param stats      Statistics.
   * @param ticksPerNs Timer ticks per ns.
   */
  void addStatistics(CacheStatistics& stats, double ticksPerNs) const;

  /// @brief Returns the measured state (as the rules see it; for tests).
  adaptive_caching::AdaptiveRuleInput ruleInput(
      const adaptive_caching::AdaptiveCounters& counters) const;

 private:
  /// @name Hot (read per evaluation; own cache line)
  /// @{
  alignas(64) std::atomic<AdaptiveCacheMode> mode_{AdaptiveCacheMode::MemoryOnly};
  uint64_t samplingMask_;  ///< P - 1 (shifted by 32 in @ref sampled).
  /// @}

  unsigned samplingShift_;                     ///< log2 P.
  double readCostTicks_;                       ///< Cost of a timer read.
  adaptive_caching::CostSampleWindow window_;  ///< Samples of the current window.

  /// @name Guarded by @c mutex_
  /// @{
  mutable std::mutex mutex_;
  adaptive_caching::CostEstimate functionCost_;        ///< Function call alone.
  adaptive_caching::CostEstimate memoryHitCost_;       ///< Hit in memory.
  adaptive_caching::CostEstimate memoryMissOverhead_;  ///< Miss without disk, minus function.
  adaptive_caching::CostEstimate diskHitCost_;         ///< Hit in the file.
  adaptive_caching::CostEstimate diskMissOverhead_;    ///< Miss with disk, minus function.
  adaptive_caching::HitRateTracker memoryHitRate_;     ///< Exact counters of the table.
  adaptive_caching::HitRateTracker diskHitRate_;       ///< Sampled disk queries (times P).
  size_t diskQueries_ = 0;                             ///< Disk queries (samples times P).
  size_t diskHits_ = 0;                                ///< Disk hits (samples times P).
  bool diskAvailable_;                                 ///< Whether TemporaryStorage can be used.
  bool probing_ = false;      ///< Whether the current MemoryOnly is a probe from None.
  size_t probeInterval_ = 1;  ///< Operator calls in None before the next probe.
  size_t callsInNone_ = 0;    ///< Operator calls in None since the last probe.
  /// Whether None was entered since the last hook (that call does not count as one in None).
  bool switchedOffInCall_ = false;
  size_t probeStartLookups_ = 0;  ///< Table lookups when the probe started.
  size_t probeStartHits_ = 0;     ///< Table hits when the probe started.
  size_t modeSwitches_ = 0;       ///< Mode changes.
  size_t noneOperatorCalls_ = 0;  ///< Operator calls that ended in None.
  /// @}

  /// @brief Updates the estimates with the samples of a window (locked).
  void update(const std::vector<adaptive_caching::CostSample>& samples);

  /// @brief Applies the rules (locked).
  void apply(const adaptive_caching::AdaptiveCounters& counters);

  /// @brief Sets the mode, counts the switch, ends a probe that switched off (locked).
  void setMode(AdaptiveCacheMode mode);

  /// @brief Starts a probe from None (locked).
  void startProbe(const adaptive_caching::AdaptiveCounters& counters);

  /// @brief Finishes a probe with the exact hit rate of its operator call (locked).
  void finishProbe(const adaptive_caching::AdaptiveCounters& counters);
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
