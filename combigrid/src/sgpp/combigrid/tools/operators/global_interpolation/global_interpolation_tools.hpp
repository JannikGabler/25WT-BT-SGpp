// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document (update)
 * @brief Implementation details of the global interpolation operator.
 *
 * The tensor-product evaluation proceeds dimension by dimension: the
 * first dimension is collapsed by interpolating across slices of the
 * tensor grid (consuming function values produced by the source function); each
 * subsequent dimension is collapsed by interpolating the intermediate
 * results. For m outputs, every intermediate result holds m values (node-major), and the 1D
 * rules interpolate all outputs at once.
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
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
 * TODO: Document
 */
template <typename ValueType>
void processTensorGridInternal(const ValueType values, const base::DataVector& point,
                               const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                               double* const out);

/**
 * TODO: Document
 */
template <typename ValueType>
std::vector<double> interpolateFirstDim(const ValueType values, double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        size_t nInterpolations);

/**
 * TODO: Document
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