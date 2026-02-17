#include <cstddef>
#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

// misc::DiscRectBB<MIType> genRectMIBoundingBox(const std::vector<MI>& mi) {
//   // TODO
//   throw base::not_implemented_exception("Operation is not yet implemented!");
// }

misc::DiscRectBB<MIType> genRectMIBoundingBox(const MIVec& miVec) {
  const std::shared_ptr<MI> max = miVec.componentWiseMax();
  const std::vector<MIType> lowerBound(max->size(), 0);

  return misc::DiscRectBB<MIType>(lowerBound, *max);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp