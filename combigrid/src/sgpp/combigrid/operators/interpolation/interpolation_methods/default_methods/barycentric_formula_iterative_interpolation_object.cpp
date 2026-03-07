#pragma once

#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/default_methods/barycentric_formula_iterative_interpolation_object.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/operators/interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace interpolation_methods {

BarycentricFormulaIterativeInterpolationObj::BarycentricFormulaIterativeInterpolationObj(
    const double pos, const std::vector<double>& nodes)
    : IterativeInterpolationObj(pos, nodes),
      weights(tools::barycentricFormulaWeights(nodes)),
      curNumerator(0),
      curDenominator(0) {}

void BarycentricFormulaIterativeInterpolationObj::addValue(const double value) {
  assert(curIdx < nodes.size());

  const double factor = weights[curIdx] / (pos - nodes[curIdx]);

  curNumerator += factor * value;
  curDenominator += factor;

  curIdx++;
}

double BarycentricFormulaIterativeInterpolationObj::getResult() {
  return curNumerator / curDenominator;
}

}  // namespace interpolation_methods

}  // namespace combigrid
}  // namespace sgpp
