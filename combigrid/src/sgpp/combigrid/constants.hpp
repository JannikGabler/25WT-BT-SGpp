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
 * @brief Constants controlling sparse-grid node lookup population.
 */
namespace sg_gen_node_lookup {

/// Granularity at which work is pushed onto the task queue when populating
/// the shared node lookup table during sparse-grid generation.
constexpr size_t FOR_ITERATIONS_TO_TASK_QUEUE_PUSH = 100;

}  // namespace sg_gen_node_lookup

/**
 * TODO: Document
 */
namespace lmdb_point_value_store {

constexpr char MAGIC[8] = {'S', 'G', 'P', 'P', 'C', 'G', 'S', 'F'};
/// Format version of the file (records and header).
constexpr uint32_t VERSION = 1;
/// Key scheme 1: XXH3-64 of the bit patterns of the point in problem coordinates (the points the
/// function receives, mapped with tools::mapToDomain).
constexpr uint32_t KEY_SCHEME = 1;
constexpr char HEADER_KEY[] = "header";

}  // namespace lmdb_point_value_store

}  // namespace constants

}  // namespace combigrid
}  // namespace sgpp
