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
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <span>
#include <utility>
#include <vector>

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
 * corner (@ref computeNeighborGPWithWeight) is computed once and serves all m outputs.
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (m outputs; accumulator slot
 *                   @c misc::LinInterpScratch::RESULT_ACC of the view's buffer).
 * @param values   View of the source function.
 * @param point    Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData   Tensor grid + level multi-index + combination coefficient.
 * @param genInstr Generation instruction (used for the domain).
 * @param scratch  Per-thread scratch space (neighbors per dimension, current corner).
 * @return Accumulator of the interpolated values (a @c double for scalar values, a pointer to
 *         the m values in the view's buffer for vector values); zero for a 0-dimensional tensor
 *         grid.
 */
template <typename ValueType>
typename ValueType::Acc sumOverCorners(ValueType values, const base::DataVector& point,
                                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                       misc::LinInterpScratch& scratch);

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
double computeNeighborGPWithWeight(size_t idx, const base::DataVector& point, size_t nDim,
                                   const std::vector<std::pair<double, double>>& neighborsPerDim,
                                   base::DataVector& gpOut);

}  // namespace lin_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
