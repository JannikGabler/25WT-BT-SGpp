// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

InterpolationMethod::InterpolationMethod(const uint64_t id) : id_(id) {}

uint64_t InterpolationMethod::id() const { return id_; }

void InterpolationMethod::interpolate(const double pos, const std::vector<double>& nodes,
                                      const std::vector<double>& values, const size_t nOutputs,
                                      double* const out) const {
  assert(values.size() == nodes.size() * nOutputs);

  const size_t nNodes = nodes.size();
  std::vector<double> outputValues(nNodes);

  for (size_t k = 0; k < nOutputs; k++) {
    // TODO: Optimize? (use a span instead of copying the values)
    for (size_t i = 0; i < nNodes; i++) {
      outputValues[i] = values[i * nOutputs + k];
    }
    out[k] = interpolate(pos, nodes, outputValues);
  }
}

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

bool InterpolationMethod::hasTrivialResult(const double pos, const std::vector<double>& nodes,
                                           const std::vector<double>& values, const size_t nOutputs,
                                           double* const out) const {
  const size_t nNodes = nodes.size();

  if (nNodes == 0) {
    std::fill_n(out, nOutputs, 0.0);
    return true;
  } else if (nNodes == 1) {
    std::copy_n(values.data(), nOutputs, out);
    return true;
  } else if (nNodes == 2) {
    for (size_t k = 0; k < nOutputs; k++) {
      out[k] =
          (values[nOutputs + k] - values[k]) / (nodes[1] - nodes[0]) * (pos - nodes[0]) + values[k];
    }
    return true;
  }

  for (size_t i = 0; i < nNodes; i++) {
    if (tools::nearlyEqual(pos, nodes[i])) {
      std::copy_n(values.data() + i * nOutputs, nOutputs, out);
      return true;
    }
  }

  return false;
}

}  // namespace combigrid
}  // namespace sgpp
