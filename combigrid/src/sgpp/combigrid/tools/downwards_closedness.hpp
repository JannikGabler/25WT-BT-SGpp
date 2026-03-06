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

/*
Simple approach
TODO: Optimize
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

/*
Naive approach
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
