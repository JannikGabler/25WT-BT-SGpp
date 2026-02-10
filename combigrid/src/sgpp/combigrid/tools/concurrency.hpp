#ifndef COMBIGRID_TOOLS_CONCURRENCY_HPP
#define COMBIGRID_TOOLS_CONCURRENCY_HPP

#include <cstddef>
#include <limits>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<size_t> partitionRange(
    const size_t length, const size_t minPartitionLength,
    const size_t maxPartitionCnt = std::numeric_limits<size_t>::max());

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif