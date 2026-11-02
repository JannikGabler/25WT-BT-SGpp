// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <atomic>
#include <cstdint>
#include <optional>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_window.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

namespace {

uint64_t stampOf(const uint64_t window, const CostSampleKind kind) {
  return ((window + 1) << 8) | static_cast<uint64_t>(kind);
}

}  // namespace

std::optional<uint64_t> CostSampleWindow::add(const CostSample& sample) {
  const uint64_t n = count_.fetch_add(1, std::memory_order_relaxed);
  CostSampleSlot& slot = slots_[n % SIZE];

  // Sequence lock without fences: a reader that sees a field of this write (acquire on the
  // release store) also sees the stamp 0 before it, so its second stamp load detects the write.
  slot.stamp.store(0, std::memory_order_relaxed);
  slot.totalTicks.store(sample.totalTicks, std::memory_order_release);
  slot.functionTicks.store(sample.functionTicks, std::memory_order_release);
  slot.stamp.store(stampOf(n / SIZE, sample.kind), std::memory_order_release);

  if (n % SIZE == SIZE - 1) {
    return n / SIZE;
  }
  return std::nullopt;
}

std::vector<CostSample> CostSampleWindow::collect(const uint64_t window) const {
  std::vector<CostSample> samples;
  samples.reserve(SIZE);

  for (const CostSampleSlot& slot : slots_) {
    const uint64_t stamp = slot.stamp.load(std::memory_order_acquire);
    const CostSample sample{static_cast<CostSampleKind>(stamp & 0xff),
                            slot.totalTicks.load(std::memory_order_acquire),
                            slot.functionTicks.load(std::memory_order_acquire)};
    if ((stamp >> 8) == window + 1 && slot.stamp.load(std::memory_order_acquire) == stamp) {
      samples.push_back(sample);
    }
  }
  return samples;
}

void CostSampleWindow::clear() {
  for (CostSampleSlot& slot : slots_) {
    slot.stamp.store(0, std::memory_order_relaxed);
  }
  // Round up to the next window: the slots of a window are never mixed with earlier samples.
  const uint64_t n = count_.load(std::memory_order_relaxed);
  count_.store((n + SIZE - 1) / SIZE * SIZE, std::memory_order_relaxed);
}

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
