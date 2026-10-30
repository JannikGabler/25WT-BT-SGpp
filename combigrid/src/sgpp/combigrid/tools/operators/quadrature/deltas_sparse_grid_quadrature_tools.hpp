// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document (update)
 * @brief Implementation details of the delta quadrature operator.
 *
 * <b>Global weights.</b> Every grid point @f$x@f$ of the sparse grid lies in exactly one delta
 * tensor grid @f$\Delta_{\vec{m}}@f$ and in exactly the tensor grids @f$\vec{\ell} \geq \vec{m}@f$.
 * Its global weight is
 * @f[ W(x) = \sum_{\vec{\ell} \geq \vec{m}} c_{\vec{\ell}} \prod_{k=0}^{d-1} w_{k,\ell_k}(i_k), @f]
 * where @f$w_{k,\ell}(i)@f$ is the weight of the @f$i@f$-th delta node of level @f$m_k@f$ in the 1D
 * rule of level @f$\ell@f$ (dimension @f$k@f$). The sum is factorized along a trie over the
 * levels (@c misc::CTCoeffTrie):
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
 * delta nodes of the levels @f$0, \ldots, \ell@f$. Listing the 1D weights of level @f$\ell@f$ in
 * this order (delta level by delta level, see @ref comp1DWeights) makes @f$w_{k,\ell}(\cdot)@f$ a
 * contiguous slice for every delta level, so no node index has to be mapped in the hot loop.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

/**
 * @brief Returns, per dimension, the function that maps the index of a delta node to the index
 * of the same node in the (inner) nodes of a higher or equal level.
 */
std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Maps the index of a node in a delta tensor grid to the index of the same node in a
 * regular tensor grid of a higher or equal level (in one dimension), taking the boundary nodes
 * into account.
 *
 * @param nNodesInDeltaTG   Number of delta nodes (including boundary nodes) of @p deltaTGLvl.
 * @param nNodesInTargetTG  Number of regular nodes (including boundary nodes) of @p targetTGLvl.
 * @param deltaTGLvl        Level of the delta tensor grid.
 * @param targetTGLvl       Level of the regular tensor grid, at least @p deltaTGLvl.
 * @param gpIdxInDeltaTG    Index of the delta node.
 * @param nodeIdxLvlMapFunc Index map of the inner nodes (see @ref compNodeIdxLvlMapFuncs).
 * @param boundaryLvlOffset Boundary level offset of the sparse grid.
 * @return Index of the node in the regular nodes of @p targetTGLvl.
 */
GPCntType mapNodeIdx(GPCntType nNodesInDeltaTG, GPCntType nNodesInTargetTG, LvlType deltaTGLvl,
                     LvlType targetTGLvl, GPCntType gpIdxInDeltaTG,
                     NodeIdxLvlMapFunc nodeIdxLvlMapFunc, LvlType boundaryLvlOffset);

/**
 * @brief Computes the 1D quadrature weights of every level of every dimension in delta order.
 *
 * @c weights1D[dim][lvl] holds the weights of the regular 1D rule of level @c lvl (including the
 * boundary nodes if @c lvl is at least the boundary level offset), for every level up to the
 * maximum level of @c dim in @p deltasSparseGrid. They are not sorted by node position but by
 * delta level: first the weights of the delta nodes of level 0, then the ones of level 1, ...,
 * each block in the order of the nodes of the delta tensor grids. Hence the weights of the delta
 * nodes of level @c m start at index @c weights1D[dim][m-1].size() (at 0 for @c m = 0).
 */
misc::FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Computes the quadrature (on the unit domain) of the grid points of one delta tensor
 * grid, each weighted with its global weight.
 */
double processDeltaTensorGrid(const misc::DeltaQuadratureContext& context,
                              const TensorGridCTData& deltaTGData,
                              misc::DeltaQuadratureScratch& scratch);

/**
 * @brief Fills @c scratch.trieNodes with the projected sub-trie of the tensor grids
 * @f$\vec{\ell} \geq@f$ @p deltaTGMI of @c context.coeffTrie (see the namespace description):
 * sets @c scratch.projDims to the dimensions in which the delta tensor grid of level @p deltaTGMI
 * has more than one node (dimension 0 if there are none); the nodes of projected depth @c t are
 * the distinct prefixes of the levels of these tensor grids in @c projDims[0], ..., projDims[t],
 * pruned to the nodes with a leaf below them; sets the weight pointers and, for the leaves, the
 * projected coefficients (leaves whose projected coefficient is 0 are left out). Leaves
 * @c scratch.trieNodes empty if there is no tensor grid @f$\vec{\ell} \geq@f$ @p deltaTGMI.
 *
 * The projected coefficients are sums of many products that cancel; they are accumulated in
 * double-double arithmetic, so that they are correct to about one rounding.
 */
void compSubTrie(const misc::DeltaQuadratureContext& context, const LvlMI& deltaTGMI,
                 misc::DeltaQuadratureScratch& scratch);

/**
 * @brief Prepares @p scratch for iterating over the grid points of @p deltaTGData: computes the
 * sub-trie (@ref compSubTrie), maps the nodes onto the domain and sets the current grid point to
 * the first one.
 */
void prepareDeltaTensorGrid(const misc::DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGData,
                            misc::DeltaQuadratureScratch& scratch);

/**
 * @brief Recomputes the partial sums @c coeffSum of the sub-trie nodes of the (projected) depths
 * @c 0, ..., maxDepth-1, i.e. the ones that depend on the node indices of the projected depths
 * @c 1, ..., maxDepth.
 */
void updateCoeffSums(misc::DeltaQuadratureScratch& scratch, size_t maxDepth);

/**
 * @brief Computes the global weights of the current row of grid points (all @p nGP0 node indices
 * of dimension @c scratch.projDims[0]) into @c scratch.rowWeights.
 */
void compRowWeights(misc::DeltaQuadratureScratch& scratch, GPCntType nGP0);

/**
 * @brief Calls @p func(gp, globalWeight) for every grid point of the delta tensor grid
 * @p deltaTGData, where @c gp is the grid point in domain coordinates (a @c base::DataVector that
 * is only valid during the call) and @c globalWeight its global quadrature weight (on the unit
 * domain).
 *
 * The grid points are visited in the order of the tensor grid (dimension 0 fastest).
 *
 * @note Aligned to 64 bytes, the fetch window of the op cache, in order to reduce op-cache misses
 * Unaligned, moving unrelated code within the module changed the deltas sparse grid quadrature by
 * up to 4 % at identical instructions (+18 % op-cache misses). Not inlined, since inlined into its
 * caller, the loop loses the alignment.
 */
template <typename Func>
[[gnu::noinline, gnu::aligned(64)]] void forEachGPWithGlobalWeight(
    const misc::DeltaQuadratureContext& context, const TensorGridCTData& deltaTGData,
    misc::DeltaQuadratureScratch& scratch, Func&& func) {
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

}  // namespace dsg_quadrature
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp