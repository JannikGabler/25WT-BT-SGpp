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

// Pareto Maximum calculations

namespace sgpp {
namespace combigrid {

/**
 * @brief Numerical constants tuning concurrency thresholds and feature
 * toggles in the combigrid module.
 */
namespace constants {

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
 * @brief Constants controlling source-function evaluation behavior.
 */
namespace source_func {

/// If @c true, source-function evaluations are routed through the cache;
/// disable to avoid hashing overhead when caching is not beneficial.
constexpr bool USE_CACHE = false;

}

/**
 * @brief Constants controlling sparse-grid generation instructions.
 */
namespace sg_gen_instr {

// Full sg gen instruction
/// Minimum number of generated multi-indices to trigger parallel generation
/// in the full sparse-grid generator.
constexpr size_t FSG_MIN_MI_FOR_CONCURRENCY = 1000;
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

}

/**
 * @brief Constants controlling sparse-grid node lookup population.
 */
namespace sg_gen_node_lookup {

/// Granularity at which work is pushed onto the task queue when populating
/// the shared node lookup table during sparse-grid generation.
constexpr size_t FOR_ITERATIONS_TO_TASK_QUEUE_PUSH = 100;

}

}  // namespace constants

}  // namespace combigrid
}  // namespace sgpp
