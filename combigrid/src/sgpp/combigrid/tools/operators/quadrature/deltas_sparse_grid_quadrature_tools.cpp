// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/deltas_sparse_grid_weighted_sum_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::DeltaWeightedSumScratch;
using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::tools::deltas_sparse_grid_generation::compNodeIdxLvlMapFuncs;
using sgpp::combigrid::tools::deltas_sparse_grid_generation::mapDeltaNodeIdxToRegular;
using sgpp::combigrid::tools::dsg_weighted_sum::processDeltaTensorGrid;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

// TODO: Functionaly decompose this function if feasible regarding runtime (?)
FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();
  const std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs =
      compNodeIdxLvlMapFuncs(deltasSparseGrid);

  // Maximum level per dimension
  LvlMI maxLvls(nDim, LvlType{0});

  for (const TensorGridCTData& tgData : deltasSparseGrid) {
    for (size_t dim = 0; dim < nDim; dim++) {
      maxLvls[dim] = std::max(maxLvls[dim], tgData.mi[dim]);
    }
  }

  FlatNestedVec<double, 3> weights;
  weights.reserveSubVecs(1, nDim);
  base::DataVector sortedWeights;  // Weights of one level, sorted by node position

  for (size_t dim = 0; dim < nDim; dim++) {
    const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);

    weights.appendSubVec(1);  // weights[dim]

    for (LvlType lvl = 0; lvl <= maxLvls[dim]; lvl++) {
      const GPCntType nNodes = lvl2GPCntFunc(lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);

      sortedWeights.resize(nNodes);
      nodeGenFunc->getQuadRule(nNodes)->genWeightsInplace(sortedWeights);

      weights.appendSubVec(2, nNodes);  // weights[dim][lvl]
      // The sub-vector just appended consists of the last nNodes elements.
      const std::span<double> weightsOfLvl = weights.flat().last(nNodes);
      size_t writeIdx = 0;

      for (LvlType deltaLvl = 0; deltaLvl <= lvl; deltaLvl++) {
        const GPCntType nDeltaNodes =
            tools::getDeltaNodeCnt(deltaLvl, lvl2GPCntFunc, deltaLvl == boundaryLvlOffset);

        for (GPCntType deltaNodeIdx = 0; deltaNodeIdx < nDeltaNodes; deltaNodeIdx++) {
          const GPCntType nodeIdx =
              mapDeltaNodeIdxToRegular(nDeltaNodes, nNodes, deltaLvl, lvl, deltaNodeIdx,
                                       nodeIdxLvlMapFuncs[dim], boundaryLvlOffset);

          assert(nodeIdx < nNodes);
          weightsOfLvl[writeIdx++] = sortedWeights[nodeIdx];
        }
      }

      assert(writeIdx == nNodes);
    }
  }

  return weights;
}

/**
 * Per thread first, then added to @p result, as the reduction of the scalar operator does output
 * by output.
 */
void combineDeltaTensorGrids(const GenericDSGOperatorContext& context,
                             const VectorSourceFunc& sourceFunc, const std::span<double> result) {
  const DeltasSparseGrid& deltasSparseGrid = context.deltasSparseGrid;
  const size_t nOutputs = result.size();

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    DeltaWeightedSumScratch scratch(deltasSparseGrid.nDim(),
                                    deltasSparseGrid.getMaxTGSumOverGPCntsPerDim(), nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

    // Dynamic scheduling as in the scalar operator (delta tensor grids differ in size by orders
    // of magnitude)
#pragma omp for schedule(dynamic) nowait
    for (const TensorGridCTData& tgData : deltasSparseGrid) {
      processDeltaTensorGrid(context, sourceFunc, tgData, scratch, tgResult);
      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }
}

}  // namespace dsg_quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
