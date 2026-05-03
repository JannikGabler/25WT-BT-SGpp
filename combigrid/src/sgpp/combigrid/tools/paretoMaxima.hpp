/**
 * @file paretoMaxima.hpp
 * @brief Computation of Pareto-maximal multi-indices in an @c MIVec.
 *
 * A multi-index @f$\vec a@f$ is dominated by @f$\vec b@f$ iff
 * @f$a_k \le b_k@f$ in every dimension and @f$\vec a \ne \vec b@f$. The
 * Pareto-maximal entries are those that are not dominated by any other
 * entry. They are needed in several combination-technique steps (e.g.
 * locating the "active set" of leaf tensor grids).
 *
 * Two algorithmic regimes are implemented:
 * - downwards-closed input ("DWC"): a multi-index is Pareto-maximal iff
 *   none of its component-wise successors is contained in @c miVec; this
 *   admits a fast hash-based test;
 * - arbitrary input ("non-DWC"): the classical pairwise comparison algorithm.
 *
 * Each regime has serial and OpenMP-parallel implementations; the latter
 * are activated above the size threshold in @c constants::mi_vec.
 */
#pragma once

#include <omp.h>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
// #include <sgpp/combigrid/tools/multiindex_domination.hpp>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

}
}  // namespace sgpp

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Tests if @p candidateIdx is Pareto-maximal under the
 * downwards-closed assumption and appends it to @p paretoMaxima if so.
 *
 * Uses a single hash lookup per dimension: the candidate is dominated iff
 * one of its immediate component-wise successors lies inside @p lookup.
 *
 * @tparam T          Multi-index element type.
 * @param miVec       Source vector of multi-indices.
 * @param lookup      Hash lookup over @p miVec.
 * @param paretoMaxima In/out: list of Pareto-maximal indices found so far.
 * @param candidateIdx Index of the candidate inside @p miVec.
 */
template <typename T>
void updateParetoMaximaDWC(const combigrid::MIVec<T>& miVec, const misc::MIVecLookup<T>& lookup,
                           std::vector<size_t>& paretoMaxima, const size_t candidateIdx) {
  MI<T> candidate = miVec[candidateIdx];

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    if (dim > 0) {
      candidate[dim - 1]--;
    }
    candidate[dim]++;

    if (lookup.contains(candidate)) {
      return;
    }
  }

  paretoMaxima.push_back(candidateIdx);
}

/**
 * @brief Tests if @p candidateIdx is Pareto-maximal in the general case.
 *
 * Compares the candidate against the running list @p paretoMaxima: drops
 * any entries the candidate dominates and only appends the candidate if
 * it is not itself dominated.
 *
 * @tparam T          Multi-index element type.
 * @param miVec       Source vector of multi-indices.
 * @param paretoMaxima In/out: running list of Pareto-maximal indices.
 * @param candidateIdx Index of the candidate inside @p miVec.
 */
template <typename T>
void updateParetoMaximaNonDWC(const combigrid::MIVec<T>& miVec, std::vector<size_t>& paretoMaxima,
                              const size_t candidateIdx) {
  bool dominated = false;

  for (auto iter = paretoMaxima.begin(); iter != paretoMaxima.end(); iter++) {
    if (miDominatesMI(miVec, *iter, candidateIdx)) {
      dominated = true;
      break;
    }
    if (miDominatesMI(miVec, candidateIdx, *iter)) {
      iter = paretoMaxima.erase(iter) - 1;
    }
  }

  if (!dominated) {
    paretoMaxima.push_back(candidateIdx);
  }
}

/**
 * @brief Merges per-thread non-DWC Pareto-maxima lists into a single list.
 *
 * Sequentially feeds every entry of every local list through
 * @ref updateParetoMaximaNonDWC, so that the final list contains only
 * globally Pareto-maximal indices.
 *
 * @tparam T              Multi-index element type.
 * @param miVec           Source vector of multi-indices.
 * @param localParetoMaxima Per-thread Pareto maxima.
 * @return Globally Pareto-maximal indices.
 */
template <typename T>
std::vector<size_t> mergeParetoMax(const combigrid::MIVec<T>& miVec,
                                   std::vector<std::vector<size_t>>& localParetoMaxima) {
  std::vector<size_t> merged;

  for (const std::vector<size_t>& local : localParetoMaxima) {
    for (const size_t candidateIdx : local) {
      updateParetoMaximaNonDWC(miVec, merged, candidateIdx);
    }
  }

  return merged;
}

/**
 * @brief Concatenates per-thread Pareto-maxima lists in order.
 *
 * Used in the DWC parallel path where local lists are already globally
 * Pareto-maximal (no merging step needed).
 *
 * @param localParetoMaxima Per-thread Pareto maxima.
 * @return Concatenation of the per-thread lists.
 */
inline std::vector<size_t> zipParetoMax(std::vector<std::vector<size_t>>& localParetoMaxima) {
  size_t totalLength = 0;
  for (size_t i = 0; i < localParetoMaxima.size(); i++) {
    totalLength += localParetoMaxima[i].size();
  }

  std::vector<size_t> result;
  result.reserve(totalLength);

  for (const std::vector<size_t>& local : localParetoMaxima) {
    result.insert(result.end(), local.begin(), local.end());
  }

  return result;
}

