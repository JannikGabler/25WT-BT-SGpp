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
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/tools/operators/cross_call/cross_call_operator_data_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;
using sgpp::combigrid::misc::QuadWeightTable;
using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

std::vector<double> estimateTensorGrids(const QuadratureContext& context,
                                        const SourceFunc& sourceFunc) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nTG = sparseGrid.nTG();
  const std::span<const size_t> order = sparseGrid.getTGProcessingOrder();

  std::vector<double> estimates(nTG);

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim());
    context.weightTable.computeAllRows();  // Nowait operation: Threads can immediately continue

    // The tensor grids differ in size by orders of magnitude -> Dynamic scheduling, largest first
#pragma omp for schedule(dynamic) nowait
    for (size_t orderIdx = 0; orderIdx < nTG; orderIdx++) {
      const size_t tgIdx = order[orderIdx];
      estimates[tgIdx] =
          processTensorGrid(context.weightTable, sourceFunc,
                            sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid, scratch);
    }
  }

  return estimates;
}

misc::FlatNestedVec<double, 2> estimateTensorGrids(const QuadratureContext& context,
                                                   const VectorSourceFunc& sourceFunc) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nTG = sparseGrid.nTG();
  const size_t nOutputs = sourceFunc.nOutputs();
  const std::span<const size_t> order = sparseGrid.getTGProcessingOrder();

  misc::FlatNestedVec<double, 2> estimates(nTG, nOutputs);

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim(), nOutputs);
    context.weightTable.computeAllRows();  // Nowait operation: Threads can immediately continue

    // The tensor grids differ in size by orders of magnitude -> Dynamic scheduling, largest first
#pragma omp for schedule(dynamic) nowait
    for (size_t orderIdx = 0; orderIdx < nTG; orderIdx++) {
      const size_t tgIdx = order[orderIdx];
      processTensorGrid(context.weightTable, sourceFunc,
                        sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid, scratch,
                        estimates[tgIdx]);
    }
  }

  return estimates;
}

double processTensorGrid(const QuadWeightTable& weightTable, const SourceFunc& sourceFunc,
                         const TensorGridView tg, QuadratureScratch& scratch) {
  assert(tg.nDim() == weightTable.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  prepareTensorGrid(weightTable, tg, scratch);

  return weighted_sum::sumOverRows(misc::ScalarValues(sourceFunc), scratch.rows);
}

void processTensorGrid(const QuadWeightTable& weightTable, const VectorSourceFunc& sourceFunc,
                       const TensorGridView tg, QuadratureScratch& scratch,
                       const std::span<double> out) {
  assert(tg.nDim() == weightTable.nDim());
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.rows.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), WeightedSumScratch::N_ACCS));

  if (tg.nGP() == 0) {
    std::ranges::fill(out, 0.0);
    return;
  }

  prepareTensorGrid(weightTable, tg, scratch);

  const misc::VectorValues values(sourceFunc, scratch.rows.valueBuffer);
  values.store(weighted_sum::sumOverRows(values, scratch.rows), out.data());
}

/**
 * Prepares the row loop over @p tg (weighted_sum::sumOverRows): 1D quadrature weights, iterated
 * dimensions.
 */
void prepareTensorGrid(const QuadWeightTable& weightTable, const TensorGridView tg,
                       QuadratureScratch& scratch) {
  getWeights(weightTable, tg, scratch);
  weighted_sum::setUpIterDims(tg, scratch.rows);
}

void getWeights(const QuadWeightTable& weightTable, const TensorGridView tg,
                QuadratureScratch& scratch) {
  const LvlMIView lvls = tg.getLevels();
  const double** const weightsOfDim = scratch.rows.weightsOfDim.data();

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    weightsOfDim[dim] = weightTable.weights(dim, lvls[dim]);
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
