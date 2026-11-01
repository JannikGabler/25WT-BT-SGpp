// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_cache.hpp
 * @brief WP-26 micro-benchmarks of the production source-function cache (@c CachedSourceFunc,
 * @c misc::PointValueTable), next to the WP-25 prototypes of perf_cache_proto.hpp (same harness,
 * same configurations, same binary):
 * - @c micro/cache/lookup|insert/production/d<d>/n<n>: the production table (XXH3 included)
 *   in the WP-25 table cases (compare with @c sharded_spin, which hashes with an own wyhash);
 * - @c micro/cache/op/<cfg>/{always,warm}_production/c<ns>: whole operators with a
 *   @c CachedSourceFunc (always: a new cache per call; warm: filled before, for interp /
 *   interplin by a quadrature on the same grid = cross-operator reuse) vs. @c none (plain
 *   @c SourceFunc, registered by perf_cache_proto.hpp);
 * - @c micro/cache/memory/production/...: heap and cache bytes with and without a memory limit;
 * - WP-28, storage policies (only with LMDB, files in @c $SGPP_PERF_CACHE_DIR, else /var/tmp):
 *   @c micro/cache/disk/{write,lookup,load}/<variant>/d<d>/n<n> (throughput of the disk store:
 *   new points + end-of-operator hook, warm hits from memory / disk, loading a persistent file)
 *   and @c micro/cache/op/<cfg>/<variant>_production/c<ns> with the variants
 *   @c persistent_cold (new file per call, incl. the final sync), @c persistent_warm (cache
 *   created on the filled file: load + operator, all hits), @c temporary_spill (memory limit 0,
 *   new cache per call) and @c temporary_spill_warm (all hits from disk).
 * Only registered with the libraries of the cache (@c SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE,
 * see tools/defines/dependencies_defines.hpp).
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_policy.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table.hpp>
#include <string>
#include <vector>

#include "perf_cache_proto.hpp"
#include "perf_harness.hpp"
#include "perf_problems.hpp"

namespace sgpp {
namespace combigrid {
namespace perf {
namespace cache_production {

using cache_proto::OpConfig;
using cache_proto::OpInstance;

/// @brief The production table behind the interface of the WP-25 table benchmarks (hashes the
/// points itself with XXH3, as the cached functions do).
class ProductionTable {
 public:
  static constexpr bool OWN_HASH = true;

  ProductionTable(const cache_proto::RecordLayout layout, size_t /*capacityHint*/,
                  const size_t memLimit)
      : table(misc::boost_point_value_table::RecordLayout{layout.d, layout.m}, memLimit,
              constants::source_func_cache::DEFAULT_SHARDS_CNT) {}

  bool find(const base::DataVector& x, uint64_t /*hash*/, double* out) {
    return table.find(x.data(), misc::BoostPointValueTable::hash(x.data(), x.size()), out);
  }

  bool insert(const base::DataVector& x, uint64_t /*hash*/, const double* value) {
    table.insert(x.data(), misc::BoostPointValueTable::hash(x.data(), x.size()), value);
    return true;
  }

  size_t size() const { return table.statistics().entries; }

