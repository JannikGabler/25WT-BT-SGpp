/**
 * @file concurrency.hpp
 * @brief Helpers for partitioning index ranges into roughly equal chunks
 * for parallel processing.
 */
#ifndef COMBIGRID_TOOLS_CONCURRENCY_HPP
#define COMBIGRID_TOOLS_CONCURRENCY_HPP

#include <cstddef>
#include <limits>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Splits the range @c [0, length) into contiguous partitions.
 *
 * Partitions are sized so that each one is at least @p minPartitionLength
 * elements long; the number of partitions is also capped at
 * @p maxPartitionCnt. If @p length itself is smaller than
 * @p minPartitionLength, the function returns @c {0, length} (a single
 * partition covering the whole range).
 *
 * @param length             Total range length.
 * @param minPartitionLength Minimum acceptable size of a partition.
 * @param maxPartitionCnt    Hard cap on the number of partitions.
 * @return Vector of partition boundaries; partition @c k covers
 *         @c [out[k], out[k+1]).
 */
std::vector<size_t> partitionRange(size_t length, size_t minPartitionLength,
                                   size_t maxPartitionCnt = std::numeric_limits<size_t>::max());

/**
 * @brief Convenience wrapper around @ref partitionRange that uses the
 * OpenMP thread count as the partition cap.
 *
 * Falls back to a single partition if the range is shorter than
 * @p minLengthForConcurrency, avoiding the overhead of spawning threads
 * for tiny problems.
 *
 * @param length                  Total range length.
 * @param minLengthForConcurrency Threshold below which serial execution is preferred.
 * @param minLengthPerThread      Minimum range length assigned to one thread.
 * @return Vector of partition boundaries.
 */
std::vector<size_t> partitionRangeForConcurrency(size_t length, size_t minLengthForConcurrency,
                                                 size_t minLengthPerThread);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
