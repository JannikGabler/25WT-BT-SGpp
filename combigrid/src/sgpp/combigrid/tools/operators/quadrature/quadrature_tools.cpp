// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <algorithm>
#include <cassert>
#include <memory>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/tools/operators/cross_call/cross_call_operator_data_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;
using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

void estimateTensorGrids(const QuadratureContext& context, const SourceFunc& sourceFunc,
                         const std::span<double> tgEstimates) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nTG = sparseGrid.nTG();
  assert(tgEstimates.size() == nTG);
  double* const estimates = tgEstimates.data();

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim());
    context.weightTable.computeAllRows();  // Nowait operation: Threads can immediately continue

    // The tensor grids differ in size by orders of magnitude -> Dynamic scheduling
#pragma omp for schedule(dynamic) nowait
    for (size_t tgIdx = 0; tgIdx < nTG; tgIdx++) {
      estimates[tgIdx] = processTensorGrid(
          context, sourceFunc, sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid, scratch);
    }
  }
}

void estimateTensorGrids(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                         const std::span<double> tgEstimates) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nTG = sparseGrid.nTG();
  const size_t nOutputs = sourceFunc.nOutputs();
  assert(tgEstimates.size() == nTG * nOutputs);
  double* const estimates = tgEstimates.data();

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim(), nOutputs);
    context.weightTable.computeAllRows();  // As in the scalar operator

    // Dynamic scheduling as in the scalar operator
#pragma omp for schedule(dynamic) nowait
    for (size_t tgIdx = 0; tgIdx < nTG; tgIdx++) {
      processTensorGrid(context, sourceFunc, sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid,
                        scratch, std::span<double>(estimates + tgIdx * nOutputs, nOutputs));
    }
  }
}

double processTensorGrid(const QuadratureContext& context, const SourceFunc& sourceFunc,
                         const TensorGridView tg, QuadratureScratch& scratch) {
  assert(tg.nDim() == context.sparseGrid.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  prepareTensorGrid(context, tg, scratch);

  return weighted_sum::sumOverRows(misc::ScalarValues(sourceFunc), scratch.rows);
}

void processTensorGrid(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                       const TensorGridView tg, QuadratureScratch& scratch,
                       const std::span<double> out) {
  assert(tg.nDim() == context.sparseGrid.nDim());
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.rows.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), WeightedSumScratch::N_ACCS));

  if (tg.nGP() == 0) {
    std::ranges::fill(out, 0.0);
    return;
  }

  prepareTensorGrid(context, tg, scratch);

  const misc::VectorValues values(sourceFunc, scratch.rows.valueBuffer);
  values.store(weighted_sum::sumOverRows(values, scratch.rows), out.data());
}

/**
 * Prepares the row loop over @p tg (weighted_sum::sumOverRows): 1D quadrature weights, iterated
 * dimensions.
 */
void prepareTensorGrid(const QuadratureContext& context, const TensorGridView tg,
                       QuadratureScratch& scratch) {
  getWeights(context, tg, scratch);
  weighted_sum::setUpIterDims(tg, scratch.rows);
}

void getWeights(const QuadratureContext& context, const TensorGridView tg,
                QuadratureScratch& scratch) {
  const LvlMIView lvls = tg.getLevels();
  const double** const weightsOfDim = scratch.rows.weightsOfDim.data();

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    weightsOfDim[dim] = context.weightTable.weights(dim, lvls[dim]);
  }
}

std::shared_ptr<const misc::QuadWeightTable> getWeightTable(const SparseGrid& sparseGrid,
                                                            const bool useCache) {
  static constexpr misc::CrossCallOperatorDataDescriptor DESCRIPTOR{
      .operatorName = "quadrature (SparseGrid)",
      .description = "1D quadrature weights per (dimension, level) row of the unit node table",
      .key = ""};
  return cross_operator_call::getOperatorData<misc::QuadWeightTable>(
      sparseGrid, useCache, DESCRIPTOR,
      [&]() { return std::make_unique<misc::QuadWeightTable>(sparseGrid); });
}

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