 private:
  misc::BoostPointValueTable table;
};

/// @brief Hits / lookups between two statistics.
inline double hitRate(const CacheStatistics& before, const CacheStatistics& after) {
  const size_t lookups = after.lookups - before.lookups;
  return lookups == 0
             ? 0.0
             : static_cast<double>(after.hits - before.hits) / static_cast<double>(lookups);
}

/// @brief An operator with a cached cost-controlled integrand (variant always / warm).
inline void addOpCase(Suite& suite, const OpConfig& cfg, const std::string& variant,
                      const double costNs) {
  const std::string name = "micro/cache/op/" + cfg.name() + "/" + variant + "_production/c" +
                           std::to_string(static_cast<int64_t>(costNs));
  suite.addMicro(name, [=](MicroRun& run) {
    const OpInstance inst(cfg);
    const cache_proto::CostedFunc costed(cfg.integrand, costNs);

    const CachedSourceFunc counting(costed);
    const double cachedResult = inst.run(counting);
    const double plainResult = inst.run(SourceFunc(costed));
    const CacheStatistics cold = counting.statistics();
    run.setCounter("evals_per_call", static_cast<double>(cold.lookups));
    run.setCounter("distinct", static_cast<double>(cold.entries));
    run.setCounter("cold_hit_rate", hitRate(CacheStatistics(), cold));
    run.setCounter("bitwise_equal_or_1thread_only",
                   (omp_get_max_threads() > 1) ? -1.0 : (cachedResult == plainResult ? 1.0 : 0.0));
    run.setCounter("threads", omp_get_max_threads());
    run.setItems(static_cast<double>(cold.lookups));

    std::optional<CachedSourceFunc> warm;
    if (variant == "warm") {
      warm.emplace(costed);
      if (cfg.op == "interp" || cfg.op == "interplin") {
        inst.runQuadrature(*warm);
      } else {
        inst.run(*warm);
      }
    }
    const CacheStatistics before = warm ? warm->statistics() : CacheStatistics();

    run.measure([&]() {
      if (warm) {
        doNotOptimize(inst.run(*warm));
      } else {
        const CachedSourceFunc fresh(costed);
        doNotOptimize(inst.run(fresh));
      }
    });
    if (warm) {
      run.setCounter("hit_rate", hitRate(before, warm->statistics()));
    }
  });
}

/// @brief Memory of a cache filled by one quadrature, with the given limit; measured: a further
/// quadrature with that cache (hits on the stored points, misses on the rejected ones).
inline void addMemoryCase(Suite& suite, const std::string& limitName, const size_t limit) {
  const OpConfig cfg{"quad", "equi", expSum, 0, 5, 8, 0};
  suite.addMicro(
      "micro/cache/memory/production/" + cfg.name() + "/" + limitName, [=](MicroRun& run) {
        const OpInstance inst(cfg);
        const size_t heapBefore = cache_proto::heapBytesInUse();
        const CachedSourceFunc f(expSum, SourceFuncCacheConfig{.memoryLimit = limit});
        inst.run(f);
        const CacheStatistics filled = f.statistics();
        run.setCounter("heap_mib", static_cast<double>(cache_proto::heapBytesInUse() - heapBefore) /
                                       (1024.0 * 1024.0));
        run.setCounter("cache_mib", static_cast<double>(filled.bytes) / (1024.0 * 1024.0));
        run.setCounter("entries", static_cast<double>(filled.entries));
        run.setCounter("rejected", static_cast<double>(filled.rejectedInserts));
        run.setCounter("threads", omp_get_max_threads());
        run.setItems(static_cast<double>(filled.lookups));
        run.measure([&]() { doNotOptimize(inst.run(f)); });
        run.setCounter("hit_rate", hitRate(filled, f.statistics()));
      });
}

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

/// @brief Directory of the benchmark files ($SGPP_PERF_CACHE_DIR, else /var/tmp: /tmp may be a
/// RAM file system).
inline std::string diskBenchDir() {
  const char* const dir = std::getenv("SGPP_PERF_CACHE_DIR");
  return (dir != nullptr && dir[0] != '\0') ? dir : "/var/tmp";
}

/// @brief A benchmark file (removed with its lock file at construction and destruction).
class BenchFile {
 public:
  explicit BenchFile(const std::string& tag)
      : path_(diskBenchDir() + "/sgpp-perf-cache-" + std::to_string(getpid()) + "-" + tag) {
    remove();
  }
  ~BenchFile() { remove(); }

  const std::string& path() const { return path_; }

  void remove() const {
    std::filesystem::remove(path_);
    std::filesystem::remove(path_ + "-lock");
  }

