// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_quadrature_tools.hpp
 * @brief Quadrature-specific internals of the quadrature operator on a @c DeltasSparseGrid: the
 * 1D quadrature weights in delta order and the parallel loop over the delta tensor grids. The grid
 * points are summed with their global weights by @c tools::dsg_weighted_sum (shared with the global
 * interpolation; see there for the global weights and the delta order).
 */
#pragma once

#include <memory>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_quadrature_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

/**
 * @brief Computes the value of every delta tensor grid of @c context.deltasSparseGrid
 * (@c tools::dsg_weighted_sum::processDeltaTensorGrid: the sum of the global weights times the
 * source function over its grid points, on the unit hypercube) in parallel and stores it at the
 * index of the delta tensor grid.
 *
 * Every value is computed by one thread with the same operations at every thread count, so the
 * values do not depend on the number of threads or the schedule; summed serially in the order of
 * the delta tensor grids (@c tools::numerics::compensatedSum), the quadrature results are
 * bitwise independent of both.
 *
 * @param context    Read-only data of the delta quadrature call.
 * @param sourceFunc Source function.
 * @return The value of delta tensor grid i at index i (size nTG).
 */
std::vector<double> estimateDeltaTensorGrids(const misc::GenericDSGOperatorContext& context,
                                             const SourceFunc& sourceFunc);

/**
 * @brief @ref estimateDeltaTensorGrids for a vector-valued source function.
 *
 * @param context    Read-only data of the delta quadrature call.
 * @param sourceFunc Vector-valued source function (m outputs).
 * @return The m values of delta tensor grid i as sub-vector i (nTG sub-vectors of m values; the
 *         elements are index-major, i.e. the values of delta tensor grid i at i m, ...,
 *         i m + m - 1).
 */
misc::FlatNestedVec<double, 2> estimateDeltaTensorGrids(
    const misc::GenericDSGOperatorContext& context, const VectorSourceFunc& sourceFunc);

/**
 * @brief Computes the 1D quadrature weights of every level of every dimension in delta order.
 *
 * @c weights1D[dim][lvl] holds the weights of the regular 1D rule of level @c lvl (including the
 * boundary nodes if @c lvl is at least the boundary level offset), for every level up to the
 * maximum level of @c dim in @p deltasSparseGrid. They are not sorted by node position but by
 * delta level: first the weights of the delta nodes of level 0, then the ones of level 1, ...,
 * each block in the order of the nodes of the delta tensor grids. Hence the weights of the delta
 * nodes of level @c m start at index @c weights1D[dim][m-1].size() (at 0 for @c m = 0).
 */
misc::FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Returns the query-independent data of the quadrature on @p deltasSparseGrid (1D weights,
 * coefficient trie): from the grid's cross-call operator data cache if @p useCache (stored there
 * on the first call), else built for the caller.
 */
std::shared_ptr<const misc::DSGQuadIndex> getIndex(const DeltasSparseGrid& deltasSparseGrid,
                                                   bool useCache);

}  // namespace dsg_quadrature
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
