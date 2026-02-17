#include <memory>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
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
bool isMIVecDownwardsClosed(const MIVec& miVec) {
  const misc::DiscUnitBB<MIType> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup> lookup = miVec.lookup();

  bool closed = true;

#pragma omp parallel for schedule(static) \
    shared(closed) if (miVec.nMI() >= constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const MI mi = miVec[miIdx];

    for (const std::vector<MIType>& offset : offsets) {
      if (offset <= mi && !lookup->contains(mi - offset)) {
        closed = false;
#pragma omp cancel for
      }
    }
#pragma omp cancellation point for
  }

  return closed;
}

/*
Naive approach
TODO: Optimize
*/
MIVec genMIVecDownwardsClosure(const MIVec& miVec) {
  const std::shared_ptr<std::vector<size_t>> paretoMaxima = miVec.paretoMaxima();
  const misc::DiscRectBB<MIType> boundingBox = genRectMIBoundingBox(miVec);

  std::vector<std::vector<MIType>> closure(boundingBox.size());

  for (const std::vector<MIType>& mi : boundingBox) {
    if (miVecDominatesMI(miVec, *paretoMaxima, mi)) {
      closure.push_back(mi);
    }
  }

  return MIVec(closure);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp