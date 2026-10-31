// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace global_interpolation {

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

void BarycentricFormula::interpolate(const double pos, const std::vector<double>& nodes,
                                     const std::vector<double>& values, const size_t nOutputs,
                                     double* const out) const {
  assert(values.size() == nodes.size() * nOutputs);

  if (hasTrivialResult(pos, nodes, values, nOutputs, out)) {
    return;
  }

  // The factors of the nodes once for all outputs (in place of the weights), then output by
  // output (the operations of evaluateFormula per output)
  const size_t nNodes = nodes.size();
  std::vector<double> factors = tools::barycentricFormulaWeights(nodes);
  double denominator = 0.0;

  for (size_t i = 0; i < nNodes; i++) {
    factors[i] = factors[i] / (pos - nodes[i]);
    denominator += factors[i];
  }

  for (size_t k = 0; k < nOutputs; k++) {
    double numerator = 0.0;
    for (size_t i = 0; i < nNodes; i++) {
      numerator += factors[i] * values[i * nOutputs + k];
    }
    out[k] = numerator / denominator;
  }
}

double BarycentricFormula::evaluateFormula(const double pos, const std::vector<double>& nodes,
                                           const std::vector<double>& values) const {
  const size_t nNodes = nodes.size();
  const std::vector<double> weights = tools::barycentricFormulaWeights(nodes);

  double numerator = 0.0;
  double denominator = 0.0;

  for (size_t i = 0; i < nNodes; i++) {
    const double factor = weights[i] / (pos - nodes[i]);

    numerator += factor * values[i];
    denominator += factor;
  }

  return numerator / denominator;
}

}  // namespace global_interpolation

}  // namespace combigrid
}  // namespace sgpp
