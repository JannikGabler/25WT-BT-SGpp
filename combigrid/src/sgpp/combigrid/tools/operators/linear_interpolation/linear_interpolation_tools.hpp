// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_tools.hpp
 * @brief Per-tensor-grid kernels of the linear interpolation operator (@c interpolateLinear).
 *
 * On every tensor grid, the multilinear interpolant at the query point is the weighted sum of
 * the source function at the @f$2^d@f$ corners of the grid cell containing the point. The corner
 * loop (@ref sumOverCorners) is written once for both value types (@c misc::ScalarValues,
 * @c misc::VectorValues); it is defined and instantiated in the @c .cpp only, the exported entry
 * points are the two @ref processTensorGrid overloads.
 *
 * Corner @c c (bit k of @c c selects the upper neighbor in dimension k) has the weight
 * @f$\prod_{k=0}^{d-1} w_k@f$, multiplied in this order, with the 1D weights
 * @f$w_k = (b_k - x_k) / (b_k - a_k)@f$ (lower neighbor @f$a_k@f$) resp.
 * @f$(x_k - a_k) / (b_k - a_k)@f$ (upper neighbor @f$b_k@f$), or 0.5 for both if the cell has
 * zero extent in dimension k (the query point is a node). The corners are visited and summed in
 * the order of @c c.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lin_interpolation {

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
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                         misc::LinInterpScratch& scratch);

/**
 * @brief Multilinear interpolation of a vector-valued source function on a single tensor grid:
 * the same corner loop, the weight of a corner serves all m outputs.
 *
 * @param sourceFunc Vector-valued source function to evaluate at the corner grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction (used for the domain).
 * @param scratch    Per-thread scratch space, constructed with m outputs.
 * @param out        Output: the m interpolated values (size m).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       misc::LinInterpScratch& scratch, std::span<double> out);

/**
 * @brief Kernel of both @ref processTensorGrid overloads: the corner loop. The weight of a
 * corner is computed once and serves all m outputs.
 *
 * Per tensor grid, the bracketing coordinates are mapped onto the domain once and the products
 * of the 1D weights of the first @c misc::LinInterpScratch::tableDim dimensions are tabulated
 * (by doubling: the same products in the same order as the definition, so the weights are
 * bitwise those of the definition); the corners are visited in index order, each one changing
 * the coordinates of the dimensions whose bit changes (two on average).
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (m outputs; accumulator slot
 *                   @c misc::LinInterpScratch::RESULT_ACC of the view's buffer).
 * @param values   View of the source function.
 * @param point    Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData   Tensor grid + level multi-index + combination coefficient.
 * @param genInstr Generation instruction (used for the domain).
 * @param scratch  Per-thread scratch space (neighbors, corner data, current corner).
 * @return Accumulator of the interpolated values (a @c double for scalar values, a pointer to
 *         the m values in the view's buffer for vector values); zero for a 0-dimensional tensor
 *         grid.
 */
template <typename ValueType>
typename ValueType::Acc sumOverCorners(ValueType values, const base::DataVector& point,
                                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                       misc::LinInterpScratch& scratch);

/**
 * @brief 1D weights of the lower and the upper neighbor of a coordinate: the relative distances
 * to the other neighbor.
 *
 * @param x     Coordinate in @f$[\mathrm{lower}, \mathrm{upper}]@f$ (normalized).
 * @param lower Lower neighbor @f$a@f$.
 * @param upper Upper neighbor @f$b@f$.
 * @return @f$((b - x) / (b - a), (x - a) / (b - a))@f$, or @c (0.5, 0.5) if @f$a = b@f$ (the
 *         coordinate is a node).
 */
std::pair<double, double> weights1D(double x, double lower, double upper);

/**
 * @brief Prepares the corner loop (@ref sumOverCorners) on one tensor grid.
 *
 * Brackets @p point in @p tg (@c TensorGrid::getNeighborsForLinInterpolation), maps the
 * bracketing coordinates onto @p domain (@c tools::mapToDomain, as
 * @c tools::denormalizeDataVector), computes the 1D weights (@ref weights1D), builds the corner
 * weight table of the first @c misc::LinInterpScratch::tableDim dimensions and sets the current
 * grid point to corner 0 (all lower neighbors).
 *
 * @param point   Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg      Tensor grid of dimension d > 0.
 * @param domain  Domain of the sparse grid.
 * @param scratch Per-thread scratch space of dimension d (output: neighbors, corner data,
 *                current grid point).
 */
void prepareCorners(const base::DataVector& point, const TensorGrid& tg,
                    const HyperCubeArea& domain, misc::LinInterpScratch& scratch);

/**
 * @brief Moves the current grid point from corner @p corner - 1 to corner @p corner.
 *
 * With t the lowest set bit of @p corner, the coordinates of dimensions 0 ... t - 1 change to
 * the lower neighbors and the coordinate of dimension t to the upper one (the other bits are
 * those of @p corner - 1).
 *
 * @param corner  Corner index in @f$[1, 2^d)@f$.
 * @param bracket Bracketing coordinates on the domain (@c misc::LinInterpScratch::bracket).
 * @param gp      Coordinates of the current grid point (d values), updated in place.
 */
void moveToCorner(size_t corner, const double* bracket, double* gp);

}  // namespace lin_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
