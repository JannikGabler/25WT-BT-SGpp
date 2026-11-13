// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_quadrature_tools.hpp
 * @brief Quadrature-specific internals of the quadrature operator on a @c DeltasSparseGrid: the
 * 1D quadrature weights in delta order and the vector-valued combination. The grid points are
 * summed with their global weights by @c tools::dsg_weighted_sum (shared with the global
 * interpolation; see there for the global weights and the delta order).
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

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
 * @brief Sets @p result to the sum of @c tools::dsg_weighted_sum::processDeltaTensorGrid
 * (vector-valued, with the quadrature weights) over the delta tensor grids of
 * @c context.deltasSparseGrid (on the unit domain), in parallel.
 *
 * @param context    Read-only data of the delta quadrature call.
 * @param sourceFunc Vector-valued source function (m outputs).
 * @param result     Output: the m sums (size m).
 */
void combineDeltaTensorGrids(const misc::GenericDSGOperatorContext& context,
                             const VectorSourceFunc& sourceFunc, std::span<double> result);

}  // namespace dsg_quadrature
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
