// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file spin_lock.hpp
 * @brief Test-and-test-and-set spin lock for very short critical sections.
 */
#pragma once

#include <atomic>
#include <thread>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Test-and-test-and-set spin lock (@c BasicLockable) for critical sections of tens of
 * nanoseconds, such as the shards of the source-function cache (WP-25: lookups at 16 threads 12
 * vs. 24 ns wall with @c std::mutex, inserts 59 vs. 74 ns).
 *
 * Waiting threads spin on a plain load (no cache-line ping-pong) and yield their time slice after
 * a while, so that a descheduled owner (more threads than cores) is not starved.
 */
class SpinLock {
 public:
  /// @brief Acquires the lock.
  void lock() {
    while (flag.exchange(true, std::memory_order_acquire)) {
      for (unsigned spins = 0; flag.load(std::memory_order_relaxed); spins++) {
        if (spins < MAX_SPINS) {
          pause();
        } else {
          std::this_thread::yield();
        }
      }
    }
  }

  /// @brief Releases the lock.
  void unlock() { flag.store(false, std::memory_order_release); }

 private:
  /// Spins with a pause before a waiting thread starts to yield.
  static constexpr unsigned MAX_SPINS = 1024;

  /// @brief Hint to the processor that the thread is spinning.
  static void pause() {
#if defined(__x86_64__) || defined(__i386__)
    __builtin_ia32_pause();
#elif defined(__aarch64__)
    asm volatile("yield");
#endif
  }

  std::atomic<bool> flag{false};  ///< Whether the lock is held.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
