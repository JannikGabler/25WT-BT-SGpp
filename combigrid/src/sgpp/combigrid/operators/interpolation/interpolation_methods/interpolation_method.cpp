#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

InterpolationMethod::InterpolationMethod(const uint64_t id) : id_(id) {}

uint64_t InterpolationMethod::id() const { return id_; }

bool InterpolationMethod::hasTrivialResult(const double pos, const std::vector<double>& nodes,
                                           const std::vector<double>& values, double& out) const {
  const size_t nNodes = nodes.size();

  if (nNodes == 0) {
    out = 0;
    return true;
  } else if (nNodes == 1) {
    out = values[0];
    return true;
  } else if (nNodes == 2) {
    out = (values[1] - values[0]) / (nodes[1] - nodes[0]) * (pos - nodes[0]) + values[0];
    return true;
  }

  for (size_t i = 0; i < nNodes; i++) {
    if (tools::nearlyEqual(pos, nodes[i])) {
      out = values[i];
      return true;
    }
  }

  return false;
}

}  // namespace combigrid
}  // namespace sgpp
