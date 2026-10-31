// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

SourceFunc::SourceFunc(const std::function<double(const base::DataVector&)>& func) : func(func) {}

SourceFunc::SourceFunc(std::function<double(const base::DataVector&)>&& func)
    : func(std::move(func)) {}

SourceFunc::SourceFunc(std::function<double(const base::DataVector&)> func,
                       std::shared_ptr<SourceFuncHook> hook)
    : func(std::move(func)), hook_(std::move(hook)) {}

double SourceFunc::evaluate(const base::DataVector& point) const { return func(point); }

double SourceFunc::evaluateNormalizedInPlace(base::DataVector& point,
                                             const HyperCubeArea& area) const {
  assert(point.size() == area.size());

  tools::denormalizeDataVector(point, area);

  return evaluate(point);
}

double SourceFunc::evaluateNormalizedOutOfPlace(const base::DataVector& point,
                                                const HyperCubeArea& area) const {
  assert(point.size() == area.size());

  return evaluate(tools::denormalizeDataVector(point, area));
}

void SourceFunc::finishOperatorCall() const {
  if (hook_ != nullptr) {
    hook_->finishOperatorCall();
  }
}

const std::shared_ptr<SourceFuncHook>& SourceFunc::hook() const { return hook_; }

}  // namespace combigrid

}  // namespace sgpp
