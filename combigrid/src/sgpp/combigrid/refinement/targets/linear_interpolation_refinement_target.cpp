// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/refinement/targets/linear_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {

/// The rows of @p points as vectors.
std::vector<base::DataVector> rowsOf(const base::DataMatrix& points) {
  std::vector<base::DataVector> rows(points.getNrows(), base::DataVector(points.getNcols()));
  for (size_t row = 0; row < points.getNrows(); row++) {
    points.getRow(row, rows[row]);
  }
  return rows;
}

}  // namespace

LinearInterpolationRefinementTarget::LinearInterpolationRefinementTarget(
    const SourceFunc& sourceFunc, const base::DataMatrix& points)
    : scalarFunc_(&sourceFunc), points_(rowsOf(points)), setup_(0) {}

LinearInterpolationRefinementTarget::LinearInterpolationRefinementTarget(
    const VectorSourceFunc& sourceFunc, const base::DataMatrix& points)
    : vectorFunc_(&sourceFunc), points_(rowsOf(points)), setup_(0) {}

LinearInterpolationRefinementTarget::~LinearInterpolationRefinementTarget() = default;

size_t LinearInterpolationRefinementTarget::nValues() const {
  return points_.size() * (scalarFunc_ != nullptr ? 1 : vectorFunc_->nOutputs());
}

void LinearInterpolationRefinementTarget::beginRefinementCall(const SparseGridSetup& setup,
                                                              const NodeTable& /*unitNodeTable*/,
                                                              const size_t nWorkers) {
  setup_ = setup;
  normPoints_.clear();
  for (const base::DataVector& point : points_) {
    normPoints_.push_back(tools::normalizeDataVector(point, setup.getDomain()));
  }
  const size_t nOutputs = scalarFunc_ != nullptr ? 0 : vectorFunc_->nOutputs();
  scratches_.clear();
  scratches_.reserve(nWorkers);
  for (size_t worker = 0; worker < nWorkers; worker++) {
    scratches_.emplace_back(setup.nDim(), nOutputs);
  }
}

void LinearInterpolationRefinementTarget::estimate(const TensorGridView& /*domainTG*/,
                                                   const TensorGridView& unitTG,
                                                   const size_t worker,
                                                   const std::span<double> out) {
  misc::LinInterpScratch& scratch = scratches_[worker];
  if (scalarFunc_ != nullptr) {
    for (size_t p = 0; p < normPoints_.size(); p++) {
      out[p] = tools::lin_interpolation::processTensorGrid(*scalarFunc_, normPoints_[p], unitTG,
                                                           setup_, scratch);
    }
    return;
  }
  const size_t m = vectorFunc_->nOutputs();
  for (size_t p = 0; p < normPoints_.size(); p++) {
    tools::lin_interpolation::processTensorGrid(*vectorFunc_, normPoints_[p], unitTG, setup_,
                                                scratch, out.subspan(p * m, m));
  }
}

void LinearInterpolationRefinementTarget::endRefinementCall() {
  scratches_.clear();
  if (scalarFunc_ != nullptr) {
    scalarFunc_->finishOperatorCall();
  } else {
    vectorFunc_->finishOperatorCall();
  }
}

}  // namespace combigrid
}  // namespace sgpp
