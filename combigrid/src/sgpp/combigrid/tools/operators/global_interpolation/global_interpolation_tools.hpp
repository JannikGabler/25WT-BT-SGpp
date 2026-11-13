// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file global_interpolation_tools.hpp
 * @brief Per-tensor-grid kernel of the global interpolation operator (@c interpolate) on a
 * @c SparseGrid.
 *
 * The tensor-product interpolant of a tensor grid at a query point x is
 * @f$\sum_p \prod_k L_{k,p_k}(x_k) f(p)@f$, where @f$L_{k,i}(x_k)@f$ are the 1D basis values
 * of the interpolation method of dimension k (@c InterpolationMethod::basisValues), computed
 * once per tensor grid and dimension. The sum is the quadrature row loop
 * (@c tools::quadrature::sumOverRows) with the basis values in place of the quadrature weights:
 * nodes mapped onto the domain once per tensor grid, grid points visited row by row, dimensions
 * with one node (basis value 1) skipped, scratch reused across tensor grids. For m outputs, the
 * same operations are performed per output.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/global_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace global_interpolation {

/**
 * @brief Tensor-product global interpolation of @p sourceFunc on the tensor grid @p tg.
 *
 * @param sourceFunc Source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p genInstr's sparse grid).
 * @param genInstr   Generation instruction (domain, node-generation functions and their 1D
 *                   interpolation methods).
 * @param scratch    Scratch of the calling thread (sized for the sparse grid of @p tg).
 * @return Interpolated value on @p tg (0 for a tensor grid without grid points).
 */
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& normPoint,
                         const TensorGrid& tg, const SGGenInstr& genInstr,
                         misc::GlobalInterpScratch& scratch);

/**
 * @brief Tensor-product global interpolation of a vector-valued source function on @p tg: the m
 * interpolated values.
 *
 * @param sourceFunc Vector-valued source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p genInstr's sparse grid).
 * @param genInstr   Generation instruction.
 * @param scratch    Scratch of the calling thread, created with m outputs.
 * @param out        Output: the m interpolated values (size m; zero for a tensor grid without
 *                   grid points).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                       const TensorGrid& tg, const SGGenInstr& genInstr,
                       misc::GlobalInterpScratch& scratch, std::span<double> out);

/**
 * @brief As @ref processTensorGrid(const SourceFunc&, const base::DataVector&, const TensorGrid&,
 * const SGGenInstr&, misc::GlobalInterpScratch&), with a scratch of its own.
 * @note Allocates the scratch; the operator reuses one scratch per thread.
 *
 * @param sourceFunc Source function to evaluate at the grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction.
 * @return Interpolated value on this single tensor grid.
 */
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr);

/**
 * @brief As @ref processTensorGrid(const VectorSourceFunc&, const base::DataVector&,
 * const TensorGrid&, const SGGenInstr&, misc::GlobalInterpScratch&, std::span<double>), with a
 * scratch of its own.
 * @note Allocates the scratch; the operator reuses one scratch per thread.
 *
 * @param sourceFunc Vector-valued source function to evaluate at the grid points.
 * @param point      Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tgData     Tensor grid + level multi-index + combination coefficient.
 * @param genInstr   Generation instruction.
 * @param out        Output: the m interpolated values (size m).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       std::span<double> out);

/**
 * @brief Sets @p result to the combination of the vector-valued interpolants of all tensor
 * grids of @p sparseGrid at @p normPoint, in parallel (per thread first, then added to
 * @p result, as the reduction of the scalar operator does output by output).
 *
 * @param sourceFunc Vector-valued source function.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param sparseGrid Sparse grid.
 * @param result     Output: the m combined values (size m).
 */
void combineTensorGrids(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                        const SparseGrid& sparseGrid, std::span<double> result);

/**
 * @brief Prepares the row loop over @p tg: 1D basis values at @p normPoint
 * (@ref fillBasisValues), nodes on the domain and iterated dimensions (as the quadrature).
 *
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg        Tensor grid (at least one dimension and one grid point).
 * @param genInstr  Generation instruction.
 * @param scratch   Scratch of the calling thread.
 */
void prepareTensorGrid(const base::DataVector& normPoint, const TensorGrid& tg,
                       const SGGenInstr& genInstr, misc::GlobalInterpScratch& scratch);

/**
 * @brief Writes the 1D basis values of every dimension of @p tg at @p normPoint to
 * @c scratch.rows.weights1D (per-dimension blocks back-to-back, in the order of the nodes).
 *
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg        Tensor grid.
 * @param genInstr  Generation instruction (the interpolation method of every dimension).
 * @param scratch   Scratch of the calling thread.
 */
void fillBasisValues(const base::DataVector& normPoint, const TensorGrid& tg,
                     const SGGenInstr& genInstr, misc::GlobalInterpScratch& scratch);

}  // namespace global_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
