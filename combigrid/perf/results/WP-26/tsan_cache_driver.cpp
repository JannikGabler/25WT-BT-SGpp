// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-26: ThreadSanitizer driver of the source-function cache (std::thread instead of OpenMP,
// whose runtime is not instrumented). Built together with the cache sources, see tsan.sh.
// 16 threads evaluate overlapping points of a CachedSourceFunc and a CachedVectorSourceFunc
// (both on small and on limited caches) while one thread reads the statistics and calls the
// end-of-operator hook; afterwards the values and counters are checked.

#include <atomic>
#include <cmath>
#include <cstdio>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <span>
#include <thread>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CachedSourceFunc;
using sgpp::combigrid::CachedVectorSourceFunc;
using sgpp::combigrid::CacheStatistics;
using sgpp::combigrid::SourceFuncCache;
using sgpp::combigrid::SourceFuncCacheConfig;

namespace {

double f(const DataVector& x) { return std::exp(-x[0] * x[0]) + x[1] * x[2]; }

DataVector pointOf(const size_t idx, const size_t nDistinct) {
  return DataVector{static_cast<double>(idx) / static_cast<double>(nDistinct), 0.5, -0.25};
}

int run(const size_t limit) {
  constexpr size_t N_THREADS = 16;
  constexpr size_t N_DISTINCT = 20000;
  constexpr size_t N_PER_THREAD = 40000;

  auto cache = std::make_shared<SourceFuncCache>(SourceFuncCacheConfig{.memoryLimit = limit});
  const CachedSourceFunc scalar(f, cache);
  const CachedVectorSourceFunc vector(
      [](const DataVector& x, const std::span<double> out) {
        out[0] = f(x);
        out[1] = -f(x);
      },
      2, SourceFuncCacheConfig{.memoryLimit = limit});
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

  const CacheStatistics stats = scalar.statistics();
  std::printf("limit %zu: lookups %zu hits %zu entries %zu rejected %zu wrong %zu\n", limit,
              stats.lookups, stats.hits, stats.entries, stats.rejectedInserts, wrong.load());
  const bool ok = wrong.load() == 0 && stats.lookups == N_THREADS * N_PER_THREAD &&
                  (limit < (size_t{1} << 30) || stats.entries == N_DISTINCT);
  return ok ? 0 : 1;
}

}  // namespace

int main() {
  const int failed = run(size_t{1} << 30) + run(size_t{1} << 20);
  std::printf("%s\n", failed == 0 ? "OK" : "FAILED");
  return failed;
}