 private:
  std::string path_;
};

inline SourceFuncCacheConfig diskConfig(const std::string& variant, const std::string& file) {
  if (variant == "persistent") {
    return SourceFuncCacheConfig{.policy = CachePolicy::PersistentStorage, .file = file};
  }
  if (variant == "temporary0") {
    return SourceFuncCacheConfig{.memoryLimit = 0, .policy = CachePolicy::TemporaryStorage};
  }
  return SourceFuncCacheConfig();
}

/// @brief n distinct points of dimension d (deterministic).
inline std::vector<base::DataVector> diskBenchPoints(const size_t nDim, const size_t n) {
  std::vector<base::DataVector> points(n, base::DataVector(nDim));
  for (size_t i = 0; i < n; i++) {
    for (size_t k = 0; k < nDim; k++) {
      points[i][k] = static_cast<double>((i * (k + 3) + k * 7919) % 104729) / 104729.0 +
                     static_cast<double>(i) * 1e-9;
    }
  }
  return points;
}

/// @brief Evaluates all points (OpenMP threads of the run) and calls the end-of-operator hook.
inline double evaluateAll(const CachedSourceFunc& f, const std::vector<base::DataVector>& points) {
  double sum = 0;
#pragma omp parallel for reduction(+ : sum) schedule(static)
  for (size_t i = 0; i < points.size(); i++) {
    sum += f.evaluate(points[i]);
  }
  f.finishOperatorCall();
  return sum;
}

/// @brief Write throughput: a new cache per call (variant memory / persistent / temporary0)
/// receives n new points; the hook writes them (persistent) - the spill writes happen during the
/// evaluations (temporary0).
inline void addDiskWriteCase(Suite& suite, const std::string& variant, const size_t nDim,
                             const size_t n) {
  suite.addMicro("micro/cache/disk/write/" + variant + "/d" + std::to_string(nDim) + "/n" +
                     std::to_string(n),
                 [=](MicroRun& run) {
                   const std::vector<base::DataVector> points = diskBenchPoints(nDim, n);
                   const BenchFile file("write");
                   run.setItems(static_cast<double>(n));
                   run.setCounter("threads", omp_get_max_threads());
                   run.measure([&]() {
                     file.remove();
                     const CachedSourceFunc f(expSum, diskConfig(variant, file.path()));
                     doNotOptimize(evaluateAll(f, points));
                     run.setCounter("file_mib", static_cast<double>(f.statistics().fileBytes) /
                                                    (1024.0 * 1024.0));
                   });
                 });
}

/// @brief Append throughput of a long-lived persistent cache whose file holds @p nExisting
/// points: every call evaluates n new points and the hook writes them (random keys into the
/// existing file, as a refinement step).
inline void addDiskAppendCase(Suite& suite, const size_t nDim, const size_t n,
                              const size_t nExisting) {
  suite.addMicro("micro/cache/disk/append/persistent/d" + std::to_string(nDim) + "/n" +
                     std::to_string(n) + "/e" + std::to_string(nExisting),
                 [=](MicroRun& run) {
                   const BenchFile file("append");
                   const CachedSourceFunc f(expSum, diskConfig("persistent", file.path()));
                   std::vector<base::DataVector> points = diskBenchPoints(nDim, nExisting);
                   evaluateAll(f, points);
                   points.resize(n);
                   double shift = 1.0;
                   run.setItems(static_cast<double>(n));
                   run.setCounter("threads", omp_get_max_threads());
                   run.measure([&]() {
                     for (base::DataVector& point : points) point[0] += shift;
                     shift = 1.0 + shift / 1024.0;
                     doNotOptimize(evaluateAll(f, points));
                   });
                   run.setCounter("file_mib", static_cast<double>(f.statistics().fileBytes) /
                                                  (1024.0 * 1024.0));
                 });
}

/// @brief Hit throughput: all points stored in memory (variant memory) or only on disk
/// (temporary0: memory limit 0).
inline void addDiskLookupCase(Suite& suite, const std::string& variant, const size_t nDim,
                              const size_t n) {
  suite.addMicro("micro/cache/disk/lookup/" + variant + "/d" + std::to_string(nDim) + "/n" +
                     std::to_string(n),
                 [=](MicroRun& run) {
                   const std::vector<base::DataVector> points = diskBenchPoints(nDim, n);
                   const CachedSourceFunc f(expSum, diskConfig(variant, ""));
                   evaluateAll(f, points);
                   const CacheStatistics before = f.statistics();
                   run.setItems(static_cast<double>(n));
                   run.setCounter("threads", omp_get_max_threads());
                   run.measure([&]() { doNotOptimize(evaluateAll(f, points)); });
                   run.setCounter("hit_rate", hitRate(before, f.statistics()));
                 });
}

/// @brief Load throughput: creating a cache on a persistent file with n records.
inline void addDiskLoadCase(Suite& suite, const size_t nDim, const size_t n) {
  suite.addMicro("micro/cache/disk/load/persistent/d" + std::to_string(nDim) + "/n" +
                     std::to_string(n),
                 [=](MicroRun& run) {
                   const BenchFile file("load");
                   {
                     const CachedSourceFunc f(expSum, diskConfig("persistent", file.path()));
                     evaluateAll(f, diskBenchPoints(nDim, n));
                   }
                   run.setItems(static_cast<double>(n));
                   run.setCounter("threads", omp_get_max_threads());
                   run.measure([&]() {
                     const SourceFuncCache cache(diskConfig("persistent", file.path()));
                     doNotOptimize(cache.statistics().loadedEntries);
                   });
                 });
}

/// @brief An operator with a cached cost-controlled integrand and a storage policy (see the file
/// documentation for the variants).
inline void addDiskOpCase(Suite& suite, const OpConfig& cfg, const std::string& variant,
                          const double costNs) {
  const std::string name = "micro/cache/op/" + cfg.name() + "/" + variant + "_production/c" +
                           std::to_string(static_cast<int64_t>(costNs));
  suite.addMicro(name, [=](MicroRun& run) {
    const OpInstance inst(cfg);
    const cache_proto::CostedFunc costed(cfg.integrand, costNs);
    const BenchFile file("op");
    const bool persistentVariant = variant.starts_with("persistent");
    const SourceFuncCacheConfig config =
        diskConfig(persistentVariant ? "persistent" : "temporary0", file.path());
    run.setCounter("threads", omp_get_max_threads());

    std::optional<CachedSourceFunc> warm;
    if (variant == "persistent_warm") {
      const CachedSourceFunc fill(costed, config);
      inst.run(fill);
    } else if (variant == "temporary_spill_warm") {
      warm.emplace(costed, config);
      inst.run(*warm);
    }
    {
      const CachedSourceFunc counting(costed);
      inst.run(counting);
      run.setItems(static_cast<double>(counting.statistics().lookups));
    }

    run.measure([&]() {
      if (warm) {
        doNotOptimize(inst.run(*warm));
        return;
      }
      if (variant == "persistent_cold") file.remove();
      const CachedSourceFunc f(costed, config);
      doNotOptimize(inst.run(f));
    });
    if (warm) {
      run.setCounter("disk_hits", static_cast<double>(warm->statistics().diskHits));
    }
  });
}

inline void addDiskCases(Suite& suite) {
  for (const size_t nDim : {5, 20}) {
    for (const std::string variant : {"memory", "persistent", "temporary0"}) {
      addDiskWriteCase(suite, variant, nDim, 100'000);
    }
    for (const std::string variant : {"memory", "temporary0"}) {
      addDiskLookupCase(suite, variant, nDim, 100'000);
    }
    addDiskLoadCase(suite, nDim, 100'000);
    addDiskAppendCase(suite, nDim, 100'000, 1'000'000);
  }

  const std::vector<OpConfig> configs = cache_proto::opConfigs();
  for (const size_t idx : {0, 1, 5}) {  // quad cc d5 L4, quad equi d20 L3, interp cc d5 L3
    for (const double cost : {0.0, 1000.0}) {
      for (const std::string variant :
           {"persistent_cold", "persistent_warm", "temporary_spill", "temporary_spill_warm"}) {
        addDiskOpCase(suite, configs[idx], variant, cost);
      }
    }
  }
}

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

inline void addCacheProductionCases(Suite& suite) {
  for (const size_t nDim : {2, 10, 20}) {
    for (const size_t n : {10'000, 1'000'000}) {
      cache_proto::addTableCases<ProductionTable>(suite, "production", nDim, n);
    }
  }

  for (const OpConfig& cfg : cache_proto::opConfigs()) {
    for (const double cost : {0.0, 1000.0}) {
      addOpCase(suite, cfg, "always", cost);
      addOpCase(suite, cfg, "warm", cost);
    }
  }

  addMemoryCase(suite, "limit1GiB", size_t{1} << 30);
  addMemoryCase(suite, "limit16MiB", size_t{16} << 20);

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
  addDiskCases(suite);
#endif
}

}  // namespace cache_production
}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
