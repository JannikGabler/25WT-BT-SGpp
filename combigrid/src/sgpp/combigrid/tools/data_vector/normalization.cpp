#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

void normalizeDataVector(base::DataVector& point, const HyperCubeArea& area) {
  assert(point.size() == area.size());

  for (size_t dim = 0; dim < point.size(); dim++) {
    point[dim] -= area[dim].first;
    point[dim] /= area[dim].second - area[dim].first;
  }
}

base::DataVector normalizeDataVector(const base::DataVector& point, const HyperCubeArea& area) {
  assert(point.size() == area.size());

  base::DataVector result(point);

  for (size_t dim = 0; dim < result.size(); dim++) {
    result[dim] -= area[dim].first;
    result[dim] /= area[dim].second - area[dim].first;
  }

  return result;
}

void denormalizeDataVector(base::DataVector& point, const HyperCubeArea& area) {
  assert(point.size() == area.size());

  for (size_t dim = 0; dim < point.size(); dim++) {  // Transform on to the area
    point[dim] *= area[dim].second - area[dim].first;
    point[dim] += area[dim].first;
  }
}

base::DataVector denormalizeDataVector(const base::DataVector& point, const HyperCubeArea& area) {
  assert(point.size() == area.size());

  base::DataVector result(point);

  for (size_t dim = 0; dim < point.size(); dim++) {  // Transform on to the area
    result[dim] *= area[dim].second - area[dim].first;
    result[dim] += area[dim].first;
  }

  return result;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp