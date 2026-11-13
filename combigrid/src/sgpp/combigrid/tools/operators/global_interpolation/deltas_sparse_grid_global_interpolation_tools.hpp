// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_global_interpolation_tools.hpp
 * @brief Internals of the global interpolation operator on a deltas sparse grid (@c interpolate
 * on a @c DeltasSparseGrid): every grid point is evaluated once per call and weighted with its
 * global weight.
 *
 * The combination technique interpolant is
 * @f$\sum_{\vec{\ell}} c_{\vec{\ell}} \sum_{p \in G_{\vec{\ell}}} \prod_k L_{k,\ell_k,p_k}(x_k)
 * f(p)@f$, where @f$L_{k,\ell,i}(x)@f$ is the weight of node i in the 1D interpolant of level
 * @f$\ell@f$ of dimension k at x (@c InterpolationMethod::basisValues; for a Lagrange rule the
 * Lagrange basis polynomial). Grouped by grid point, it is @f$\sum_p W_p(\vec{x}) f(p)@f$ with
 * @f$W_p(\vec{x}) = \sum_{\vec{\ell} \geq \vec{m}} c_{\vec{\ell}} \prod_k L_{k,\ell_k,p_k}(x_k)
 * @f$ (@f$\vec{m}@f$: the level of the delta tensor grid of p). These are the global weights of
 * the deltas sparse grid quadrature with the 1D quadrature weights replaced by the 1D basis
 * values at @f$\vec{x}@f$, so both operators share the weighted sum over the grid points
 * (@c tools::dsg_weighted_sum): a call computes the basis values of every (dimension, level) in
 * delta order (@ref compBasisValues), then sums the weighted values of the delta tensor grids
 * (in parallel).
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <span>

namespace sgpp {
namespace combigrid {
namespace tools {
namespace dsg_global_interpolation {

/**
 * @brief Returns the index of the global interpolation on @p deltasSparseGrid, built on the first
 * call (thread-safe) and kept in the grid's operator data (@c DeltasSparseGrid::getOperatorData).
 */
const misc::DSGGlobalInterpIndex& getIndex(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Returns whether a call runs in parallel: the grid has at least
 * @c constants::global_interpolation::DSG_PARALLEL_MIN_GRID_POINTS grid points.
 */
bool runInParallel(const misc::DSGGlobalInterpIndex& index);

/**
 * @brief Computes the basis values of the 1D interpolant of row @p row (dimension j, level l) at
 * @c normPoint[j] (@c InterpolationMethod::basisValues on the regular nodes) and writes them in
 * delta order to the row's entries of @p weights.
 *
 * @param index     Index of the deltas sparse grid.
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param row       Row of @p index.
 * @param regular   Scratch of at least @c index.maxRowSize() doubles.
 * @param weights   Output: a copy of @c index.weightLayout().
 */
void fillBasisValuesOfRow(const misc::DSGGlobalInterpIndex& index,
                          const base::DataVector& normPoint, size_t row, std::span<double> regular,
                          misc::FlatNestedVec<double, 3>& weights);

/**
 * @brief Computes the 1D weights of the query point: the basis values of every row
 * (@ref fillBasisValuesOfRow), in the layout of @c tools::dsg_quadrature::comp1DWeights.
 *
 * @param index     Index of the deltas sparse grid.
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @return @c weights[dim][lvl]: the basis values of the nodes in delta order.
 */
misc::FlatNestedVec<double, 3> compBasisValues(const misc::DSGGlobalInterpIndex& index,
                                               const base::DataVector& normPoint);

/**
 * @brief Evaluates the interpolant at the query point: computes the basis values, then sums the
 * global weight times the source function over all grid points of @p deltasSparseGrid (every grid
 * point evaluated once): the sums of the delta tensor grids
 * (@c dsg_weighted_sum::processDeltaTensorGrid), added in the grid's order.
 *
 * @param index            Index of @p deltasSparseGrid.
 * @param deltasSparseGrid Deltas sparse grid.
 * @param normPoint        Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param sourceFunc       Source function.
 * @param parallel         Whether to run in parallel (one parallel region; else on the calling
 *                         thread, without a parallel region); the result is the same.
 * @return The interpolated value.
 */
double evaluateInterpolant(const misc::DSGGlobalInterpIndex& index,
                           const DeltasSparseGrid& deltasSparseGrid,
                           const base::DataVector& normPoint, const SourceFunc& sourceFunc,
                           bool parallel);

/**
 * @brief @ref evaluateInterpolant for a vector-valued source function (m outputs; the global
 * weight of a grid point serves all outputs).
 *
 * @param index            Index of @p deltasSparseGrid.
 * @param deltasSparseGrid Deltas sparse grid.
 * @param normPoint        Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param sourceFunc       Vector-valued source function.
 * @param parallel         Whether to run in parallel; the result is the same.
 * @param result           Output: the m interpolated values.
 */
void evaluateInterpolant(const misc::DSGGlobalInterpIndex& index,
                         const DeltasSparseGrid& deltasSparseGrid,
                         const base::DataVector& normPoint, const VectorSourceFunc& sourceFunc,
                         bool parallel, std::span<double> result);

}  // namespace dsg_global_interpolation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