/**
 * @brief Serial DWC Pareto-maxima computation.
 * @tparam T   Multi-index element type.
 * @param miVec Downwards-closed source set.
 * @return Indices of Pareto-maximal multi-indices.
 */
template <typename T>
std::vector<size_t> computeParetoMaxSerialDWC(const combigrid::MIVec<T>& miVec) {
  std::vector<size_t> paretoMaxima;

  const misc::MIVecLookup<T> lookup(miVec);

  for (size_t candidateIdx = 0; candidateIdx < miVec.nMI(); candidateIdx++) {
    updateParetoMaximaDWC(miVec, lookup, paretoMaxima, candidateIdx);
  }

  return paretoMaxima;
}

/**
 * @brief Serial non-DWC Pareto-maxima computation on the index sub-range
 * @c [startIdx, endIdx].
 * @tparam T      Multi-index element type.
 * @param miVec    Source set.
 * @param startIdx Inclusive start index.
 * @param endIdx   Inclusive end index.
 * @return Indices of locally Pareto-maximal multi-indices in that range.
 */
template <typename T>
std::vector<size_t> computeParetoMaxSerialNotDWC(const combigrid::MIVec<T>& miVec,
                                                 const size_t startIdx, const size_t endIdx) {
  std::vector<size_t> paretoMaxima;

  for (size_t candidateIdx = startIdx; candidateIdx <= endIdx; candidateIdx++) {
    updateParetoMaximaNonDWC(miVec, paretoMaxima, candidateIdx);
  }

  return paretoMaxima;
}

/**
 * @brief Parallel DWC Pareto-maxima computation (OpenMP).
 *
 * Each thread processes a static slice of @p miVec; per-thread results
 * are concatenated since the DWC test is independent across candidates.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Downwards-closed source set.
 * @return Indices of Pareto-maximal multi-indices.
 */
template <typename T>
std::vector<size_t> computeParetoMaxParallelDWC(const combigrid::MIVec<T>& miVec) {
  const size_t nThreads = static_cast<size_t>(omp_get_max_threads());
  const misc::MIVecLookup<T> lookup(miVec);

  std::vector<std::vector<size_t>> localParetoMaxima(nThreads);

#pragma omp parallel
  {
    const int threadId = omp_get_thread_num();

#pragma omp for schedule(static)
    for (size_t candidateIdx = 0; candidateIdx < miVec.nMI(); candidateIdx++) {
      updateParetoMaximaDWC<T>(miVec, lookup, localParetoMaxima[threadId], candidateIdx);
    }
  }

  return zipParetoMax(localParetoMaxima);
}

/**
 * @brief Parallel non-DWC Pareto-maxima computation (OpenMP).
 *
 * Splits @p miVec into batches sized so that each thread processes at
 * least @c PM_MIN_MIVEC_BATCH_LENGTH_PER_THREAD entries; per-thread
 * Pareto sets are merged at the end with @ref mergeParetoMax.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return Indices of Pareto-maximal multi-indices.
 */
template <typename T>
std::vector<size_t> computeParetoMaxParallelNonDWC(const combigrid::MIVec<T>& miVec) {
  // const size_t length = miVec.nMI() * miVec.nDim();
  const size_t minBatchSize =
      (constants::mi_vec::PM_MIN_MIVEC_BATCH_LENGTH_PER_THREAD + miVec.nDim() - 1) / miVec.nDim();

  const std::vector<size_t> partitioning = tools::partitionRange(miVec.nMI(), minBatchSize);
  std::vector<std::vector<size_t>> localParetoMaxima(partitioning.size() - 1);

#pragma omp parallel num_threads(partitioning.size() - 1)
  {
    const int threadId = omp_get_thread_num();

    localParetoMaxima[threadId] =
        computeParetoMaxSerialNotDWC(miVec, partitioning[threadId], partitioning[threadId + 1] - 1);
  }

  return mergeParetoMax(miVec, localParetoMaxima);
}

/**
 * @brief Public entry point: dispatches to the appropriate serial/parallel,
 * DWC/non-DWC implementation.
 *
 * @tparam T                  Multi-index element type.
 * @param miVec               Source set.
 * @param isDownwardsClosed    Set this to @c true when @p miVec is known to
 *                            be downwards closed (allows the faster DWC path).
 * @return Indices of Pareto-maximal multi-indices.
 */
template <typename T>
std::vector<size_t> computeParetoMaxima(const combigrid::MIVec<T>& miVec,
                                        const bool isDownwardsClosed) {
  const size_t length = miVec.nMI() * miVec.nDim();
  if (length == 0) {
    return std::vector<size_t>{};
  }

  if (isDownwardsClosed) {
    if (length < constants::mi_vec::PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
      return computeParetoMaxSerialDWC<T>(miVec);
    } else {
      return computeParetoMaxParallelDWC<T>(miVec);
    }
  } else {
    if (length < constants::mi_vec::PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
      return computeParetoMaxSerialNotDWC<T>(miVec, 0, miVec.nMI() - 1);
    } else {
      return computeParetoMaxParallelNonDWC<T>(miVec);
    }
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp