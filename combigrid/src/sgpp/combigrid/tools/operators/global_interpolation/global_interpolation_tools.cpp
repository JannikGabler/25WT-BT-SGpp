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
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
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

void combineTensorGrids(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                        const SparseGrid& sparseGrid, const std::span<double> result) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const size_t nOutputs = result.size();

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    GlobalInterpScratch scratch(sparseGrid.nDim(), sparseGrid.getMaxTGSumOverGPCntsPerDim(),
                                nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

#pragma omp for schedule(guided) nowait
    for (size_t tgIdx = 0; tgIdx < sparseGrid.nTG(); tgIdx++) {
      const TensorGridCTDataView tgData = sparseGrid.getDomainTensorGrid(tgIdx);
      processTensorGrid(sourceFunc, normPoint, tgData.tensorGrid,
                        sparseGrid.getUnitTensorGrid(tgIdx).tensorGrid, genInstr, scratch,
                        tgResult);
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

void prepareTensorGrid(const base::DataVector& normPoint, const TensorGridView& tg,
                       const TensorGridView& unitTG, const SGGenInstr& genInstr,
                       GlobalInterpScratch& scratch) {
  fillBasisValues(normPoint, unitTG, genInstr, scratch);
  weighted_sum::setUpIterDims(tg, scratch.rows);
}

void fillBasisValues(const base::DataVector& normPoint, const TensorGridView& unitTG,
                     const SGGenInstr& genInstr, GlobalInterpScratch& scratch) {
  assert(normPoint.size() >= unitTG.nDim());

  base::DataVector& basisValues = scratch.rows.weights1D;
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
    offset += nNodes;
  }
}

}  // namespace global_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
