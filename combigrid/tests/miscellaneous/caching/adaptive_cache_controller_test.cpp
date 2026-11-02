// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_cache_controller_test.cpp
 * @brief Boost.Test cases for the adaptive caching policy with injected cost measurements
 * (deterministic): the switching rules (@c misc::adaptive_caching::nextAdaptiveMode), the
 * state machine (@c misc::AdaptiveCacheController: windows, cold start, hysteresis, probes with
 * backoff, the temporary-file tier, outliers, statistics), the hit-rate tracker and the
 * lock-free sample window.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_cache_controller.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_counters.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rule_input.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rules.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_window.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/hit_rate_tracker.hpp>
#include <thread>
#include <vector>

using sgpp::combigrid::AdaptiveCacheMode;
using sgpp::combigrid::CacheStatistics;
using sgpp::combigrid::misc::AdaptiveCacheController;
using sgpp::combigrid::misc::adaptive_caching::AdaptiveCounters;
using sgpp::combigrid::misc::adaptive_caching::AdaptiveRuleInput;
using sgpp::combigrid::misc::adaptive_caching::CostSample;
using sgpp::combigrid::misc::adaptive_caching::CostSampleKind;
using sgpp::combigrid::misc::adaptive_caching::CostSampleWindow;
using sgpp::combigrid::misc::adaptive_caching::expectedOverhead;
using sgpp::combigrid::misc::adaptive_caching::HitRateTracker;
using sgpp::combigrid::misc::adaptive_caching::nextAdaptiveMode;

namespace cfg = sgpp::combigrid::constants::source_func_cache;

namespace {

constexpr size_t WINDOW = CostSampleWindow::SIZE;

/// Injected costs (ticks) of one kind of evaluation.
struct Costs {
  uint64_t hit = 100;       ///< Total of a memory hit.
  uint64_t overhead = 150;  ///< Overhead of a miss (without the function).
  uint64_t function = 20;   ///< The function.
};

/// A controller that times every evaluation (shift 0) with a free timer; simulates the table's
/// exact counters.
class Simulation {
 public:
  explicit Simulation(const bool diskAvailable = true) : controller(diskAvailable, 0, 0.0) {}

  /// @p n evaluations, a share @p hitRate of them memory hits (evenly spread).
  void evaluate(const size_t n, const double hitRate, const Costs& costs) {
    size_t phaseHits = 0;
    for (size_t i = 0; i < n; i++) {
      const bool hit = static_cast<double>(phaseHits + 1) <= hitRate * static_cast<double>(i + 1);
      lookups++;
      if (hit) {
        hits++;
        phaseHits++;
      }
      add(hit ? CostSample{CostSampleKind::MemoryHit, costs.hit, 0}
              : CostSample{CostSampleKind::MemoryMiss, costs.overhead + costs.function,
                           costs.function});
    }
  }

  /// Up to @p n evaluations in the temporary-file tier (all memory misses), a share hit on disk;
  /// stops when the mode leaves TemporaryStorage.
  void evaluateDisk(const size_t n, const double diskHitRate, const uint64_t diskHit,
                    const uint64_t diskMissOverhead, const uint64_t function) {
    size_t diskHits = 0;
    for (size_t i = 0; i < n && mode() == AdaptiveCacheMode::TemporaryStorage; i++) {
      const bool hit =
          static_cast<double>(diskHits + 1) <= diskHitRate * static_cast<double>(i + 1);
      lookups++;
      if (hit) diskHits++;
      add(hit ? CostSample{CostSampleKind::DiskHit, diskHit, 0}
              : CostSample{CostSampleKind::DiskMiss, diskMissOverhead + function, function});
    }
  }

  void finishOperatorCall() { controller.finishOperatorCall(counters()); }

  AdaptiveCounters counters() const { return AdaptiveCounters{lookups, hits, memoryFull}; }

  AdaptiveCacheMode mode() const { return controller.mode(); }

  AdaptiveCacheController controller;
  size_t lookups = 0;
  size_t hits = 0;
  bool memoryFull = false;

 private:
  void add(const CostSample& sample) {
    if (controller.mode() == AdaptiveCacheMode::None) {
      return;  // the function is called directly: nothing is measured
    }
    if (const auto window = controller.addSample(sample)) {
      controller.decide(*window, counters());
    }
  }
};

}  // namespace

BOOST_AUTO_TEST_SUITE(adaptive_cache_rules)

BOOST_AUTO_TEST_CASE(expected_overhead_per_avoided_evaluation) {
  BOOST_CHECK_EQUAL(expectedOverhead(80, 150, -1), 80);  // unknown hit rate: bound h = 1
  BOOST_CHECK_EQUAL(expectedOverhead(80, -1, 0.5), 80);  // unknown miss overhead: bound
  BOOST_CHECK_EQUAL(expectedOverhead(80, 150, 1.0), 80);
  BOOST_CHECK_CLOSE(expectedOverhead(80, 150, 0.5), 230, 1e-12);
  BOOST_CHECK(std::isinf(expectedOverhead(80, 150, 0.0)));
}

BOOST_AUTO_TEST_CASE(memory_tier_rules_with_hysteresis) {
  AdaptiveRuleInput in;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);  // nothing measured
  in.memoryHitCost = 80;
  in.functionCost = 20;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::None);  // even all hits lose
  in.functionCost = 100;                                         // 100 >= 0.8 * 80
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);
  in.memoryMissOverhead = 150;
  in.memoryHitRate = 0.5;  // E = 230: 100 < 0.8 * 230
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::None);
  in.memoryHitRate = 0.9;  // E = 96.7: 100 >= 77.3
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);

  // A probe from None stays only if caching wins by 1.25: 100 <= 1.25 * 96.7.
  in.probing = true;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::None);
  in.functionCost = 121;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);

  in.mode = AdaptiveCacheMode::None;  // only probes leave None
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::None);
}

BOOST_AUTO_TEST_CASE(disk_tier_rules_with_hysteresis) {
  AdaptiveRuleInput in;
  in.memoryHitCost = 80;
  in.functionCost = 1000;
  in.diskAvailable = true;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);  // memory not full
  in.memoryFull = true;  // no disk measurement yet: E_memory = 80 as the bound
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::TemporaryStorage);
  in.diskAvailable = false;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);

  in.diskAvailable = true;
  in.diskHitCost = 700;  // to disk needs > 1050, back needs <= 840
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);
  in.mode = AdaptiveCacheMode::TemporaryStorage;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::TemporaryStorage);  // 1000 > 840
  in.diskMissOverhead = 900;
  in.diskHitRate = 0.5;  // E_disk = 1600: 1000 <= 1920
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);
  in.diskHitRate = 0.0;
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);
  in.diskHitRate = -1;
  in.functionCost = 50;  // the file in use: to None only via MemoryOnly
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::MemoryOnly);
  in.memoryFull = false;  // the file not in use: the memory rule (50 < 0.8 * 80)
  BOOST_CHECK(nextAdaptiveMode(in) == AdaptiveCacheMode::None);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(adaptive_cache_controller)

BOOST_AUTO_TEST_CASE(cheap_function_is_switched_off_in_the_first_window) {
  Simulation sim;
  sim.evaluate(WINDOW - 1, 0.0, Costs{});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);  // no window complete yet
  sim.evaluate(1, 0.0, Costs{});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);
}

BOOST_AUTO_TEST_CASE(cold_start_keeps_caching_until_the_warm_up) {
  // Mid-cost function (500 ticks vs. hit 100, miss overhead 150): at the cold start's hit rate
  // of 0.1 it would lose (E = 1450), at the final 0.8 it wins (E = 137.5).
  Simulation sim;
  const Costs costs{100, 150, 500};
  sim.evaluate(cfg::ADAPTIVE_WARMUP_LOOKUPS / 2, 0.1, costs);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.evaluate(cfg::ADAPTIVE_WARMUP_LOOKUPS / 2 + 4 * cfg::ADAPTIVE_HIT_RATE_LOOKUPS, 0.8, costs);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);

  // After the warm-up the marginal hit rate decides: at 0.1 it switches off.
  sim.evaluate(4 * cfg::ADAPTIVE_HIT_RATE_LOOKUPS, 0.1, costs);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);
}

BOOST_AUTO_TEST_CASE(expensive_function_stays_on) {
  Simulation sim;
  sim.evaluate(2 * cfg::ADAPTIVE_WARMUP_LOOKUPS, 0.5, Costs{100, 150, 10000});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.finishOperatorCall();
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);

  CacheStatistics stats;
  sim.controller.addStatistics(stats, 2.0);
  BOOST_CHECK(stats.adaptiveMode == AdaptiveCacheMode::MemoryOnly);
  BOOST_CHECK_EQUAL(stats.modeSwitches, 0);
  BOOST_CHECK_CLOSE(stats.functionCostNs, 5000, 1e-9);
  BOOST_CHECK_CLOSE(stats.memoryHitCostNs, 50, 1e-9);
  BOOST_CHECK_CLOSE(stats.memoryMissOverheadNs, 75, 1e-9);
}

