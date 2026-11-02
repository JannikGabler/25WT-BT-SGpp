// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-27: ThreadSanitizer driver of the adaptive caching policy (std::thread instead of OpenMP,
// whose runtime is not instrumented). Built together with the cache sources, see tsan.sh. Every
// evaluation is timed (sampling shift 0: windows of 64 evaluations, decisions all the time).
// 16 threads evaluate overlapping points of an adaptive CachedSourceFunc and
// CachedVectorSourceFunc while one thread reads the statistics and calls the end-of-operator hook
// (probes from None concurrent with evaluations and decisions):
// - a cheap function (switched to None and probed again and again),
// - an expensive one (stays MemoryOnly), with memory limit 0 (TemporaryStorage: the temporary
//   file is created concurrently by the first misses, then spilled to and read).
// Afterwards the values and the counters are checked.

#include <stdlib.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_timer.hpp>
#include <span>
#include <string>
#include <thread>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
using sgpp::combigrid::CachePolicy;
using sgpp::combigrid::CacheStatistics;
using sgpp::combigrid::SourceFuncCacheConfig;

namespace {

double cheap(const DataVector& x) { return std::exp(-x[0] * x[0]) + x[1] * x[2]; }

double expensive(const DataVector& x) {
  const auto end = std::chrono::steady_clock::now() + std::chrono::microseconds(20);
  while (std::chrono::steady_clock::now() < end) {
  }
  return cheap(x);
}

DataVector pointOf(const size_t idx, const size_t nDistinct) {
  return DataVector{static_cast<double>(idx) / static_cast<double>(nDistinct), 0.5, -0.25};
}

bool run(double (*f)(const DataVector&), const size_t limit, const size_t nPerThread,
         const char* name) {
  constexpr size_t N_THREADS = 16;
  constexpr size_t N_DISTINCT = 2000;
  const SourceFuncCacheConfig config{.memoryLimit = limit, .policy = CachePolicy::Adaptive};
  const CachedSourceFunc scalar(f, config);
  const CachedVectorSourceFunc vector(
      [f](const DataVector& x, const std::span<double> out) {
        out[0] = f(x);
        out[1] = -f(x);
      },
      2, config);
  std::atomic<size_t> wrong{0};
  std::atomic<bool> done{false};

  std::thread hooks([&]() {
    while (!done.load()) {
      const CacheStatistics stats = scalar.statistics();
      if (stats.hits > stats.lookups) wrong++;
      scalar.finishOperatorCall();
      vector.finishOperatorCall();
    }
  });
  std::vector<std::thread> workers;
  for (size_t t = 0; t < N_THREADS; t++) {
    workers.emplace_back([&, t]() {
      std::vector<double> out(2);
      for (size_t i = 0; i < nPerThread; i++) {
        const DataVector point = pointOf((i * 7919 + t * 104729) % N_DISTINCT, N_DISTINCT);
        if (scalar.evaluate(point) != f(point)) wrong++;
        vector.evaluate(point, out);
        if (out[0] != f(point) || out[1] != -f(point)) wrong++;
      }
    });
  }
  for (std::thread& w : workers) w.join();
  done.store(true);
  hooks.join();

  const CacheStatistics stats = scalar.statistics();
  std::printf(
      "%s limit %zu: mode %d switches %zu none-calls %zu lookups %zu hits %zu (disk %zu) entries "
      "%zu disk %zu function %.0f ns hit %.0f ns wrong %zu\n",
      name, limit, static_cast<int>(stats.adaptiveMode), stats.modeSwitches,
      stats.noneOperatorCalls, stats.lookups, stats.hits, stats.diskHits, stats.entries,
      stats.diskEntries, stats.functionCostNs, stats.memoryHitCostNs, wrong.load());
  return wrong.load() == 0;
}

}  // namespace

int main() {
  std::string dir = (std::filesystem::temp_directory_path() / "sgpp-tsan-XXXXXX").string();
  if (mkdtemp(dir.data()) == nullptr) return 2;
  setenv("TMPDIR", dir.c_str(), 1);
  sgpp::combigrid::misc::adaptive_caching::CostTimer::overrideSamplingShift(0);

  int failed = 0;
  failed += run(cheap, size_t{1} << 30, 20000, "cheap") ? 0 : 1;
  failed += run(expensive, size_t{1} << 30, 300, "expensive") ? 0 : 1;
  failed += run(expensive, 0, 300, "expensive") ? 0 : 1;
  std::filesystem::remove_all(dir);
  std::printf("%s\n", failed == 0 ? "OK" : "FAILED");
  return failed;
}
