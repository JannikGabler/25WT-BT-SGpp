// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace {

size_t checkedNOutputs(const size_t nOutputs) {
  if (nOutputs == 0) {
    throw base::data_exception("VectorSourceFunc: the number of outputs must be at least 1");
  }

  return nOutputs;
}

}  // namespace

VectorSourceFunc::VectorSourceFunc(
    const std::function<void(const base::DataVector&, std::span<double>)>& func,
    const size_t nOutputs)
    : func(func), nOutputs_(checkedNOutputs(nOutputs)) {}

VectorSourceFunc::VectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)>&& func, const size_t nOutputs)
    : func(std::move(func)), nOutputs_(checkedNOutputs(nOutputs)) {}

size_t VectorSourceFunc::nOutputs() const { return nOutputs_; }

void VectorSourceFunc::evaluate(const base::DataVector& point, std::span<double> out) const {
  assert(out.size() == nOutputs_);

  func(point, out);
}

void VectorSourceFunc::evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area,
                                                 std::span<double> out) const {
  assert(point.size() == area.size());

  tools::denormalizeDataVector(point, area);

  evaluate(point, out);
}

}  // namespace combigrid
}  // namespace sgpp
