#ifndef COMBIGRID_TOOLS_CONCURRENCY_HPP
#define COMBIGRID_TOOLS_CONCURRENCY_HPP

#include <cstddef>
#include <limits>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<size_t> partitionRange(size_t length, size_t minPartitionLength,
                                   size_t maxPartitionCnt = std::numeric_limits<size_t>::max());

std::vector<size_t> partitionRangeForConcurrency(size_t length, size_t minLengthForConcurrency,
                                                 size_t minLengthPerThread);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif