#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/iterators/hyperbox_iterator.hpp>
#include <sgpp/combigrid/tools/iterators/iterations.hpp>
#include <sgpp/combigrid/tools/multiindex_bounding_boxes.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

MIVec genDownwardsClosure(const MIVec& miVec) {
  const std::vector<size_t> paretoMaxima = miVec.paretoMaxima();
  const std::vector<MIType> boundingBox = genRectMIBoundingBox(miVec, paretoMaxima);

  std::vector<MI> closure(boundingBox.size());
  std::vector<MIType> curPos(miVec.nDim());

  do {
  } while (iterateHyperbox(curPos, boundingBox));
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp