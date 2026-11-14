// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/projected_trie_node.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
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
                                             const TensorGridCTDataView& deltaTGData,
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
                              const SourceFunc& sourceFunc, const TensorGridCTDataView& deltaTGData,
                              DeltaWeightedSumScratch& scratch) {
  return sumWithGlobalWeights(context, misc::ScalarValues(sourceFunc), deltaTGData, scratch);
}

void processDeltaTensorGrid(const GenericDSGOperatorContext& context,
                            const VectorSourceFunc& sourceFunc,
                            const TensorGridCTDataView& deltaTGData,
                            DeltaWeightedSumScratch& scratch, const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), DeltaWeightedSumScratch::N_ACCS));

  const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumWithGlobalWeights(context, values, deltaTGData, scratch), out.data());
}

void compSubTrie(const GenericDSGOperatorContext& context, const LvlMIView deltaTGMI,
                 DeltaWeightedSumScratch& scratch) {
  assert(deltaTGMI.size() == context.coeffTrie.nDim());

  tools::ProjectedSubTrieBuilder(context, deltaTGMI, scratch).build();
}

void prepareDeltaTensorGrid(const GenericDSGOperatorContext& context,
                            const TensorGridCTDataView& deltaTGData,
                            DeltaWeightedSumScratch& scratch) {
  const TensorGridView deltaTG = deltaTGData.tensorGrid;
  const size_t nDim = deltaTG.nDim();

  assert(nDim >= 1);

  compSubTrie(context, deltaTGData.mi, scratch);

  // The nodes are read from the node table, which stores them on the domain
  size_t projDepth = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double* const nodes = deltaTG.nodesInDim(dim);
    scratch.gp[dim] = nodes[0];

    if (projDepth < scratch.projDims.size() && scratch.projDims[projDepth] == dim) {
      scratch.nodesOfProjDepth[projDepth] = nodes;
      scratch.gpCntOfProjDepth[projDepth] = deltaTG.nGPInDim(dim);
      scratch.gpMI[projDepth] = 0;
      projDepth++;
    }
  }

  // The row loop reads the nodes of the row dimension from a copy in the scratch: read from the
  // node table was slower at the same instructions (lower IPC, also in the source function called
  // from the loop). Copying the nodes of all projected dimensions gained nothing more.
  const GPCntType nRowNodes = scratch.gpCntOfProjDepth[0];
  if (scratch.rowNodes.size() < nRowNodes) {
    scratch.rowNodes.resize(nRowNodes);
  }
  std::copy_n(scratch.nodesOfProjDepth[0], nRowNodes, scratch.rowNodes.data());
  scratch.nodesOfProjDepth[0] = scratch.rowNodes.data();
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
