// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file constants.hpp
 * @brief Compile-time tuning constants for the @c combigrid module.
 *
 * Each constant controls a heuristic threshold (typically the minimum
 * problem size at which it becomes worthwhile to switch to a parallel
 * implementation). Adjusting these values trades off scheduling overhead
 * against parallel speed-up.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

// Pareto Maximum calculations

namespace sgpp {
namespace combigrid {

/**
 * @brief Numerical constants tuning concurrency thresholds and feature
 * toggles in the combigrid module.
 */
namespace constants {

/**
 * @brief Constants of the source-function cache (@c SourceFuncCache).
 */
namespace source_func_cache {

/// Number of shards of the table of a cache (each with a lock, a record arena and an index;
/// selected by bits 24..29 of the hash, so at most 64). WP-25: one shared table at 16 threads
/// keeps the hits that per-thread tables lose (0.82 vs. 0.47).
constexpr size_t DEFAULT_SHARDS_CNT = 64;

/// Default soft memory limit of a cache in bytes.
constexpr size_t DEFAULT_MEMORY_LIMIT = size_t{1} << 30;  // (1 GiB)

/// Records per chunk of the record arena of a shard, as a power of two (2^7 = 128: every shard
/// allocates whole chunks, so small caches must not pay a large chunk per shard; WP-25: 4096
/// records cost 2.5 KB per entry at 1e4 entries).
constexpr size_t RECORDS_PER_CHUNK_LOG2 = 7;

/// Default limit of the file of the storage policies in bytes.
/// (is also bounded by the free space of the file system when the cache is created).
constexpr size_t DEFAULT_STORAGE_SPACE_LIMIT = size_t{1} << 35;  // (32 GiB)

/// Records a shard of the table collects beyond the memory limit before it writes them to the
/// disk store in one transaction (storage policies; they are found in the buffer meanwhile).
constexpr size_t SPILL_BATCH_RECORDS = 256;

/// Records per write transaction of the disk store (LMDB holds the dirty pages of a transaction
/// in memory; WP-28: 64Ki records of 56 B cost ~25 ms).
constexpr size_t STORE_RECORDS_PER_TXN = size_t{1} << 16;

/// Smallest size of the file of the disk store in pages (LMDB needs a few pages of its own).
constexpr size_t MIN_STORE_PAGES = 16;

// Adaptive policy (CachePolicy::Adaptive, WP-27). c_f = measured cost of the function, E = the
// expected overhead of a cache tier per evaluation it avoids, (h c_hit + (1 - h) o_miss) / h.

/// MemoryOnly / TemporaryStorage -> None if c_f < factor * E_memory.
constexpr double ADAPTIVE_TO_NONE_FACTOR = 0.8;
/// None -> MemoryOnly (a probe stays) only if c_f > factor * E_memory.
constexpr double ADAPTIVE_FROM_NONE_FACTOR = 1.25;
/// MemoryOnly -> TemporaryStorage (memory full) if c_f > factor * E_disk.
constexpr double ADAPTIVE_TO_DISK_FACTOR = 1.5;
/// TemporaryStorage -> MemoryOnly if c_f <= factor * E_disk.
constexpr double ADAPTIVE_FROM_DISK_FACTOR = 1.2;

/// Timed evaluations per decision window (the thread that completes a window decides).
constexpr size_t ADAPTIVE_WINDOW_SAMPLES = 64;
/// Lookups before the hit rate enters the rules (before: the bound for hit rate 1, so that the
/// cold start of a cache never switches caching off; WP-27: the hit rate of the first 4k lookups
/// is far below that of the whole operator call).
constexpr size_t ADAPTIVE_WARMUP_LOOKUPS = size_t{1} << 16;
/// The hit rate is that of the last one to two spans of this many lookups (marginal).
constexpr size_t ADAPTIVE_HIT_RATE_LOOKUPS = size_t{1} << 14;
/// Largest number of operator calls in None between two probes (backoff 1, 2, 4, ...).
constexpr size_t ADAPTIVE_MAX_PROBE_INTERVAL = 64;
/// Timer cost per evaluation the sampling may add on average, in ns: one evaluation in P is
/// timed, P a power of two from the calibrated cost of a timer read (rdtscp 37 ns -> P = 64,
/// steady_clock with HPET 1.9 us -> P = 2048).
constexpr double ADAPTIVE_TIMER_BUDGET_NS = 2.0;

}  // namespace source_func_cache

/**
 * @brief Constants controlling parallel processing of multi-index vectors.
 */
namespace mi_vec {

// Component wise max
/// Minimum number of multi-indices in a vector for parallel component-wise
/// maximum computation to be used.
constexpr size_t CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000;

// Pareto maxima
/// Minimum number of multi-indices for parallel Pareto-maxima computation.
constexpr size_t PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000;
/// Minimum batch size assigned to a single thread during parallel
/// Pareto-maxima computation.
constexpr size_t PM_MIN_MIVEC_BATCH_LENGTH_PER_THREAD = 5000;

// Downwards closedness
/// Minimum number of multi-indices for parallel downwards-closedness
/// computation.
constexpr size_t DWC_MIN_MI_FOR_CONCURRENCY = 10000;

}  // namespace mi_vec

/**
 * @brief Constants controlling sparse-grid generation instructions.
 */
namespace sg_gen_instr {

// Complete sg gen instruction
/// Minimum number of generated multi-indices to trigger parallel generation
/// in the complete sparse-grid generator.
constexpr size_t CSG_MIN_MI_FOR_CONCURRENCY = 1000;
/// Minimum batch size per thread during parallel full sparse-grid
/// generation.
constexpr size_t FSG_MIN_MI_PER_THREAD = 500;

}  // namespace sg_gen_instr

/**
 * @brief Constants controlling combination-technique coefficient
 * computation.
 */
namespace ct_coefficients {

/// Minimum number of multi-indices for parallel combination-coefficient
/// computation.
constexpr size_t MIN_MIS_FOR_CONCURRENCY = 1000;

}  // namespace ct_coefficients

/**
 * @brief Constants of the linear interpolation operator (@c interpolateLinear).
 */
namespace lin_interpolation {

/// The corner loop on a tensor grid of dimension d takes the products of the 1D weights of the
/// first min(d, WEIGHT_TABLE_MAX_DIM) dimensions from a table of 2^min(d, WEIGHT_TABLE_MAX_DIM)
/// entries per thread (8 KiB for 10, built once per tensor grid); the factors of the remaining
/// dimensions are multiplied per corner. Bounds the scratch memory at high d.
constexpr size_t WEIGHT_TABLE_MAX_DIM = 10;

/// The linear interpolation on a deltas sparse grid runs its three passes (mark the corners,
/// evaluate the marked slots, gather the corners per tensor grid) in one parallel region, with a
/// barrier after each of the first two. Below this many corner visits per pass (number of
/// combination tensor grids times 2^d) it runs on the calling thread only. Measured at 16 threads
/// with a cheap integrand (a product of d cosines), parallel vs. serial per call: 132 visits
/// (d = 2, level 16) 13.6 vs. 9.4 us, 872 visits (d = 3, level 8) 22 vs. 38 us.
constexpr size_t DSG_PARALLEL_MIN_CORNER_VISITS = 256;

}  // namespace lin_interpolation

/**
 * @brief Constants controlling sparse-grid node lookup population.
 */
namespace sg_gen_node_lookup {

/// Granularity at which work is pushed onto the task queue when populating
/// the shared node lookup table during sparse-grid generation.
constexpr size_t FOR_ITERATIONS_TO_TASK_QUEUE_PUSH = 100;

}  // namespace sg_gen_node_lookup

/**
 * @brief File format of the disk store of the source-function cache
 * (@c misc::LMDBPointValueStore): the header record (@c misc::lmdb_point_value_store::FileHeader)
 * in the sub-database @c meta. A file with another magic, version or key scheme is rejected.
 */
namespace lmdb_point_value_store {

/// Magic bytes at the start of the header ("SGPPCGSF", not null-terminated).
constexpr char MAGIC[8] = {'S', 'G', 'P', 'P', 'C', 'G', 'S', 'F'};
/// Format version of the file (records and header).
constexpr uint32_t VERSION = 1;
/// Key scheme 1: XXH3-64 of the bit patterns of the point in problem coordinates (the points the
/// function receives, mapped with tools::mapToDomain).
constexpr uint32_t KEY_SCHEME = 1;
/// Key of the header in the sub-database @c meta (with its terminating null byte).
constexpr char HEADER_KEY[] = "header";

}  // namespace lmdb_point_value_store

}  // namespace constants

}  // namespace combigrid
}  // namespace sgpp
