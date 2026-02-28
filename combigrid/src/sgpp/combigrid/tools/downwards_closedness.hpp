#ifndef COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP
#define COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP

#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/tools/multiindex_bounding_box_generation.hpp>
#include <sgpp/combigrid/tools/multiindex_domination.hpp>

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
/*
Naive approach
TODO: Optimize
*/
MIVec<T> genMIVecDownwardsClosure(const MIVec<T>& miVec) {
  const std::shared_ptr<std::vector<size_t>> paretoMaxima = miVec.paretoMaxima();
  const misc::DiscRectBB<T> boundingBox = genRectMIBoundingBox(miVec);

  std::vector<std::vector<T>> closure{};
  closure.reserve(boundingBox.size());

  for (const std::vector<T>& mi : boundingBox) {
    if (miVecDominatesMI(miVec, *paretoMaxima, mi)) {
      closure.push_back(mi);
    }
  }

  return MIVec<T>(closure);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
