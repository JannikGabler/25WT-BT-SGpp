#pragma once

#include <omp.h>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <vector>

template <typename T>
class MIVec;  // Forward Declaration

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
MI<T> mergeComponentWiseMax(const size_t nDim, const std::vector<MI<T>>& localMax) {
  MI<T> globalMax(nDim);

  for (const MI<T>& localMI : localMax) {
    for (size_t dim = 0; dim < nDim; dim++) {
      globalMax[dim] = std::max(globalMax[dim], localMI[dim]);
    }
  }

  return globalMax;
}

/*
TODO: Optimize (used pareto Maxima if cached by the miVec)
 */
template <typename T>
MI<T> computeComponentWiseMaxParallel(const MIVec<T>& miVec) {
  const std::vector<size_t> partitioning =
      tools::partitionRange(miVec.nMI(), 1, omp_get_max_threads());

  std::vector<MI<T>> localMax(partitioning.size() - 1, MI<T>(miVec.nDim()));

#pragma omp parallel
  {
    const size_t threadId = static_cast<size_t>(omp_get_thread_num());
    const size_t startIdx = partitioning[threadId];
    const size_t endIdx = partitioning[threadId + 1] - 1;

    for (size_t miIdx = startIdx; miIdx <= endIdx; miIdx++) {
      for (size_t dim = 0; dim < miVec.nDim(); dim++) {
        localMax[threadId][dim] = std::max(localMax[threadId][dim], miVec(miIdx, dim));
      }
    }
  }

  return mergeComponentWiseMax(miVec.nDim(), localMax);
}

/*
TODO: Optimize (used pareto Maxima if cached by the miVec)
 */
template <typename T>
MI<T> computeComponentWiseMaxSerial(const MIVec<T>& miVec) {
  MI<T> max(miVec.nDim());

  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    for (size_t dim = 0; dim < miVec.nDim(); dim++) {
      max[dim] = std::max(max[dim], miVec(miIdx, dim));
    }
  }

  return max;
}

template <typename T>
MI<T> computeComponentWiseMax(const MIVec<T>& miVec) {
  if (miVec.nMI() * miVec.nDim() < constants::mi_vec::CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
    return computeComponentWiseMaxSerial(miVec);
  } else {
    return computeComponentWiseMaxParallel(miVec);
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp