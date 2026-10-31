// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/optimized_barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace global_interpolation {

namespace {

/// Number of factors computed at once by the multi-output formula (stack buffer).
constexpr size_t FACTOR_BLOCK_SIZE = 64;

}  // namespace

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

void OptBarycentricFormula::interpolate(const double pos, const std::vector<double>& nodes,
                                        const std::vector<double>& values, const size_t nOutputs,
                                        double* const out) const {
  assert(values.size() == nodes.size() * nOutputs);

  if (hasTrivialResult(pos, nodes, values, nOutputs, out)) {
    return;
  }

  // The factors of the nodes once for all outputs, then output by output: the operations of
  // barycentricFormula per output. The factors are computed in blocks into a buffer on the stack
  // (no allocation for any number of nodes), and the numerators accumulate in out. The factor
  // of node i is w_i / (pos - x_i) with w_i = (-1)^i (halved at both ends), which rounds as
  // barycentricFormula's 1 / (2 (pos - x_0)), curWeight / (pos - x_i), ...
  const size_t nNodes = nodes.size();
  std::array<double, FACTOR_BLOCK_SIZE> factors;
  double denominator = 0.0;
  double sign = 1.0;

  for (size_t start = 0; start < nNodes; start += FACTOR_BLOCK_SIZE) {
    const size_t blockSize = std::min(FACTOR_BLOCK_SIZE, nNodes - start);

    for (size_t j = 0; j < blockSize; j++) {
      const size_t i = start + j;
      const double weight = (i == 0 || i == nNodes - 1) ? 0.5 * sign : sign;
      factors[j] = weight / (pos - nodes[i]);
      denominator += factors[j];
      sign = -sign;
    }

    const double* const blockValues = values.data() + start * nOutputs;
    for (size_t k = 0; k < nOutputs; k++) {
      double numerator = (start == 0) ? 0.0 : out[k];
      for (size_t j = 0; j < blockSize; j++) {
        numerator += factors[j] * blockValues[j * nOutputs + k];
      }
      out[k] = numerator;
    }
  }

  for (size_t k = 0; k < nOutputs; k++) {
    out[k] /= denominator;
  }
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

}  // namespace global_interpolation

}  // namespace combigrid
}  // namespace sgpp
