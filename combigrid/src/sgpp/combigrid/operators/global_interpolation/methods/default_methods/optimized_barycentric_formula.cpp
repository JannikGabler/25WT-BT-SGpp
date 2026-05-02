#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/optimized_barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>

namespace sgpp {
namespace combigrid {

namespace interpolation {

OptBarycentricFormula::OptBarycentricFormula()
    : InterpolationMethod(tools::fnv1aHash("Optimized Barycentric Formula Interpolation Method")) {}

double OptBarycentricFormula::interpolate(const double pos, const std::vector<double>& nodes,
                                          const std::vector<double>& values) const {
  assert(nodes.size() == values.size());

  double result;

  if (!hasTrivialResult(pos, nodes, values, result)) {
    result = barycentricFormula(pos, nodes, values);
  }

  return result;
}

double OptBarycentricFormula::barycentricFormula(const double pos, const std::vector<double>& nodes,
                                                 const std::vector<double>& values) const {
  const size_t nNodes = nodes.size();

  double curWeight = 1.0;
  double curFactor = 1.0 / (2.0 * (pos - nodes[0]));
  double numerator = values[0] * curFactor;
  double denominator = curFactor;

  for (size_t i = 1; i < nNodes - 1; i++) {
    curWeight *= -1.0;

    curFactor = curWeight / (pos - nodes[i]);
    numerator += curFactor * values[i];
    denominator += curFactor;
  }

  curFactor = -curWeight / (2.0 * (pos - nodes[nNodes - 1]));
  numerator += curFactor * values[nNodes - 1];
  denominator += curFactor;

  return numerator / denominator;
}

}  // namespace interpolation

}  // namespace combigrid
}  // namespace sgpp