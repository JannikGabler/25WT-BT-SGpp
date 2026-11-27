// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_tools.hpp
 * @brief Per-tensor-grid kernel of the quadrature operator (@c quadrature) on a @c SparseGrid:
 * the 1D quadrature weights of a tensor grid and the weighted sum over its grid points
 * (@c tools::weighted_sum, shared with the global interpolation).
 */
#pragma once

#include <memory>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

/**
 * @brief Computes the quadrature estimate of every tensor grid of @c context.sparseGrid (unit
 * hypercube, without the coefficient) in parallel and stores it at the index of the tensor grid.
 *
 * Every estimate is computed by one thread with the same operations at every thread count, so
 * the estimates do not depend on the number of threads or the schedule; combined serially in the
 * order of the tensor grids (@c tools::numerics::compensatedDotProduct), the quadrature results
 * are bitwise independent of both.
 *
 * @param context     Read-only data of the quadrature call.
 * @param sourceFunc  Source function.
 * @return The estimate of tensor grid i at index i (size nTG).
 */
std::vector<double> estimateTensorGrids(const misc::QuadratureContext& context,
                                        const SourceFunc& sourceFunc);

/**
 * @brief @ref estimateTensorGrids for a vector-valued source function.
 *
 * @param context     Read-only data of the quadrature call.
 * @param sourceFunc  Vector-valued source function (m outputs).
 * @return The m estimates of tensor grid i as sub-vector i (nTG sub-vectors of m values; the
 *         elements are index-major, i.e. the values of tensor grid i at i m, ..., i m + m - 1).
 */
misc::FlatNestedVec<double, 2> estimateTensorGrids(const misc::QuadratureContext& context,
                                                   const VectorSourceFunc& sourceFunc);

/**
 * @brief Tensor-product quadrature on a single tensor grid.
 *
 * Looks up the 1D quadrature weights of @p tg (@ref getWeights) and sums the weight times the
 * source function over the grid points (@c tools::weighted_sum::sumOverRows): the nodes are read
 * from the node table of the sparse grid (in domain coordinates), the grid points are visited row
 * by row, the dimensions with one node are
 * not iterated (constant coordinate and weight factor), and a step to the next row updates only
 * the coordinates and the partial weight products of the dimensions that changed. The source
 * function is evaluated at the grid points in domain coordinates.
 *
 * @param weightTable 1D quadrature weights per (dimension, level) of the tensor grid's levels.
 * @param sourceFunc Source function.
 * @param tg         Tensor grid (at least one dimension).
 * @param scratch    Per-thread scratch space; reused across calls.
 * @return Quadrature estimate on the unit hypercube
 *         @f$[0,1]^d@f$ (the domain volume is multiplied in by the caller).
 */
double processTensorGrid(const misc::QuadWeightTable& weightTable, const SourceFunc& sourceFunc,
                         TensorGridView tg, misc::QuadratureScratch& scratch);

/**
 * @brief @ref processTensorGrid for a vector-valued source function: the same row loop with m
 * values per grid point.
 *
 * @param weightTable 1D quadrature weights per (dimension, level).
 * @param sourceFunc Vector-valued source function (m outputs).
 * @param tg         Tensor grid (at least one dimension).
 * @param scratch    Per-thread scratch space, constructed with m outputs; reused across calls.
 * @param out        Output: the m quadrature estimates on the unit hypercube (size m).
 */
void processTensorGrid(const misc::QuadWeightTable& weightTable, const VectorSourceFunc& sourceFunc,
                       TensorGridView tg, misc::QuadratureScratch& scratch, std::span<double> out);

/**
 * @brief Prepares the row loop over @p tg: 1D quadrature weights (@ref getWeights) and iterated
 * dimensions (@c tools::weighted_sum).
 *
 * @param weightTable 1D quadrature weights per (dimension, level).
 * @param tg      Tensor grid (at least one dimension and one grid point).
 * @param scratch Per-thread scratch space.
 */
void prepareTensorGrid(const misc::QuadWeightTable& weightTable, TensorGridView tg,
                       misc::QuadratureScratch& scratch);

/**
 * @brief Points @c scratch.rows.weightsOfDim to the 1D quadrature weights of every dimension of
 * @p tg in the weight table of the quadrature call (computing them there on first use).
 *
 * @param weightTable 1D quadrature weights per (dimension, level).
 * @param tg      Tensor grid.
 * @param scratch Scratch space; @c rows.weightsOfDim is filled in.
 */
void getWeights(const misc::QuadWeightTable& weightTable, TensorGridView tg,
                misc::QuadratureScratch& scratch);

/**
 * @brief Returns the 1D quadrature weight table of @p sparseGrid: from the grid's cross-call
 * operator data cache if @p useCache (stored there on the first call), else a new table.
 */
std::shared_ptr<const misc::QuadWeightTable> getWeightTable(const SparseGrid& sparseGrid,
                                                            bool useCache);

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
