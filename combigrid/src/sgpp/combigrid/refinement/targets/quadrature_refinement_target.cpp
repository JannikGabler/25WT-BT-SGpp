// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/refinement/targets/quadrature_refinement_target.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

QuadratureRefinementTarget::QuadratureRefinementTarget(const SourceFunc& sourceFunc)
    : scalarFunc_(&sourceFunc) {}

QuadratureRefinementTarget::QuadratureRefinementTarget(const VectorSourceFunc& sourceFunc)
    : vectorFunc_(&sourceFunc) {}

QuadratureRefinementTarget::~QuadratureRefinementTarget() = default;

size_t QuadratureRefinementTarget::nValues() const {
  return scalarFunc_ != nullptr ? 1 : vectorFunc_->nOutputs();
}

void QuadratureRefinementTarget::beginRefinementCall(const SparseGridSetup& setup,
                                                     const NodeTable& unitNodeTable,
                                                     const size_t nWorkers) {
  volume_ = setup.getVolumeOfDomain();
  weightTable_ = std::make_unique<misc::QuadWeightTable>(unitNodeTable, setup);
  const size_t nOutputs = scalarFunc_ != nullptr ? 0 : vectorFunc_->nOutputs();
  scratches_.clear();
  scratches_.reserve(nWorkers);
  for (size_t worker = 0; worker < nWorkers; worker++) {
    scratches_.emplace_back(setup.nDim(), nOutputs);
  }
}

void QuadratureRefinementTarget::estimate(const TensorGridView& domainTG,
                                          const TensorGridView& /*unitTG*/, const size_t worker,
                                          const std::span<double> out) {
  if (scalarFunc_ != nullptr) {
    out[0] = tools::quadrature::processTensorGrid(*weightTable_, *scalarFunc_, domainTG,
                                                  scratches_[worker]);
  } else {
    tools::quadrature::processTensorGrid(*weightTable_, *vectorFunc_, domainTG, scratches_[worker],
                                         out);
  }
}

void QuadratureRefinementTarget::finishCombination(const std::span<double> values) const {
  for (double& value : values) {
    value *= volume_;
  }
}

void QuadratureRefinementTarget::endRefinementCall() {
  weightTable_.reset();
  scratches_.clear();
  if (scalarFunc_ != nullptr) {
    scalarFunc_->finishOperatorCall();
  } else {
    vectorFunc_->finishOperatorCall();
  }
}

}  // namespace combigrid
}  // namespace sgpp
