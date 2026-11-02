// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_timer.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

namespace {

constexpr int MIN_SAMPLING_SHIFT = 4;
constexpr int MAX_SAMPLING_SHIFT = 16;

std::atomic<int> samplingShiftOverride{-1};

/// Median over 9 batches of the interval of 32 back-to-back reads (on one CPU) per read.
double measureReadCost() {
  std::array<double, 9> perRead{};
  for (double& cost : perRead) {
    uint32_t firstCpu = 0;
    uint32_t lastCpu = 0;
    do {
      const uint64_t first = CostTimer::read(firstCpu);
      uint64_t last = first;
      for (int i = 0; i < 32; i++) last = CostTimer::read(lastCpu);
      cost = static_cast<double>(last - first) / 32.0;
    } while (firstCpu != lastCpu);
  }
  std::nth_element(perRead.begin(), perRead.begin() + 4, perRead.end());
  return perRead[4];
}

/// Ticks per ns over 200 us of steady_clock (exactly 1 without rdtscp).
double measureTicksPerNs() {
#ifdef SGPP_COMBIGRID_COST_TIMER_RDTSCP
  uint32_t cpu = 0;
  const auto start = std::chrono::steady_clock::now();
  const uint64_t startTicks = CostTimer::read(cpu);
  std::chrono::steady_clock::time_point now;
  do {
    now = std::chrono::steady_clock::now();
  } while (now - start < std::chrono::microseconds(200));
  const uint64_t ticks = CostTimer::read(cpu) - startTicks;
  return static_cast<double>(ticks) / std::chrono::duration<double, std::nano>(now - start).count();
#else
  return 1.0;
#endif
}

}  // namespace

double CostTimer::readCostTicks() {
  static const double cost = measureReadCost();
  return cost;
}

double CostTimer::ticksPerNs() {
  static const double rate = measureTicksPerNs();
  return rate;
}

unsigned CostTimer::samplingShift() {
  const int override = samplingShiftOverride.load(std::memory_order_relaxed);
  if (override >= 0) {
    return static_cast<unsigned>(override);
  }
  static const unsigned shift = []() {
    const double readNs = readCostTicks() / ticksPerNs();
    const double period = 2.0 * readNs / constants::source_func_cache::ADAPTIVE_TIMER_BUDGET_NS;
    const int log2Period = static_cast<int>(std::ceil(std::log2(std::max(period, 1.0))));
    return static_cast<unsigned>(std::clamp(log2Period, MIN_SAMPLING_SHIFT, MAX_SAMPLING_SHIFT));
  }();
  return shift;
}

void CostTimer::overrideSamplingShift(const int shift) {
  samplingShiftOverride.store(shift, std::memory_order_relaxed);
}

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
