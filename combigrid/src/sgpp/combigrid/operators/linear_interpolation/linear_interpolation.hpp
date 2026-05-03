/**
 * @file linear_interpolation.hpp
 * @brief Multilinear (piecewise-linear in every dimension) interpolation
 * operator on a combination-technique sparse grid.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_scratch.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Evaluates the linear (multilinear) sparse-grid interpolant at @p point.
 *
 * Loops over the contained tensor grids, performs a multilinear
 * interpolation on each, and returns the combination-technique linear
 * combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{lin}}_{\vec{\ell}} f(\vec{x})@f$. The @p point coordinates
 * are interpreted in the domain attached to the sparse grid's generation
 * instruction and are normalized to @f$[0,1]^d@f$ internally before each
 * tensor-grid evaluation.
 *
 * @param sourceFunc Black-box source function to interpolate.
 * @param point      Evaluation point; must have @c size() == sparseGrid.nDim().
 * @param sparseGrid Sparse grid to interpolate on. Must have an attached
 *                   generation instruction.
 * @return Interpolated value.
 */
double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the linear interpolation operator.
 *
 * These helpers operate on a single tensor grid and are used by the
 * top-level driver @ref interpolateLinear. They are exposed primarily for
 * unit-testing.
 */
namespace linear_interpolation {

/**
 * @brief Multilinear interpolation on a single tensor grid.
 *
 * Performs the @f$2^d@f$-corner multilinear interpolation: identifies the
 * grid cell containing @p point, evaluates @p sourceFunc at each of its
 * @f$2^d@f$ corners, and combines them with the standard tensor-product
 * hat-function weights.
 *
 * @param sourceFunc Source function to evaluate at the corner grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction (used for the domain).
 * @param scratch    Per-thread scratch space; reused across calls.
 * @return Interpolated value on this single tensor grid.
 */
double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                   tools::LinInterpScratch& scratch);

/**
 * @brief Computes one corner of the surrounding cell and its associated
 * multilinear weight.
 *
 * The bits of @p idx select the lower (@c 0) or upper (@c 1) per-dimension
 * neighbor; iterating @p idx over @f$[0, 2^d)@f$ visits every corner. If
 * the cell has zero extent in some dimension (the query point coincides
 * with a node) the corresponding factor is replaced by @c 0.5 so that
 * both halves of @p idx contribute equally.
 *
 * @param idx              Corner index in @f$[0, 2^d)@f$.
 * @param point            Query point in normalized coordinates.
 * @param nDim             Spatial dimension.
 * @param neighborsPerDim  Per-dimension @c (lower, upper) neighbor pairs.
 * @param gpOut            Output: coordinates of the selected corner.
 * @return Multilinear weight for this corner.
 */
double computeNeighborGPWithWeight(size_t idx, const base::DataVector& point, const size_t nDim,
                                   const std::vector<std::pair<double, double>>& neighborsPerDim,
                                   base::DataVector& gpOut);

}  // namespace linear_interpolation
}  // namespace combigrid
}  // namespace sgpp
