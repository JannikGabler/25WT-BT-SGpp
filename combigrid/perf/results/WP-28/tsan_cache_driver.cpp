// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-28: ThreadSanitizer driver of the source-function cache incl. the storage policies
// (std::thread instead of OpenMP, whose runtime is not instrumented; LMDB is not instrumented
// either, its pthread locks are). Built together with the cache sources, see tsan.sh. Derived
// from the WP-26 driver: 16 threads evaluate overlapping points of a CachedSourceFunc and a
// CachedVectorSourceFunc (memory only, temporary and persistent files, unlimited / limited / no
// memory: spills during the evaluations) while one thread reads the statistics and calls the
// end-of-operator hook (flushes concurrent with inserts); afterwards the values and counters
// are checked, and the persistent file is reloaded.

#include <stdlib.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <span>
#include <string>
#include <thread>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
using sgpp::combigrid::CachePolicy;
using sgpp::combigrid::CacheStatistics;
using sgpp::combigrid::SourceFuncCache;
using sgpp::combigrid::SourceFuncCacheConfig;

namespace {

double f(const DataVector& x) { return std::exp(-x[0] * x[0]) + x[1] * x[2]; }

DataVector pointOf(const size_t idx, const size_t nDistinct) {
  return DataVector{static_cast<double>(idx) / static_cast<double>(nDistinct), 0.5, -0.25};
}

SourceFuncCacheConfig configOf(const size_t limit, const CachePolicy policy, const std::string& dir,
                               const std::string& name) {
  const bool persistent = policy == CachePolicy::PersistentStorage;
  return SourceFuncCacheConfig{
      .memoryLimit = limit,
      .policy = policy,
      .file = persistent ? dir + "/" + name + std::to_string(limit) : ""};
}

bool evaluateConcurrently(const size_t limit, const CachePolicy policy, const std::string& dir) {
  constexpr size_t N_THREADS = 16;
  constexpr size_t N_DISTINCT = 20000;
  constexpr size_t N_PER_THREAD = 40000;

  const bool persistent = policy == CachePolicy::PersistentStorage;
  const auto config = [&](const std::string& name) { return configOf(limit, policy, dir, name); };
  auto cache = std::make_shared<SourceFuncCache>(config("scalar"));
  const CachedSourceFunc scalar(f, cache);
  const CachedVectorSourceFunc vector(
      [](const DataVector& x, const std::span<double> out) {
        out[0] = f(x);
        out[1] = -f(x);
      },
      2, config("vector"));
  std::atomic<size_t> wrong{0};
  std::atomic<bool> done{false};

  std::thread reader([&]() {
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
      for (size_t i = 0; i < N_PER_THREAD; i++) {
        const DataVector point = pointOf((i * 7919 + t * 104729) % N_DISTINCT, N_DISTINCT);
        if (scalar.evaluate(point) != f(point)) wrong++;
        vector.evaluate(point, out);
        if (out[0] != f(point) || out[1] != -f(point)) wrong++;
      }
    });
  }
  for (std::thread& w : workers) w.join();
  done.store(true);
  reader.join();

  scalar.finishOperatorCall();
  const CacheStatistics stats = scalar.statistics();
  std::printf(
      "policy %d limit %zu: lookups %zu hits %zu (disk %zu) entries %zu disk %zu rejected %zu "
      "wrong %zu\n",
      static_cast<int>(policy), limit, stats.lookups, stats.hits, stats.diskHits, stats.entries,
      stats.diskEntries, stats.rejectedInserts, wrong.load());
  const bool stored = (policy == CachePolicy::MemoryOnly)
                          ? (limit < (size_t{1} << 30) || stats.entries == N_DISTINCT)
                          : (persistent ? stats.diskEntries == N_DISTINCT
                                        : stats.entries + stats.diskEntries == N_DISTINCT);
  return wrong.load() == 0 && stats.lookups == N_THREADS * N_PER_THREAD && stored &&
         (policy == CachePolicy::MemoryOnly || stats.rejectedInserts == 0);
}

int run(const size_t limit, const CachePolicy policy, const std::string& dir) {
  bool ok = evaluateConcurrently(limit, policy, dir);
  if (policy == CachePolicy::PersistentStorage) {
    // All functions of the cache are destroyed: the file is unlocked and holds every point.
    const SourceFuncCache reloaded(configOf(limit, policy, dir, "scalar"));
    std::printf("  reloaded %zu entries\n", reloaded.statistics().loadedEntries);
    ok = ok && reloaded.statistics().loadedEntries == 20000;
  }
  return ok ? 0 : 1;
}

}  // namespace

int main() {
  std::string dir = (std::filesystem::temp_directory_path() / "sgpp-tsan-XXXXXX").string();
  if (mkdtemp(dir.data()) == nullptr) return 2;
  int failed = run(size_t{1} << 30, CachePolicy::MemoryOnly, dir) +
               run(size_t{1} << 20, CachePolicy::MemoryOnly, dir);
  for (const CachePolicy policy : {CachePolicy::TemporaryStorage, CachePolicy::PersistentStorage}) {
    for (const size_t limit : {size_t{1} << 30, size_t{1} << 20, size_t{0}}) {
      failed += run(limit, policy, dir);
    }
  }
  std::filesystem::remove_all(dir);
  std::printf("%s\n", failed == 0 ? "OK" : "FAILED");
  return failed;
}
