#include <memory>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/iterators.hpp>
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

#pragma omp parallel for schedule(static) if (miVec.nMI() >= 5)  // TODO
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const MI mi = miVec[miIdx];

    for (const std::vector<MIType>& offset : offsets) {
      if (offset <= mi && !lookup->contains(mi)) {
        closed = false;
      }
    }
  }
}

/*
Naive approach
TODO: Optimize
*/
// MIVec genMIVecDownwardsClosure(const MIVec& miVec) {
//   const std::vector<size_t> paretoMaxima = miVec.paretoMaxima();
//   const std::vector<MIType> boundingBox = genRectMIBoundingBox(miVec, paretoMaxima);

//   std::vector<MI> closure(boundingBox.size());
//   MI curMI(miVec.nDim(), 0);

//   do {
//     if (miVecDominatesMI(miVec, paretoMaxima, curMI)) {
//       closure.push_back(curMI);
//     }
//   } while (iterateHyperbox(curMI, boundingBox));

//   return MIVec(closure);
// }

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp