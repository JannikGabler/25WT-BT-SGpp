// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_weighted_sum_tools.hpp
 * @brief Weighted sum over the grid points of a deltas sparse grid, each grid point weighted with
 * its global weight: the per-delta-tensor-grid kernel shared by the quadrature and the global
 * interpolation on a @c DeltasSparseGrid.
 *
 * <b>Global weights.</b> Every grid point @f$x@f$ of the sparse grid lies in exactly one delta
 * tensor grid @f$\Delta_{\vec{m}}@f$ and in exactly the tensor grids @f$\vec{\ell} \geq \vec{m}@f$.
 * Its global weight is
 * @f[ W(x) = \sum_{\vec{\ell} \geq \vec{m}} c_{\vec{\ell}} \prod_{k=0}^{d-1} w_{k,\ell_k}(i_k), @f]
 * where @f$w_{k,\ell}(i)@f$ is the 1D weight of the @f$i@f$-th delta node of level @f$m_k@f$ at
 * level @f$\ell@f$ (dimension @f$k@f$): for the quadrature its weight in the 1D quadrature rule of
 * level @f$\ell@f$ (@c tools::dsg_quadrature::comp1DWeights), for the global interpolation the
 * value of its basis function of the 1D interpolant of level @f$\ell@f$ at the query point
 * (@c tools::dsg_global_interpolation::compBasisValues). The sum is factorized along a trie over
 * the levels (@c misc::CTCoeffTrie):
 * @f$W = \sum_{\ell_0} w_{0,\ell_0}(i_0) \sum_{\ell_1} w_{1,\ell_1}(i_1) \cdots
 * \sum_{\ell_{d-1}} w_{d-1,\ell_{d-1}}(i_{d-1})\, c_{\vec{\ell}}@f$.
 * Iterating over the grid points of @f$\Delta_{\vec{m}}@f$ with dimension 0 fastest, the partial
 * sums of depth @f$k@f$ only change with @f$i_{k+1}@f$, so they are updated only then.
 *
 * <b>Projection.</b> In a dimension @f$k@f$ in which @f$\Delta_{\vec{m}}@f$ has a single node
 * (e.g. level 0 for a boundary level offset @f$\geq 1@f$, i.e. most dimensions of most delta
 * tensor grids at high @f$d@f$), the factor @f$w_{k,\ell_k}(0)@f$ does not change from grid point
 * to grid point. These factors are multiplied into the coefficients, and the tensor grids are
 * merged by their levels in the other dimensions @f$P@f$ (the projected dimensions; dimension 0
 * if there are none): @f$W = \sum_{\vec{\ell}_P} C(\vec{\ell}_P) \prod_{k \in P}
 * w_{k,\ell_k}(i_k)@f$ with the projected coefficients
 * @f$C(\vec{\ell}_P) = \sum c_{\vec{\ell}} \prod_{k \notin P} w_{k,\ell_k}(0)@f$. The trie
 * over @f$\vec{\ell}_P@f$ (the projected sub-trie) has only @f$|P|@f$ depths and far fewer nodes,
 * and the grid points are iterated over the dimensions in @f$P@f$ only (rows along the first one).
 *
 * <b>1D weights in delta order.</b> For nested nodes, the regular nodes of level @f$\ell@f$ are the
 * delta nodes of the levels @f$0, \ldots, \ell@f$. The 1D weights of level @f$\ell@f$
 * (@c misc::GenericDSGOperatorContext::weights1D) are listed in this order (delta level by delta
 * level), which makes @f$w_{k,\ell}(\cdot)@f$ a contiguous slice for every delta level, so no node
 * index has to be mapped in the hot loop.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/projected_trie_node.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_weighted_sum {

/**
 * @brief Computes the weighted sum of the source function over the grid points of one delta
 * tensor grid, each weighted with its global weight (with the 1D weights of @p context).
 *
 * @param context     Read-only data of the operator call.
 * @param sourceFunc  Source function.
 * @param deltaTGData Delta tensor grid.
 * @param scratch     Per-thread scratch space; reused across calls.
 * @return Sum of the global weight times the function value over the grid points.
 */
double processDeltaTensorGrid(const misc::GenericDSGOperatorContext& context,
                              const SourceFunc& sourceFunc, const TensorGridCTData& deltaTGData,
                              misc::DeltaWeightedSumScratch& scratch);

/**
 * @brief @ref processDeltaTensorGrid for a vector-valued source function: the same iteration with
 * m values per grid point (the global weight of a grid point is computed once for all outputs).
 *
 * @param context     Read-only data of the operator call.
 * @param sourceFunc  Vector-valued source function (m outputs).
 * @param deltaTGData Delta tensor grid.
 * @param scratch     Per-thread scratch space, constructed with m outputs; reused across calls.
 * @param out         Output: the m sums of the delta tensor grid (size m).
 */
void processDeltaTensorGrid(const misc::GenericDSGOperatorContext& context,
                            const VectorSourceFunc& sourceFunc, const TensorGridCTData& deltaTGData,
                            misc::DeltaWeightedSumScratch& scratch, std::span<double> out);

/**
 * @brief Fills @c scratch.trieNodes with the projected sub-trie of the tensor grids
 * @f$\vec{\ell} \geq@f$ @p deltaTGMI of @c context.coeffTrie (see the file description):
 * sets @c scratch.projDims to the dimensions in which the delta tensor grid of level @p deltaTGMI
 * has more than one node (dimension 0 if there are none); the nodes of projected depth @c t are
 * the distinct prefixes of the levels of these tensor grids in @c projDims[0], ..., projDims[t],
 * pruned to the nodes with a leaf below them; sets the weight pointers and, for the leaves, the
 * projected coefficients (leaves whose projected coefficient is 0 are left out). Leaves
 * @c scratch.trieNodes empty if there is no tensor grid @f$\vec{\ell} \geq@f$ @p deltaTGMI.
 *
 * The projected coefficients are sums of many products that cancel; they are accumulated in
 * double-double arithmetic (@c misc::DoubleDouble), so that they are correct to about one rounding.
 */
void compSubTrie(const misc::GenericDSGOperatorContext& context, const LvlMI& deltaTGMI,
                 misc::DeltaWeightedSumScratch& scratch);

/**
 * @brief Prepares @p scratch for iterating over the grid points of @p deltaTGData: computes the
 * sub-trie (@ref compSubTrie), maps the nodes onto the domain and sets the current grid point to
 * the first one.
 */
void prepareDeltaTensorGrid(const misc::GenericDSGOperatorContext& context,
                            const TensorGridCTData& deltaTGData,
                            misc::DeltaWeightedSumScratch& scratch);

/**
 * @brief Recomputes the partial sums @c coeffSum of the sub-trie nodes of the (projected) depths
 * @c 0, ..., maxDepth-1, i.e. the ones that depend on the node indices of the projected depths
 * @c 1, ..., maxDepth.
 *
 * @note Defined in the header for inlining.
 */
inline void updateCoeffSums(misc::DeltaWeightedSumScratch& scratch, const size_t maxDepth) {
  for (size_t depth = maxDepth; depth >= 1; depth--) {
    const GPCntType nodeIdx = scratch.gpMI[depth];
    const misc::ProjectedTrieNode* const children = scratch.trieNodes[depth].data();
    size_t childIdx = 0;

    for (misc::ProjectedTrieNode& parent : scratch.trieNodes[depth - 1]) {
      double sum = 0.0;

      for (; childIdx < parent.childEnd; childIdx++) {
        sum += children[childIdx].weights[nodeIdx] * children[childIdx].coeffSum;
      }

      parent.coeffSum = sum;
    }
  }
}

/**
 * @brief Computes the global weights of the current row of grid points (all @p nGP0 node indices
 * of dimension @c scratch.projDims[0]) into @c scratch.rowWeights.
 */
void compRowWeights(misc::DeltaWeightedSumScratch& scratch, GPCntType nGP0);

/**
 * @brief Calls @p func(gp, globalWeight) for every grid point of the delta tensor grid
 * @p deltaTGData, where @c gp is the grid point in domain coordinates (a @c base::DataVector that
 * is only valid during the call) and @c globalWeight its global weight (with the 1D weights of
 * @p context).
 *
 * The grid points are visited in the order of the tensor grid (dimension 0 fastest).
 *
 * @note Aligned to 64 bytes, the fetch window of the op cache, in order to reduce op-cache misses.
 * Not inlined, since inlined into its caller, the loop loses the alignment.
 */
template <typename Func>
[[gnu::noinline, gnu::aligned(64)]] void forEachGPWithGlobalWeight(
    const misc::GenericDSGOperatorContext& context, const TensorGridCTData& deltaTGData,
    misc::DeltaWeightedSumScratch& scratch, Func&& func) {
  if (deltaTGData.tensorGrid.nGP() == 0) {
    return;
  }

  prepareDeltaTensorGrid(context, deltaTGData, scratch);

  // Only the projected dimensions are iterated over: the other ones have a single node, whose
  // coordinate prepareDeltaTensorGrid sets
  const size_t nProjDepths = scratch.projDims.size();
  const size_t* const projDims = scratch.projDims.data();
  const GPCntType* const gpCntOfProjDepth = scratch.gpCntOfProjDepth.data();
  const double* const* const nodesOfProjDepth = scratch.nodesOfProjDepth.data();
  const double* const rowWeights = scratch.rowWeights.data();
  GPCntType* const gpMI = scratch.gpMI.data();
  double* const gp = scratch.gp.data();
  const base::DataVector& gpVec = scratch.gp;
  const size_t rowDim = projDims[0];
  const GPCntType nGPPerRow = gpCntOfProjDepth[0];
  const double* const rowNodes = nodesOfProjDepth[0];

  updateCoeffSums(scratch, nProjDepths - 1);

  while (true) {
    compRowWeights(scratch, nGPPerRow);

    for (GPCntType idx = 0; idx < nGPPerRow; idx++) {
      gp[rowDim] = rowNodes[idx];
      func(gpVec, rowWeights[idx]);
    }

    // Advances to the next row: increments the node index of the first projected depth >= 1 that
    // does not wrap around
    size_t depth = 1;

    for (; depth < nProjDepths; depth++) {
      double& coord = gp[projDims[depth]];

      if (++gpMI[depth] < gpCntOfProjDepth[depth]) {
        coord = nodesOfProjDepth[depth][gpMI[depth]];
        break;
      }

      gpMI[depth] = 0;
      coord = nodesOfProjDepth[depth][0];
    }

    if (depth == nProjDepths) {  // Wrapped around in every projected depth: all rows are done
      return;
    }

    updateCoeffSums(scratch, depth);
  }
}

}  // namespace dsg_weighted_sum
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
