/**
 * @file global_interpolation.hpp
 * @brief Global tensor-product interpolation operator on a
 * combination-technique sparse grid.
 *
 * Unlike the multilinear operator in @c linear_interpolation.hpp, the
 * global operator uses a (potentially high-degree) 1D interpolation rule
 * per dimension (e.g. a barycentric formula), assembled into a tensor
 * product via successive collapse along each dimension.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Evaluates the global tensor-product sparse-grid interpolant at @p point.
 *
 * Loops over the contained tensor grids (in parallel via OpenMP),
 * performs a tensor-product 1D interpolation on each, and returns the
 * weighted sum @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{glob}}_{\vec{\ell}} f(\vec{x})@f$. The 1D interpolation rule
 * used in each dimension is the one returned by the dimension's
 * node-generation function (e.g. a barycentric Lagrange formula for
 * Chebyshev nodes).
 *
 * @param sourceFunc Black-box source function.
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Interpolated value.
 */
double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the global interpolation operator.
 *
 * The tensor-product evaluation proceeds dimension by dimension: the
 * first dimension is collapsed by interpolating across slices of the
 * tensor grid (consuming function values produced by @p sourceFunc); each
 * subsequent dimension is collapsed by interpolating the intermediate
 * results.
 */
namespace global_interpolation {

/**
 * @brief Tensor-product global interpolation on a single tensor grid.
 *
 * @param sourceFunc Source function to evaluate at grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction (provides domain, node-generation
 *                   functions, and 1D interpolation methods).
 * @return Interpolated value on this single tensor grid.
 */
double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr);

/**
 * @brief Collapses dimension 0 of the tensor grid by 1D interpolation.
 *
 * Evaluates @p sourceFunc on the @f$n_0 \cdot \mathrm{nInterpolations}@f$
 * grid points contributing to the first-dimension collapse and produces
 * one interpolated value per slice along dimension 0.
 *
 * @param sourceFunc       Source function.
 * @param point            Query coordinate in dimension 0.
 * @param tgData           Tensor-grid combination data.
 * @param genInstr         Generation instruction (for node generator + domain).
 * @param nInterpolations  Number of slices to collapse
 *                         (@f$\prod_{k\ge 1} n_k@f$).
 * @return Vector of @p nInterpolations intermediate interpolation values.
 */
std::vector<double> interpolateFirstDim(const SourceFunc& sourceFunc, double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        size_t nInterpolations);

/**
 * @brief Collapses dimension @p dim (with @c dim @c >=1) in place.
 *
 * Consumes the intermediate values previously produced by
 * @ref interpolateFirstDim or by the previous call to this function and
 * shrinks @p interpolationResults by a factor of @c n_dim. After every
 * dimension has been collapsed, @p interpolationResults contains a single
 * scalar (the final interpolated value).
 *
 * @param dim                  Current dimension being collapsed.
 * @param point                Query coordinate in dimension @p dim.
 * @param tgData               Tensor-grid combination data.
 * @param genInstr             Generation instruction.
 * @param nInterpolations      Number of remaining 1D interpolations after
 *                             this collapse.
 * @param interpolationResults In/out: intermediate values; resized to
 *                             @p nInterpolations on return.
 */
void interpolateLaterDim(size_t dim, double point, const TensorGridCTData& tgData,
                         const SGGenInstr& genInstr, size_t nInterpolations,
                         std::vector<double>& interpolationResults);

/**
 * @brief Computes how many 1D interpolations remain after collapsing each
 * dimension.
 *
 * Entry @c k of the returned vector is @f$\prod_{j>k} n_j@f$, i.e. the
 * number of intermediate values that must be produced when the algorithm
 * is about to collapse dimension @c k.
 *
 * @param tensorGrid Tensor grid.
 * @return Per-dimension interpolation count.
 */
std::vector<size_t> getInterpolationCntPerDim(const TensorGrid& tensorGrid);

}  // namespace global_interpolation
}  // namespace combigrid
}  // namespace sgpp
