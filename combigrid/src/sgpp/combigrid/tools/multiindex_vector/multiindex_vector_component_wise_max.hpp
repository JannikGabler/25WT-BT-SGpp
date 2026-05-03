/**
 * @file multiindex_vector_component_wise_max.hpp
 * @brief Component-wise maximum of every multi-index in an @c MIVec.
 */
#ifndef COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP

#include <omp.h>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  // Forward Declaration

namespace tools {

/**
 * @brief Merges per-thread component-wise maxima into a single multi-index.
 * @tparam T   Multi-index element type.
 * @param nDim Multi-index dimensionality.
 * @param localMax Per-thread maxima.
 * @return Component-wise maximum across @p localMax.
 */
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

/**
 * @brief Parallel component-wise maximum (OpenMP).
 *
 * Splits @p miVec into one chunk per thread, computes a local maximum
 * per chunk, and merges the per-thread results.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return Component-wise maximum across all multi-indices.
 *
 * @note Could be sped up if the cached Pareto maxima are already
 * available on @p miVec (TODO).
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

/**
 * @brief Serial component-wise maximum.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return Component-wise maximum across all multi-indices.
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

/**
 * @brief Public entry point: dispatches to the serial or parallel
 * implementation based on @c CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return Component-wise maximum across all multi-indices.
 */
template <typename T>
MI<T> computeComponentWiseMax(const combigrid::MIVec<T>& miVec) {
  if (miVec.nMI() * miVec.nDim() < constants::mi_vec::CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
    return computeComponentWiseMaxSerial(miVec);
  } else {
    return computeComponentWiseMaxParallel(miVec);
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif