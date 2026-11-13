// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/projected_trie_node.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/deltas_sparse_grid_weighted_sum_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/projected_sub_trie_builder.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

using sgpp::combigrid::misc::DeltaWeightedSumScratch;
using sgpp::combigrid::misc::GenericDSGOperatorContext;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_weighted_sum {

namespace {

/**
 * Sums the global weight times the source function over the grid points of @p deltaTGData and
 * returns the accumulator of the sums. Written once for both value types (misc::ScalarValues,
 * misc::VectorValues), which perform the same operations per output; the global weight of a grid
 * point is computed once for all outputs. @p values (a small view) is captured by value, so that
 * the row loop reaches the source function through the closure directly.
 */
template <typename ValueType>
typename ValueType::Acc sumWithGlobalWeights(const GenericDSGOperatorContext& context,
                                             const ValueType values,
                                             const TensorGridCTData& deltaTGData,
                                             DeltaWeightedSumScratch& scratch) {
  typename ValueType::Acc result = values.zeroAcc(DeltaWeightedSumScratch::TG_SUM_ACC);

  forEachGPWithGlobalWeight(
      context, deltaTGData, scratch,
      [&result, values](const base::DataVector& gp, const double globalWeight) {
        values.addScaled(result, globalWeight, values.evaluate(gp));
      });

  return result;
}

}  // namespace

double processDeltaTensorGrid(const GenericDSGOperatorContext& context,
                              const SourceFunc& sourceFunc, const TensorGridCTData& deltaTGData,
                              DeltaWeightedSumScratch& scratch) {
  return sumWithGlobalWeights(context, misc::ScalarValues(sourceFunc), deltaTGData, scratch);
}

void processDeltaTensorGrid(const GenericDSGOperatorContext& context,
                            const VectorSourceFunc& sourceFunc, const TensorGridCTData& deltaTGData,
                            DeltaWeightedSumScratch& scratch, const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), DeltaWeightedSumScratch::N_ACCS));

  const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumWithGlobalWeights(context, values, deltaTGData, scratch), out.data());
}

void compSubTrie(const GenericDSGOperatorContext& context, const LvlMI& deltaTGMI,
                 DeltaWeightedSumScratch& scratch) {
  assert(deltaTGMI.size() == context.coeffTrie.nDim());

  tools::ProjectedSubTrieBuilder(context, deltaTGMI, scratch).build();
}

void prepareDeltaTensorGrid(const GenericDSGOperatorContext& context,
                            const TensorGridCTData& deltaTGData, DeltaWeightedSumScratch& scratch) {
  const TensorGrid& deltaTG = deltaTGData.tensorGrid;
  const size_t nDim = deltaTG.nDim();
  const GPMI& nGPPerDim = deltaTG.getGPCntPerDim();
  const base::DataVector& nodesPerDim = deltaTG.getNodesPerDim();

  assert(nDim >= 1);
  assert(scratch.nodesPerDim.size() >= nodesPerDim.size());

  compSubTrie(context, deltaTGData.mi, scratch);

  // Maps the nodes onto the domain once, instead of every grid point (with tools::mapToDomain,
  // as the other operators)
  size_t offset = 0;
  size_t projDepth = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = context.domain[dim].first;
    const double width = context.domain[dim].second - context.domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;

    for (GPCntType nodeIdx = 0; nodeIdx < nGPPerDim[dim]; nodeIdx++) {
      nodes[nodeIdx] = tools::mapToDomain(nodesPerDim[offset + nodeIdx], lower, width);
    }

    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];

    if (projDepth < scratch.projDims.size() && scratch.projDims[projDepth] == dim) {
      scratch.nodesOfProjDepth[projDepth] = nodes;
      scratch.gpCntOfProjDepth[projDepth] = nGPPerDim[dim];
      scratch.gpMI[projDepth] = 0;
      projDepth++;
    }
  }
}

void compRowWeights(misc::DeltaWeightedSumScratch& scratch, const GPCntType nGP0) {
  double* const rowWeights = scratch.rowWeights.data();
  std::fill_n(rowWeights, nGP0, 0.0);

  for (const misc::ProjectedTrieNode& node : scratch.trieNodes[0]) {
    const double coeffSum = node.coeffSum;
    const double* const weights = node.weights;

    for (GPCntType idx0 = 0; idx0 < nGP0; idx0++) {
      rowWeights[idx0] += coeffSum * weights[idx0];
    }
  }
}

}  // namespace dsg_weighted_sum
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
