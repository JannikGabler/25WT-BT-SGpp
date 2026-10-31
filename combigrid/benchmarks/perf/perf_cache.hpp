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
 * - @c micro/cache/memory/production/...: heap and cache bytes with and without a memory limit.
 * Only registered with the libraries of the cache (@c SGPP_COMBIGRID_HAS_CACHE_LIBRARIES, see
 * optional_libraries.hpp).
 */
#pragma once

#include <sgpp/combigrid/optional_libraries.hpp>

#ifdef SGPP_COMBIGRID_HAS_CACHE_LIBRARIES

#include <cstddef>
#include <cstdint>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/point_value_table.hpp>
#include <string>

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
      : table(misc::RecordLayout{layout.d, layout.m}, memLimit) {}

  bool find(const base::DataVector& x, uint64_t /*hash*/, double* out) {
    return table.find(x.data(), misc::PointValueTable::hash(x.data(), x.size()), out);
  }

  bool insert(const base::DataVector& x, uint64_t /*hash*/, const double* value) {
    table.insert(x.data(), misc::PointValueTable::hash(x.data(), x.size()), value);
    return true;
  }

  size_t size() const { return table.statistics().entries; }

 private:
  misc::PointValueTable table;
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
        const CachedSourceFunc f(expSum, CacheConfig{.memoryLimit = limit});
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
}

}  // namespace cache_production
}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_HAS_CACHE_LIBRARIES
