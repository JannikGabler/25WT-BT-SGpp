// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <numeric>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/deltas_sparse_grid_global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::DeltaQuadratureScratch;
using sgpp::combigrid::misc::DSGGlobalInterpIndex;
using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::tools::dsg_quadrature::processDeltaTensorGrid;

namespace sgpp {
namespace combigrid {
namespace tools {
namespace dsg_global_interpolation {

namespace {

/**
 * The passes of a call: fills the basis values of the query point into @p weights (all rows),
 * then calls process(tgIdx, scratch) for every delta tensor grid of @p deltasSparseGrid, with a
 * scratch per thread (m = @p nOutputs, 0 for a scalar source function). In parallel, both passes
 * run in one parallel region: the rows largest first (generic rules cost O(n^2) per row), then,
 * after the barrier of the first loop, the delta tensor grids (dynamic scheduling as in the deltas
 * sparse grid quadrature: they differ in size by orders of magnitude).
 */
template <typename Process>
void runPasses(const DSGGlobalInterpIndex& index, const DeltasSparseGrid& deltasSparseGrid,
               const base::DataVector& normPoint, FlatNestedVec<double, 3>& weights,
               const size_t nOutputs, const bool parallel, Process&& process) {
  const size_t nDim = deltasSparseGrid.nDim();
  const size_t maxSumOfGPCnts = deltasSparseGrid.getMaxTGSumOverGPCntsPerDim();
  const size_t nTG = deltasSparseGrid.nTG();
  const std::span<const size_t> rowsBySize = index.rowsBySize();
  const std::span<const size_t> tgsBySize = index.tgsBySize();

  if (!parallel) {
    std::vector<double> regular(index.maxRowSize());
    for (const size_t row : rowsBySize) {
      fillBasisValuesOfRow(index, normPoint, row, regular, weights);
    }
    DeltaQuadratureScratch scratch(nDim, maxSumOfGPCnts, nOutputs);
    for (size_t tgIdx = 0; tgIdx < nTG; tgIdx++) {
      process(tgIdx, scratch);
    }
    return;
  }

#pragma omp parallel
  {
    std::vector<double> regular(index.maxRowSize());

#pragma omp for schedule(dynamic)
    for (size_t k = 0; k < rowsBySize.size(); k++) {
      fillBasisValuesOfRow(index, normPoint, rowsBySize[k], regular, weights);
    }

    DeltaQuadratureScratch scratch(nDim, maxSumOfGPCnts, nOutputs);

#pragma omp for schedule(dynamic)
    for (size_t k = 0; k < nTG; k++) {
      process(tgsBySize[k], scratch);
    }
  }
}

}  // namespace

const DSGGlobalInterpIndex& getIndex(const DeltasSparseGrid& deltasSparseGrid) {
  return deltasSparseGrid.getOperatorData().get<DSGGlobalInterpIndex>(
      [&]() { return std::make_unique<DSGGlobalInterpIndex>(deltasSparseGrid); });
}

bool runInParallel(const DSGGlobalInterpIndex& index) {
  return index.nGridPoints() >= constants::global_interpolation::DSG_PARALLEL_MIN_GRID_POINTS;
}

void fillBasisValuesOfRow(const DSGGlobalInterpIndex& index, const base::DataVector& normPoint,
                          const size_t row, const std::span<double> regular,
                          FlatNestedVec<double, 3>& weights) {
  const std::vector<double>& nodes = index.nodes(row);
  const std::span<double> rowValues = regular.first(nodes.size());
  index.method(row).basisValues(normPoint[index.rowDim(row)], nodes, rowValues);

  const std::span<const GPCntType> deltaOrder = index.deltaOrder(row);
  double* const dst = weights.data() + index.weightStart(row);
  for (size_t k = 0; k < deltaOrder.size(); k++) {
    dst[k] = rowValues[deltaOrder[k]];
  }
}

FlatNestedVec<double, 3> compBasisValues(const DSGGlobalInterpIndex& index,
                                         const base::DataVector& normPoint) {
  assert(normPoint.size() == index.nDim());

  FlatNestedVec<double, 3> weights = index.weightLayout();
  std::vector<double> regular(index.maxRowSize());
  for (size_t row = 0; row < index.nRows(); row++) {
    fillBasisValuesOfRow(index, normPoint, row, regular, weights);
  }
  return weights;
}

double evaluateInterpolant(const DSGGlobalInterpIndex& index,
                           const DeltasSparseGrid& deltasSparseGrid,
                           const base::DataVector& normPoint, const SourceFunc& sourceFunc,
                           const bool parallel) {
  assert(normPoint.size() == index.nDim());

  FlatNestedVec<double, 3> weights = index.weightLayout();
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .domain = deltasSparseGrid.getGenInstr().getDomain(),
                                          .weights1D = weights,
                                          .coeffTrie = index.coeffTrie()};
  std::vector<double> tgSums(deltasSparseGrid.nTG());

  runPasses(index, deltasSparseGrid, normPoint, weights, 0, parallel,
            [&](const size_t tgIdx, DeltaQuadratureScratch& scratch) {
              tgSums[tgIdx] = processDeltaTensorGrid(
                  context, sourceFunc, deltasSparseGrid.getTensorGrid(tgIdx), scratch);
            });

  return std::accumulate(tgSums.begin(), tgSums.end(), 0.0);
}

void evaluateInterpolant(const DSGGlobalInterpIndex& index,
                         const DeltasSparseGrid& deltasSparseGrid,
                         const base::DataVector& normPoint, const VectorSourceFunc& sourceFunc,
                         const bool parallel, const std::span<double> result) {
  assert(normPoint.size() == index.nDim());
  assert(result.size() == sourceFunc.nOutputs());

  const size_t nOutputs = result.size();
  FlatNestedVec<double, 3> weights = index.weightLayout();
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .domain = deltasSparseGrid.getGenInstr().getDomain(),
                                          .weights1D = weights,
                                          .coeffTrie = index.coeffTrie()};
  std::vector<double> tgSums(deltasSparseGrid.nTG() * nOutputs);

  runPasses(index, deltasSparseGrid, normPoint, weights, nOutputs, parallel,
            [&](const size_t tgIdx, DeltaQuadratureScratch& scratch) {
              processDeltaTensorGrid(context, sourceFunc, deltasSparseGrid.getTensorGrid(tgIdx),
                                     scratch,
                                     std::span<double>(tgSums.data() + tgIdx * nOutputs, nOutputs));
            });

  std::ranges::fill(result, 0.0);
  for (size_t tgIdx = 0; tgIdx < deltasSparseGrid.nTG(); tgIdx++) {
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += tgSums[tgIdx * nOutputs + k];
    }
  }
}

}  // namespace dsg_global_interpolation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
