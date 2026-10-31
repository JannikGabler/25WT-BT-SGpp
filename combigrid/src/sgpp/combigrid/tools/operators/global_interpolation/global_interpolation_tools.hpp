// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file global_interpolation_tools.hpp
 * @brief Per-tensor-grid kernels of the global interpolation operator (@c interpolate).
 *
 * The tensor-product evaluation proceeds dimension by dimension: the
 * first dimension is collapsed by interpolating across slices of the
 * tensor grid (consuming function values produced by the source function); each
 * subsequent dimension is collapsed by interpolating the intermediate
 * results. For m outputs, every intermediate result holds m values (node-major), and the 1D
 * rules interpolate all outputs at once.
 *
 * The kernel templates are written once for both value types (@c misc::ScalarValues,
 * @c misc::VectorValues); they are defined and instantiated in the @c .cpp only, the exported
 * entry points are the two @ref processTensorGrid overloads.
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
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
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr);

/**
 * @brief Tensor-product global interpolation of a vector-valued source function on a single
 * tensor grid: the m interpolated values.
 *
 * @param sourceFunc Vector-valued source function to evaluate at grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction.
 * @param out        Output: the m interpolated values (size m).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       std::span<double> out);

/**
 * @brief Kernel of both @ref processTensorGrid overloads: collapses dimension 0
 * (@ref interpolateFirstDim), then every later dimension (@ref interpolateLaterDim).
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (m outputs).
 * @param values   View of the source function (its buffer is not used).
 * @param point    Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData   Tensor grid + level multi-index + combination coefficient.
 * @param genInstr Generation instruction.
 * @param out      Output: the m interpolated values (m = 1 for scalar values); all zero for a
 *                 0-dimensional tensor grid.
 */
template <typename ValueType>
void processTensorGridInternal(const ValueType values, const base::DataVector& point,
                               const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                               double* const out);

/**
 * @brief Collapses dimension 0: evaluates the source function on the grid points, slice by
 * slice of @f$n_0@f$ consecutive grid points, and interpolates every slice at @p point.
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues.
 * @param values          View of the source function.
 * @param point           Coordinate 0 of the query point (normalized).
 * @param tgData          Tensor grid + level multi-index + combination coefficient.
 * @param genInstr        Generation instruction (domain, node-generation functions, boundary
 *                        level offset).
 * @param nInterpolations Number of slices, @f$\prod_{j>0} n_j@f$.
 * @return The @p nInterpolations interpolated values, m values per slice (node-major).
 */
template <typename ValueType>
std::vector<double> interpolateFirstDim(const ValueType values, double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        size_t nInterpolations);

/**
 * @brief Collapses dimension @p dim @f$\geq 1@f$ in place: interpolates every group of
 * @f$n_{dim}@f$ consecutive intermediate results at @p point.
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues.
 * @param values               View of the source function (provides m and the 1D rule).
 * @param dim                  Dimension to collapse.
 * @param point                Coordinate @p dim of the query point (normalized).
 * @param tgData               Tensor grid + level multi-index + combination coefficient.
 * @param genInstr             Generation instruction.
 * @param nInterpolations      Number of results after the collapse, @f$\prod_{j>dim} n_j@f$.
 * @param interpolationResults In: the results of dimension @p dim - 1
 *                             (@f$n_{dim} \cdot@f$ @p nInterpolations, times m); out: the
 *                             @p nInterpolations results (times m).
 */
template <typename ValueType>
void interpolateLaterDim(const ValueType values, size_t dim, double point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                         size_t nInterpolations, std::vector<double>& interpolationResults);

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
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
