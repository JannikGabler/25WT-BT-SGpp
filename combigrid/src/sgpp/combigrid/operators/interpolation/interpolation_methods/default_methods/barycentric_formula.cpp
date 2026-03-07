#include <cassert>
#include <memory>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/tools/operators/interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace interpolation_methods {

BarycentricFormula::BarycentricFormula(const NodeGenFunc* nodeGenFunc)
    : InterpolationMethod(tools::fnv1aHashCombine(
          tools::fnv1aHash("Barycentric Formula Interpolation Method"), nodeGenFunc->id())) {}

double BarycentricFormula::interpolate(const double pos, const std::vector<double>& nodes,
                                       const std::vector<double>& values) const {
  assert(nodes.size() == values.size());

  double result;

  if (!hasTrivialResult(pos, nodes, values, result)) {
    result = evaluateFormula(pos, nodes, values);
  }

  return result;
}

double BarycentricFormula::evaluateFormula(const double pos, const std::vector<double>& nodes,
                                           const std::vector<double>& values) const {
  const size_t nNodes = nodes.size();
  const std::vector<double> weights = tools::barycentricFormulaWeights(nodes);

  double numerator = 0;
  double denominator = 0;

  for (size_t i = 1; i < nNodes - 1; i++) {
    const double factor = weights[i] / (pos - nodes[i]);

    numerator += factor * values[i];
    denominator += factor;
  }

  return numerator / denominator;
}

}  // namespace interpolation_methods

}  // namespace combigrid
}  // namespace sgpp