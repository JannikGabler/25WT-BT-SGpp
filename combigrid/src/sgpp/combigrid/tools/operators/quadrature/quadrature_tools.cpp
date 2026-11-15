// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <cassert>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;
using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

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
 * Sets @p result to the sum over the tensor grids of the coefficient times the vector-valued
 * quadrature estimate of the tensor grid (unit hypercube), in parallel: per thread first, then
 * added to @p result, as the reduction of the scalar operator does output by output.
 */
void combineTensorGrids(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                        const std::span<double> result) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nOutputs = result.size();

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim(), nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);
    context.weightTable.computeAllRows();  // As in the scalar operator

    // Dynamic scheduling as in the scalar operator
#pragma omp for schedule(dynamic) nowait
    for (size_t tgIdx = 0; tgIdx < sparseGrid.nTG(); tgIdx++) {
      const TensorGridCTDataView tgData = sparseGrid.getDomainTensorGrid(tgIdx);
      processTensorGrid(context, sourceFunc, tgData.tensorGrid, scratch, tgResult);
      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgData.coefficient * tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }
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

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
