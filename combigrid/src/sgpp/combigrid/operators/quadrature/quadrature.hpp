/**
 * @file quadrature.hpp
 * @brief Tensor-product quadrature operator on a combination-technique
 * sparse grid.
 *
 * Each contained tensor grid contributes a tensor-product quadrature
 * (Clenshaw-Curtis, Simpson, trapezoidal, etc.) whose 1D rule is selected
 * per dimension by the node-generation function. The combination
 * coefficients yield the final sparse-grid integral approximation.
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_scratch.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Approximates @f$\int_\Omega f(\vec{x})\,\mathrm{d}\vec{x}@f$ on
 * the domain attached to @p sparseGrid using its combination-technique
 * structure.
 *
 * Internally, every tensor grid in @p sparseGrid is integrated with a
 * tensor-product quadrature rule (parallelized via OpenMP). The resulting
 * estimates are combined via the stored coefficients and finally scaled
 * by the volume of the domain.
 *
 * @param sourceFunc Source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Approximated integral value.
 */
double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the sparse-grid quadrature operator.
 */
namespace quadrature_operator {

/**
 * @brief Tensor-product quadrature on a single tensor grid.
 *
 * Iterates over all grid points, multiplies the per-dimension 1D weights,
 * evaluates @p sourceFunc, and accumulates the weighted sum.
 *
 * @param sourceFunc Source function.
 * @param tg         Tensor grid.
 * @param genInstr   Generation instruction (provides domain and per-dim
 *                   node-generation/quadrature rules).
 * @param scratch    Per-thread scratch space; reused across calls.
 * @return Quadrature estimate on the unit hypercube
 *         @f$[0,1]^d@f$ (the domain volume is multiplied in by the caller).
 */
double quadrature(const SourceFunc& sourceFunc, const TensorGrid& tg, const SGGenInstr& genInstr,
                  tools::QuadScratch& scratch);

/**
 * @brief Populates @p scratch.weights with the 1D quadrature weights for
 * every dimension of @p tg, concatenated dimension by dimension.
 *
 * @param tg            Tensor grid.
 * @param nodeGenFuncs  Per-dimension node-generation functions (each one
 *                      provides the matching quadrature rule).
 * @param scratch       Scratch space; @c weights and @c quadRules are filled in.
 */
void getWeights(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                tools::QuadScratch& scratch);

/**
 * @brief Computes the tensor-product weight at multi-index @p gpMI from
 * the per-dimension weights stored in @p scratch.
 *
 * @param gpMI         Per-dimension grid-point indices.
 * @param gpCntPerDim  Per-dimension grid-point counts (used to walk the
 *                     concatenated weight vector).
 * @param scratch      Scratch space pre-populated by @ref getWeights.
 * @return Tensor-product quadrature weight at @p gpMI.
 */
double getWeightForGP(const std::vector<GPCntType>& gpMI, const GPMI& gpCntPerDim,
                      const tools::QuadScratch& scratch);

/**
 * @brief Selects the per-dimension 1D quadrature rule for @p tg.
 *
 * Stores pointers into @p scratch.quadRules; the rules themselves are
 * provided by the dimension's node-generation function.
 *
 * @param tg            Tensor grid.
 * @param nodeGenFuncs  Per-dimension node-generation functions.
 * @param scratch       Scratch space with size-@c nDim @c quadRules vector.
 */
void getQuadRules(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                  tools::QuadScratch& scratch);

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp
