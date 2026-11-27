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
 * once per tensor grid and dimension. The sum is the weighted sum over the grid points
 * (@c tools::weighted_sum::sumOverRows) with the basis values as 1D weights: nodes read from the
 * node table of the sparse grid, grid points visited row by row, dimensions with one node
 * (basis value 1) skipped, scratch reused across tensor grids. For m outputs, the same operations
 * are performed per output.
 *
 * The basis values are computed on @f$[0,1]^d@f$, from the normalized query point and the unit
 * nodes (@c SparseGridBase::getUnitNodeTable); the source function is evaluated at the domain
 * nodes (@c SparseGridBase::getDomainNodeTable).
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/global_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace global_interpolation {

/**
 * @brief Computes the interpolant of every tensor grid of @p sparseGrid at @p normPoint (without
 * the coefficient) in parallel and stores it at the index of the tensor grid.
 *
 * Every value is computed by one thread with the same operations at every thread count, so the
 * values do not depend on the number of threads or the schedule; combined serially in the order
 * of the tensor grids (@c tools::numerics::compensatedDotProduct), the interpolation results are
 * bitwise independent of both.
 *
 * @param sourceFunc Source function.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param sparseGrid Sparse grid.
 * @return The value of tensor grid i at index i (size nTG).
 */
std::vector<double> estimateTensorGrids(const SourceFunc& sourceFunc,
                                        const base::DataVector& normPoint,
                                        const SparseGrid& sparseGrid);

/**
 * @brief @ref estimateTensorGrids for a vector-valued source function.
 *
 * @param sourceFunc Vector-valued source function (m outputs).
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param sparseGrid Sparse grid.
 * @return The m values of tensor grid i as sub-vector i (nTG sub-vectors of m values; the
 *         elements are index-major, i.e. the values of tensor grid i at i m, ..., i m + m - 1).
 */
misc::FlatNestedVec<double, 2> estimateTensorGrids(const VectorSourceFunc& sourceFunc,
                                                   const base::DataVector& normPoint,
                                                   const SparseGrid& sparseGrid);

/**
 * @brief Tensor-product global interpolation of @p sourceFunc on the tensor grid @p tg.
 *
 * @param sourceFunc Source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p setup's sparse grid).
 * @param unitTG     The same tensor grid with its nodes on the unit interval.
 * @param setup   Setup of the grid (domain, node-generation functions and their 1D
 *                   interpolation methods).
 * @param scratch    Scratch of the calling thread (sized for the sparse grid of @p tg).
 * @return Interpolated value on @p tg (0 for a tensor grid without grid points).
 */
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& normPoint,
                         const TensorGridView& tg, const TensorGridView& unitTG,
                         const SparseGridSetup& setup, misc::GlobalInterpScratch& scratch);

/**
 * @brief Tensor-product global interpolation of a vector-valued source function on @p tg: the m
 * interpolated values.
 *
 * @param sourceFunc Vector-valued source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p setup's sparse grid).
 * @param unitTG     The same tensor grid with its nodes on the unit interval.
 * @param setup   Setup of the grid.
 * @param scratch    Scratch of the calling thread, created with m outputs.
 * @param out        Output: the m interpolated values (size m; zero for a tensor grid without
 *                   grid points).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                       const TensorGridView& tg, const TensorGridView& unitTG,
                       const SparseGridSetup& setup, misc::GlobalInterpScratch& scratch,
                       std::span<double> out);

/**
 * @brief As @ref processTensorGrid(const SourceFunc&, const base::DataVector&, TensorGridView,
 * TensorGridView, const SparseGridSetup&, misc::GlobalInterpScratch&), with a scratch of its own.
 * @note Allocates the scratch; the operator reuses one scratch per thread.
 *
 * @param sourceFunc Source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p setup's sparse grid).
 * @param unitTG     The same tensor grid with its nodes on the unit interval.
 * @param setup   Setup of the grid.
 * @return Interpolated value on this single tensor grid.
 */
double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& normPoint,
                         const TensorGridView& tg, const TensorGridView& unitTG,
                         const SparseGridSetup& setup);

/**
 * @brief As @ref processTensorGrid(const VectorSourceFunc&, const base::DataVector&,
 * TensorGridView, TensorGridView, const SparseGridSetup&, misc::GlobalInterpScratch&,
 * std::span<double>), with a
 * scratch of its own.
 * @note Allocates the scratch; the operator reuses one scratch per thread.
 *
 * @param sourceFunc Vector-valued source function to evaluate at the grid points.
 * @param normPoint  Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg         Tensor grid (of @p setup's sparse grid).
 * @param unitTG     The same tensor grid with its nodes on the unit interval.
 * @param setup   Setup of the grid.
 * @param out        Output: the m interpolated values (size m).
 */
void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& normPoint,
                       const TensorGridView& tg, const TensorGridView& unitTG,
                       const SparseGridSetup& setup, std::span<double> out);

/**
 * @brief Prepares the row loop over @p tg: 1D basis values at @p normPoint
 * (@ref fillBasisValues) and iterated dimensions (@c tools::weighted_sum).
 *
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param tg        Tensor grid (at least one dimension and one grid point).
 * @param unitTG    The same tensor grid with its nodes on the unit interval.
 * @param setup  Setup of the grid.
 * @param scratch   Scratch of the calling thread.
 */
void prepareTensorGrid(const base::DataVector& normPoint, const TensorGridView& tg,
                       const TensorGridView& unitTG, const SparseGridSetup& setup,
                       misc::GlobalInterpScratch& scratch);

/**
 * @brief Writes the 1D basis values of every dimension of @p unitTG at @p normPoint to
 * @c scratch.basisValues (per-dimension blocks back-to-back, in the order of the nodes) and points
 * @c scratch.rows.weightsOfDim to the blocks.
 *
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param unitTG    Tensor grid with its nodes on the unit interval.
 * @param setup  Setup of the grid (the interpolation method of every dimension).
 * @param scratch   Scratch of the calling thread.
 */
void fillBasisValues(const base::DataVector& normPoint, const TensorGridView& unitTG,
                     const SparseGridSetup& setup, misc::GlobalInterpScratch& scratch);

}  // namespace global_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
