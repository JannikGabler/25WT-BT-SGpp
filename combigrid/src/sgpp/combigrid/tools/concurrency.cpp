#include <cstddef>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

inline std::vector<size_t> partitionRange(const size_t length, const size_t minPartitionLength,
                                          const size_t maxPartitionCnt) {
  if (length < minPartitionLength) {
    return {0, length - 1};
  }

  const size_t nPartitions = std::min(maxPartitionCnt, length / minPartitionLength);
  const size_t chunkSize = length / nPartitions;

  std::vector<size_t> partitioning(nPartitions + 1);
  partitioning[0] = 0;
  partitioning[nPartitions] = length - 1;

  for (size_t i = 1; i < nPartitions; i++) {
    partitioning[i] = i * chunkSize;
  }

  return partitioning;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
