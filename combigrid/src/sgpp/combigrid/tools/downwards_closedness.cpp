#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/iterators.hpp>
#include <sgpp/combigrid/tools/multiindex_bounding_boxes.hpp>
#include <vector>
#include "sgpp/combigrid/tools/multiindex_domination.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

/*
Naive approach
TODO: Optimize
*/
bool isMIVecDownwardsClosed(const MIVec& miVec) {
#pragma omp parallel for schedule(static)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    MI curMI(miVec.nDim(), 0);
  }
}

/*
Naive approach
TODO: Optimize
*/
MIVec genMIVecDownwardsClosure(const MIVec& miVec) {
  const std::vector<size_t> paretoMaxima = miVec.paretoMaxima();
  const std::vector<MIType> boundingBox = genRectMIBoundingBox(miVec, paretoMaxima);

  std::vector<MI> closure(boundingBox.size());
  MI curMI(miVec.nDim(), 0);

  do {
    if (miVecDominatesMI(miVec, paretoMaxima, curMI)) {
      closure.push_back(curMI);
    }
  } while (iterateHyperbox(curMI, boundingBox));

  return MIVec(closure);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp