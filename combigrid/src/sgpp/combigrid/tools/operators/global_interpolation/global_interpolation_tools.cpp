// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/global_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::GlobalInterpScratch;
using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace global_interpolation {

namespace {

/**
 * Calls @p process(tgIdx, scratch) for every tensor grid of @p sparseGrid in parallel, with a
 * scratch per thread for m = @p nOutputs (0 for a scalar source function).
 */
template <typename Process>
void forEachTensorGrid(const SparseGrid& sparseGrid, const size_t nOutputs, Process&& process) {
  const size_t nTG = sparseGrid.nTG();

#pragma omp parallel
  {
    GlobalInterpScratch scratch(sparseGrid.nDim(), sparseGrid.getMaxTGSumOverGPCntsPerDim(),
                                nOutputs);

#pragma omp for schedule(guided) nowait
    for (size_t tgIdx = 0; tgIdx < nTG; tgIdx++) {
      process(tgIdx, scratch);
    }
  }
}

}  // namespace

std::vector<double> estimateTensorGrids(const SourceFunc& sourceFunc,
                                        const base::DataVector& normPoint,
                                        const SparseGrid& sparseGrid) {
  std::vector<double> values(sparseGrid.nTG());
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  forEachTensorGrid(sparseGrid, 0, [&](const size_t tgIdx, GlobalInterpScratch& scratch) {
    values[tgIdx] =
        processTensorGrid(sourceFunc, normPoint, sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid,
                          sparseGrid.getUnitTensorGrid(tgIdx).tensorGrid, genInstr, scratch);
  });
  return values;
}

misc::FlatNestedVec<double, 2> estimateTensorGrids(const VectorSourceFunc& sourceFunc,
                                                   const base::DataVector& normPoint,
                                                   const SparseGrid& sparseGrid) {
  const size_t nOutputs = sourceFunc.nOutputs();
  misc::FlatNestedVec<double, 2> values(sparseGrid.nTG(), nOutputs);
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  forEachTensorGrid(sparseGrid, nOutputs, [&](const size_t tgIdx, GlobalInterpScratch& scratch) {
    processTensorGrid(sourceFunc, normPoint, sparseGrid.getDomainTensorGrid(tgIdx).tensorGrid,
                      sparseGrid.getUnitTensorGrid(tgIdx).tensorGrid, genInstr, scratch,
                      values[tgIdx]);
  });
  return values;
}

double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& normPoint,
                         const TensorGridView& tg, const TensorGridView& unitTG,
                         const SGGenInstr& genInstr, GlobalInterpScratch& scratch) {
  if (tg.nDim() == 0 || tg.nGP() == 0) {
    return 0;
  }

  prepareTensorGrid(normPoint, tg, unitTG, genInstr, scratch);

  return weighted_sum::sumOverRows(misc::ScalarValues(sourceFunc), scratch.rows);
}

void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                       const TensorGridView& tg, const TensorGridView& unitTG,
                       const SGGenInstr& genInstr, GlobalInterpScratch& scratch,
                       const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.rows.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), WeightedSumScratch::N_ACCS));

  if (tg.nDim() == 0 || tg.nGP() == 0) {
    std::ranges::fill(out, 0.0);
    return;
  }

  prepareTensorGrid(normPoint, tg, unitTG, genInstr, scratch);

  const misc::VectorValues values(sourceFunc, scratch.rows.valueBuffer);
  values.store(weighted_sum::sumOverRows(values, scratch.rows), out.data());
}

double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& normPoint,
                         const TensorGridView& tg, const TensorGridView& unitTG,
                         const SGGenInstr& genInstr) {
  GlobalInterpScratch scratch(tg.nDim(), tg.sumOfGPCntsPerDim());
  return processTensorGrid(sourceFunc, normPoint, tg, unitTG, genInstr, scratch);
}

void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                       const TensorGridView& tg, const TensorGridView& unitTG,
                       const SGGenInstr& genInstr, const std::span<double> out) {
  GlobalInterpScratch scratch(tg.nDim(), tg.sumOfGPCntsPerDim(), sourceFunc.nOutputs());
  processTensorGrid(sourceFunc, normPoint, tg, unitTG, genInstr, scratch, out);
}

void prepareTensorGrid(const base::DataVector& normPoint, const TensorGridView& tg,
                       const TensorGridView& unitTG, const SGGenInstr& genInstr,
                       GlobalInterpScratch& scratch) {
  fillBasisValues(normPoint, unitTG, genInstr, scratch);
  weighted_sum::setUpIterDims(tg, scratch.rows);
}

void fillBasisValues(const base::DataVector& normPoint, const TensorGridView& unitTG,
                     const SGGenInstr& genInstr, GlobalInterpScratch& scratch) {
  assert(normPoint.size() >= unitTG.nDim());

  base::DataVector& basisValues = scratch.basisValues;
  basisValues.resize(unitTG.sumOfGPCntsPerDim());
  size_t offset = 0;

  for (size_t dim = 0; dim < unitTG.nDim(); dim++) {
    const GPCntType nNodes = unitTG.nGPInDim(dim);
    const double* const nodes = unitTG.nodesInDim(dim);
    const InterpolationMethod* const method =
        genInstr.getNodeGenFuncForDim(dim)->getInterpolationMethod(nNodes);

    // The interpolation methods take the nodes of one dimension as a vector of their own
    scratch.nodes1D.assign(nodes, nodes + nNodes);
    method->basisValues(normPoint[dim], scratch.nodes1D,
                        std::span<double>(basisValues.data() + offset, nNodes));
    scratch.rows.weightsOfDim[dim] = basisValues.data() + offset;
    offset += nNodes;
  }
}

}  // namespace global_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
