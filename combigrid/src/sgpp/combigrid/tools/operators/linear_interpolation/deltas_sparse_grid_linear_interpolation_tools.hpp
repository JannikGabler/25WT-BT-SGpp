// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dsg_linear_interpolation_tools.hpp
 * @brief Internals of the linear interpolation operator on a deltas sparse grid
 * (@c interpolateLinear on a @c DeltasSparseGrid): every distinct corner of the combination
 * tensor grids is evaluated once per call.
 *
 * The combination technique interpolant is @f$\sum_{\vec{\ell}} c_{\vec{\ell}}
 * I^{\mathrm{lin}}_{\vec{\ell}} f(\vec{x})@f$; @f$I^{\mathrm{lin}}_{\vec{\ell}}@f$ is the weighted
 * sum of f at the @f$2^d@f$ corners of the cell of the tensor grid @f$\vec{\ell}@f$ that contains
 * @f$\vec{x}@f$ (see linear_interpolation_tools.hpp). Most corners are shared by many tensor grids:
 * with nested nodes, a corner coordinate in dimension j is identified by the level at which it
 * appears first in the bracket chain of @f$x_j@f$ (see @c misc::DSGLinInterpIndex), which maps
 * every corner to a slot of a per-call value store. A call (@ref prepareQuery, then the three
 * passes in one parallel region):
 * 1. @ref markCorners: per combination tensor grid (parallel), the slots of its corners are marked;
 * 2. @ref evaluateMarked: per block of slots (parallel), the source function is evaluated at the
 *    marked slots, i.e. exactly once at every distinct corner and nowhere else;
 * 3. @ref gatherCorners: per combination tensor grid (parallel), the values of its corners are
 *    summed with the weights and in the order of the per-tensor-grid kernel of the @c SparseGrid
 *    operator (@c lin_interpolation::sumOverCorners), so the sum of a tensor grid is the one of
 *    that kernel; the sums are combined with the coefficients.
 *
 * Passes 1 and 3 walk the corners of a tensor grid in index order, computing the slot
 * incrementally like the coordinates of the kernel (forEachCornerSlot in the .cpp): corner c
 * changes the trie lookups of the dimensions 0 ... t (t: lowest set bit of c, two on average).
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_query.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {
namespace tools {
namespace dsg_lin_interpolation {

/**
 * @brief Returns the index of the linear interpolation on @p deltasSparseGrid, built on the first
 * call (thread-safe) and kept in the grid's operator data (@c DeltasSparseGrid::getOperatorData).
 * @throws std::invalid_argument if the grid has no dimensions.
 */
const misc::DSGLinInterpIndex& getIndex(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Returns whether a call runs its passes in parallel: the number of corner visits per pass
 * is at least @c constants::lin_interpolation::DSG_PARALLEL_MIN_CORNER_VISITS.
 */
bool runInParallel(const misc::DSGLinInterpIndex& index);

/**
 * @brief Fills the corner ids, 1D weights and coordinates of @p query for the query point.
 * @param index     Index of the deltas sparse grid.
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param domain    Domain of the deltas sparse grid.
 * @param query     Data of the call.
 */
void prepareQuery(const misc::DSGLinInterpIndex& index, const base::DataVector& normPoint,
                  const HyperCubeArea& domain, misc::DSGLinInterpQuery& query);

/**
 * @brief @ref prepareQuery for dimension @p dim: walks the levels 0 ... maxLvl(dim), brackets
 * @p x in the nodes of each level (the union of the delta nodes up to that level, as
 * @c TensorGrid::getNeighborsForLinInterpolation does in the nodes of a tensor grid) and assigns
 * each bracketing coordinate the id of its first appearance.
 *
 * @param index  Index of the deltas sparse grid.
 * @param x      Normalized query coordinate.
 * @param dim    Dimension.
 * @param domain Domain interval of @p dim.
 * @param query  Data of the call (rows of @p dim).
 */
void prepareDimension(const misc::DSGLinInterpIndex& index, double x, size_t dim,
                      std::pair<double, double> domain, misc::DSGLinInterpQuery& query);

/**
 * @brief Index of the first value of @p nodes (sorted) that is not less than @p x, as
 * @c std::lower_bound, searched from @p hint outwards with doubling steps (galloping), then by
 * bisection. Any hint gives the result of @c std::lower_bound; a hint near the result touches few
 * cache lines (@ref prepareDimension searches the delta nodes of every level, large arrays at high
 * levels, starting from the position of the previous level).
 */
size_t lowerBoundFrom(std::span<const double> nodes, double x, size_t hint);

/**
 * @brief Bracket of a coordinate in sorted nodes, given the largest node below it and the
 * smallest node at or above it (-inf / +inf if there is none), with the rules of
 * @c TensorGrid::getNeighborsForLinInterpolation: (largest, 1) if no node is at or above @p x;
 * (node, node) if the node at or above @p x is within 1e-12 of @p x; (0, smallest) if no node is
 * below @p x; else (below, at or above).
 */
std::pair<double, double> bracketOf(double below, double atOrAbove, double x);

/**
 * @brief Sets @c scratch.cornerIds to the corner ids of the tensor grid of level @p mi.
 */
void setCornerIds(const misc::DSGLinInterpIndex& index, const misc::DSGLinInterpQuery& query,
                  const LvlMI& mi, misc::DSGLinInterpScratch& scratch);

/**
 * @brief Pass 1: marks the slots of the corners of all combination tensor grids (work-shared
 * loop over the tensor grids, binds to the enclosing parallel region; ends with its barrier).
 */
void markCorners(const misc::DSGLinInterpIndex& index, misc::DSGLinInterpQuery& query,
                 misc::DSGLinInterpScratch& scratch);

/**
 * @brief Pass 2: evaluates the source function at the marked slots and stores the values
 * (work-shared loop over the blocks of slots; ends with its barrier).
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (buffer of @p scratch).
 */
template <typename ValueType>
void evaluateMarked(ValueType values, const misc::DSGLinInterpIndex& index,
                    misc::DSGLinInterpQuery& query, misc::DSGLinInterpScratch& scratch);

/**
 * @brief Pass 3: sums the stored values of the corners of the combination tensor grids with the
 * corner weights (per tensor grid as the @c SparseGrid kernel) and combines the sums with the
 * coefficients (work-shared loop over the tensor grids without barrier).
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (accumulator slots
 *                   @c misc::DSGLinInterpScratch::TG_ACC and @c THREAD_ACC of the buffer).
 * @return The combination of the tensor grids of this thread (a @c double, or a pointer to the m
 *         values in the buffer).
 */
template <typename ValueType>
typename ValueType::Acc gatherCorners(ValueType values, const misc::DSGLinInterpIndex& index,
                                      const misc::DSGLinInterpQuery& query,
                                      misc::DSGLinInterpScratch& scratch);

}  // namespace dsg_lin_interpolation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
