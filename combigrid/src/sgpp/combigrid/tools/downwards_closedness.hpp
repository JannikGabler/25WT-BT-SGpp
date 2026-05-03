/**
 * @file downwards_closedness.hpp
 * @brief Tests and constructions related to the downwards-closedness of
 * multi-index sets.
 *
 * The combination technique requires its level-multi-index set to be
 * downwards closed (DWC): for every @f$\vec{\ell} \in I@f$ all
 * @f$\vec{\ell}'\le\vec{\ell}@f$ must also belong to @f$I@f$.
 */
#ifndef COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP
#define COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP

#include <omp.h>
#include <cstddef>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <sgpp/combigrid/tools/multiindex_bounding_box_generation.hpp>
#include <sgpp/combigrid/tools/multiindex_domination.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Tests whether a multi-index set is downwards closed.
 *
 * For every multi-index @f$\vec{\ell}@f$ in @p miVec the function checks
 * that all immediate predecessors @f$\vec{\ell} - \vec e_k@f$ (for the
 * @f$d@f$ unit vectors that yield non-negative values) are also present.
 * If a single predecessor is missing the parallel scan is cancelled and
 * @c false is returned. Parallelized with OpenMP above the threshold
 * @c constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return @c true iff @p miVec is downwards closed.
 *
 * @note Naive implementation; the unit-bounding-box iteration cost is
 *       @f$O(2^d)@f$ per multi-index.
 */
template <typename T>
bool isMIVecDownwardsClosed(const MIVec<T>& miVec) {
  const misc::DiscUnitBB<T> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup<T>> lookup = miVec.lookup();

  bool closed = true;

#pragma omp parallel for schedule(static) \
    shared(closed) if (miVec.nMI() >= constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const MI<T> mi = miVec[miIdx];

    for (const std::vector<T>& offset : offsets) {
      if (offset <= mi && !lookup->contains(mi - offset)) {
        closed = false;
#pragma omp cancel for
      }
    }
#pragma omp cancellation point for
  }

  return closed;
}

/**
 * @brief Merges per-thread closure batches into a single @c MIVec.
 *
 * @tparam T              Multi-index element type.
 * @param nDim            Multi-index dimensionality.
 * @param localClosures   Per-thread closure entries.
 * @return Combined closure as a single @c MIVec.
 */
template <typename T>
MIVec<T> mergeLocalClosures(const size_t nDim,
                            const std::vector<std::vector<std::vector<T>>>& localClosures) {
  size_t miCnt = 0;

  for (size_t i = 0; i < localClosures.size(); i++) {
    miCnt += localClosures[i].size();
  }

  MIVec<T> closure(nDim, miCnt);
  size_t miIdx = 0;

  for (const std::vector<std::vector<T>>& localClosure : localClosures) {
    for (const std::vector<T>& mi : localClosure) {
      closure.setMI(miIdx++, mi);
    }
  }
  return closure;
}

/**
 * @brief Generates the downwards closure of a multi-index set.
 *
 * Iterates over every multi-index in the bounding box of @p miVec; a
 * candidate is added to the closure iff at least one Pareto-maximal
 * multi-index of @p miVec dominates it. The iteration is partitioned
 * across OpenMP threads.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return Downwards closure of @p miVec.
 *
 * @note Naive @f$O(\#\text{box} \cdot \#\text{Pareto})@f$ approach.
 */
template <typename T>
MIVec<T> genMIVecDownwardsClosure(const MIVec<T>& miVec) {
  const std::shared_ptr<std::vector<size_t>> paretoMaxima = miVec.paretoMaxima();
  const misc::DiscRectBB<T> boundingBox = genRectMIBoundingBox(miVec);
  const auto part = partitionRangeForConcurrency(boundingBox.size(), 1, 1);  // TODO

  std::vector<std::vector<std::vector<T>>> localClosures(part.size() - 1);

#pragma omp parallel num_threads(part.size() - 1)
  {
    const size_t threadId = static_cast<size_t>(omp_get_thread_num());
    const size_t startIdx = part[threadId];
    const size_t endIdx = part[threadId + 1];
    auto iter = boundingBox.begin(startIdx);

    for (size_t i = startIdx; i < endIdx; i++) {
      const std::vector<T> mi = *iter;

      if (miVecDominatesMI(miVec, *paretoMaxima, mi)) {
        localClosures[threadId].emplace_back(mi);
      }

      ++iter;
    }
  }

  return mergeLocalClosures(miVec.nDim(), localClosures);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