BOOST_AUTO_TEST_CASE(probes_back_off_exponentially) {
  Simulation sim;
  sim.evaluate(WINDOW, 0.0, Costs{});
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::None);
  sim.finishOperatorCall();  // the call that switched off does not count as one in None
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);

  // Failing probes: after 1, 2, 4, ... operator calls in None, up to the maximum interval.
  size_t expectedInterval = 1;
  for (int probe = 0; probe < 9; probe++) {
    for (size_t call = 1; call < expectedInterval; call++) {
      sim.finishOperatorCall();
      BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);
    }
    sim.finishOperatorCall();
    BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);  // the probe call
    sim.evaluate(WINDOW, 0.0, Costs{});                        // its first window fails
    BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);
    sim.finishOperatorCall();  // end of the probe call
    expectedInterval = std::min(2 * expectedInterval, cfg::ADAPTIVE_MAX_PROBE_INTERVAL);
  }

  CacheStatistics stats;
  sim.controller.addStatistics(stats, 1.0);
  BOOST_CHECK_EQUAL(stats.modeSwitches, 1 + 2 * 9);
}

BOOST_AUTO_TEST_CASE(successful_probe_resets_the_backoff) {
  // Expensive misses (contention, a full shard) make caching lose: no hits, so the miss
  // overhead (1000) stands in for the hit cost.
  Simulation sim;
  const Costs contended{100, 1000, 500};
  sim.evaluate(WINDOW, 0.0, contended);
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::None);
  sim.finishOperatorCall();  // end of the call that switched off
  sim.finishOperatorCall();  // probe 1 fails
  sim.evaluate(WINDOW, 0.0, contended);
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::None);
  sim.finishOperatorCall();  // end of probe 1
  sim.finishOperatorCall();
  sim.finishOperatorCall();  // probe 2 (interval 2)
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::MemoryOnly);

  // The contention is gone: the probe stays at the end of its call (exact hit rate 0.8).
  sim.evaluate(2 * WINDOW, 0.8, Costs{100, 150, 500});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.finishOperatorCall();
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);

  // Not a probe any more: the band of MemoryOnly applies (stays while 110 >= 0.8 * 100; a probe
  // would need 110 > 1.25 * 100).
  sim.evaluate(8 * WINDOW, 0.5, Costs{100, 150, 110});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.finishOperatorCall();
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
}

BOOST_AUTO_TEST_CASE(probe_decided_at_the_end_of_its_call) {
  Simulation sim;
  sim.evaluate(WINDOW, 0.0, Costs{100, 1000, 130});
  sim.finishOperatorCall();
  sim.finishOperatorCall();  // probe
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  // Function 130 vs. hit 100: the bound holds in the window (130 > 1.25 * 100), but at the end
  // the call's hit rate of 0.5 gives E = 675 (miss overhead still 575 after one window).
  sim.evaluate(WINDOW, 0.5, Costs{100, 150, 130});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.finishOperatorCall();
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::None);
}

BOOST_AUTO_TEST_CASE(temporary_storage_once_memory_is_full) {
  Simulation sim;
  const Costs costs{100, 150, 5000};
  sim.evaluate(WINDOW, 0.5, costs);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  sim.memoryFull = true;
  sim.evaluate(WINDOW, 0.5, costs);
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::TemporaryStorage);

  // Disk hits of 2000 ticks: 5000 > 1.2 * 2000, stays.
  sim.evaluateDisk(4 * WINDOW, 1.0, 2000, 2500, 5000);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::TemporaryStorage);
  // Disk hits of 4500 ticks: 5000 <= 1.2 * 4500, back to memory only.
  sim.evaluateDisk(8 * WINDOW, 1.0, 4500, 2500, 5000);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
  // Not again: the measured disk cost needs 5000 > 1.5 * E_disk.
  sim.evaluate(4 * WINDOW, 0.5, costs);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
}

BOOST_AUTO_TEST_CASE(disk_hit_rate_decides_after_its_warm_up) {
  Simulation sim;
  sim.memoryFull = true;
  sim.evaluate(WINDOW, 0.5, Costs{100, 150, 5000});
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::TemporaryStorage);
  // Spilled values are never reused: cheap disk queries, but hit rate 0 after the warm-up.
  sim.evaluateDisk(2 * cfg::ADAPTIVE_WARMUP_LOOKUPS, 0.0, 1000, 1000, 5000);
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);
}

BOOST_AUTO_TEST_CASE(without_a_disk_never_temporary_storage) {
  Simulation sim(false);
  sim.memoryFull = true;
  sim.evaluate(4 * WINDOW, 0.5, Costs{100, 150, 5000});
  BOOST_CHECK(sim.mode() == AdaptiveCacheMode::MemoryOnly);

  Simulation failing;
  failing.memoryFull = true;
  failing.evaluate(WINDOW, 0.5, Costs{100, 150, 5000});
  BOOST_REQUIRE(failing.mode() == AdaptiveCacheMode::TemporaryStorage);
  failing.controller.disableDisk();  // the temporary file could not be created
  BOOST_CHECK(failing.mode() == AdaptiveCacheMode::MemoryOnly);
  failing.evaluate(4 * WINDOW, 0.5, Costs{100, 150, 5000});
  BOOST_CHECK(failing.mode() == AdaptiveCacheMode::MemoryOnly);
}

BOOST_AUTO_TEST_CASE(outliers_are_trimmed) {
  // One preempted hit of 1e9 ticks per window must not make caching look expensive.
  Simulation sim;
  for (int w = 0; w < 4; w++) {
    sim.evaluate(WINDOW - 1, 1.0, Costs{100, 150, 90});
    if (const auto window =
            sim.controller.addSample(CostSample{CostSampleKind::MemoryHit, 1'000'000'000, 0})) {
      sim.controller.decide(*window, sim.counters());
    }
  }
  CacheStatistics stats;
  sim.controller.addStatistics(stats, 1.0);
  BOOST_CHECK_CLOSE(stats.memoryHitCostNs, 100, 1e-9);
}

BOOST_AUTO_TEST_CASE(reset_starts_over) {
  Simulation sim;
  sim.evaluate(WINDOW, 0.0, Costs{});
  sim.finishOperatorCall();
  BOOST_REQUIRE(sim.mode() == AdaptiveCacheMode::None);
  sim.controller.reset();
  CacheStatistics stats;
  sim.controller.addStatistics(stats, 1.0);
  BOOST_CHECK(stats.adaptiveMode == AdaptiveCacheMode::MemoryOnly);
  BOOST_CHECK_EQUAL(stats.modeSwitches, 0);
  BOOST_CHECK_EQUAL(stats.noneOperatorCalls, 0);
  BOOST_CHECK_EQUAL(stats.functionCostNs, 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(adaptive_cache_measurement)

BOOST_AUTO_TEST_CASE(hit_rate_tracker_is_marginal) {
  HitRateTracker tracker;
  BOOST_CHECK_LT(tracker.rate(10, 0, 100), 0);  // fewer than a span
  tracker.update(100, 0, 100);                  // anchors (0, 0) and (100, 0)
  BOOST_CHECK_EQUAL(tracker.rate(100, 0, 100), 0.0);
  tracker.update(200, 90, 100);  // anchors (100, 0) and (200, 90)
  BOOST_CHECK_CLOSE(tracker.rate(200, 90, 100), 0.9, 1e-12);
  tracker.update(300, 180, 100);  // anchors (200, 90) and (300, 180)
  BOOST_CHECK_CLOSE(tracker.rate(300, 180, 100), 0.9, 1e-12);
  tracker.reset(300, 180);
  BOOST_CHECK_LT(tracker.rate(350, 230, 100), 0);
}

BOOST_AUTO_TEST_CASE(window_collects_concurrent_samples) {
  CostSampleWindow window;
  constexpr size_t N_THREADS = 8;
  constexpr size_t PER_THREAD = 64 * WINDOW;
  std::atomic<size_t> completed{0};
  std::atomic<size_t> collected{0};
  std::vector<std::thread> threads;
  for (size_t t = 0; t < N_THREADS; t++) {
    threads.emplace_back([&]() {
      for (size_t i = 0; i < PER_THREAD; i++) {
        if (const auto w = window.add(CostSample{CostSampleKind::MemoryHit, 7, 0})) {
          completed++;
          for (const CostSample& s : window.collect(*w)) {
            if (s.totalTicks == 7 && s.kind == CostSampleKind::MemoryHit) collected++;
          }
        }
      }
    });
  }
  for (std::thread& t : threads) t.join();
  BOOST_CHECK_EQUAL(completed.load(), N_THREADS * PER_THREAD / WINDOW);
  // Under this contention (every evaluation a sample) many slots are reused before their window
  // is collected; in the caches, one evaluation in P is a sample.
  BOOST_CHECK_LE(collected.load(), N_THREADS * PER_THREAD);
  BOOST_CHECK_GT(collected.load(), 0);

  // Sequentially every sample of a window is collected.
  CostSampleWindow sequential;
  for (size_t i = 0; i + 1 < WINDOW; i++) {
    BOOST_CHECK(!sequential.add(CostSample{CostSampleKind::MemoryMiss, i, i / 2}));
  }
  const auto w = sequential.add(CostSample{CostSampleKind::DiskHit, 1, 0});
  BOOST_REQUIRE(w);
  BOOST_CHECK_EQUAL(sequential.collect(*w).size(), WINDOW);
  sequential.clear();
  BOOST_CHECK(sequential.collect(*w).empty());
}

BOOST_AUTO_TEST_SUITE_END()
