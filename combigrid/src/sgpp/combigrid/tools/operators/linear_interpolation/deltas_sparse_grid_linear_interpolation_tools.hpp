// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_tools.hpp
 * @brief Internals of the linear interpolation operator on a deltas sparse grid
 * (@c interpolateLinear on a @c DeltasSparseGrid): every distinct corner of the combination
 * tensor grids is evaluated once per call and weighted with its global weight.
 *
 * The combination technique interpolant is @f$\sum_{\vec{\ell}} c_{\vec{\ell}}
 * I^{\mathrm{lin}}_{\vec{\ell}} f(\vec{x})@f$; @f$I^{\mathrm{lin}}_{\vec{\ell}}@f$ is the weighted
 * sum of f at the @f$2^d@f$ corners of the cell of the tensor grid @f$\vec{\ell}@f$ that contains
 * @f$\vec{x}@f$ (see linear_interpolation_tools.hpp). Most corners are shared by many tensor grids:
 * with nested nodes, a corner coordinate in dimension j is identified by the level at which it
 * appears first in the bracket chain of @f$x_j@f$ and its index s among the coordinates new there,
 * its id @f$(\lambda_j, s_j)@f$ (see @c misc::DSGLinInterpIndex), which maps every corner to a
 * slot.
 *
 * <b>Slot weights.</b> Let @f$v_j(\ell; \lambda, s)@f$ be the 1D weight of the id
 * @f$(\lambda, s)@f$ at level @f$\ell \geq \lambda@f$ (0 if it does not bracket @f$x_j@f$ at
 * @f$\ell@f$). The interpolant is @f$\sum_{\mathrm{slots}} W \cdot f@f$ with
 * @f[ W(\vec{\lambda}, \vec{s}) = \sum_{\vec{\ell}} c_{\vec{\ell}}
 *     \prod_{j=0}^{d-1} v_j(\ell_j; \lambda_j, s_j). @f]
 * The sum factorizes into one 1D transform per dimension (sum factorization over the downwards
 * closed set): starting with @f$c_{\vec{\ell}}@f$ in the first slot of every multi-index, the
 * transform of dimension k replaces the level @f$\ell_k@f$ by the id @f$(\lambda_k, s_k)@f$ along
 * every fiber of dimension k (@ref transformFiber), in place:
 * @f$W_k(\ldots, (\lambda_k, s_k), \ldots) = \sum_{\ell_k \geq \lambda_k}
 * v_k(\ell_k; \lambda_k, s_k)\, W_{k-1}(\ldots, \ell_k, \ldots)@f$, summed in ascending order
 * of @f$\ell_k@f$: each level adds its input to the two ids that bracket @f$x_k@f$ there. The
 * cost is two multiply-adds per slot and dimension, independent of the number of tensor grids and
 * of @f$2^d@f$. Alongside, a flag per slot is transformed with "or" instead of the weighted sum,
 * starting from @f$c_{\vec{\ell}} \neq 0@f$: it ends nonzero exactly at the corners of the
 * combination tensor grids (the corners of tensor grids with coefficient 0 and the slots of ids
 * that occur at no level are not evaluated, also where the weight cancels).
 *
 * A call: @ref prepareContext fills the read-only @c misc::DSGLinInterpContext (brackets, ids,
 * 1D weights). Then, in one parallel region, the d transforms (@ref transformDimension, one barrier
 * each) and @ref evaluateChunks (each reached slot evaluated once, its value times its weight
 * summed per fixed chunk of leaves) write the shared @c misc::DSGLinInterpSlotScratch; finally the
 * chunk sums are added in order (@ref sumChunks). Every slot weight and chunk sum is computed by
 * one thread in a fixed order, so the result does not depend on the number of threads.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_lin_interpolation {

/**
 * @brief Returns the index of the linear interpolation on @p deltasSparseGrid: from the grid's
 * cross-call operator data cache if @p useCache (built on the first call, thread-safe), else
 * built for the caller.
 * @throws std::invalid_argument if the grid has no dimensions.
 */
std::shared_ptr<const misc::DSGLinInterpIndex> getIndex(const DeltasSparseGrid& deltasSparseGrid,
                                                        bool useCache);

/**
 * @brief Returns whether a call runs its passes in parallel: the number of corner visits (number
 * of combination tensor grids times @f$2^d@f$) is at least
 * @c constants::lin_interpolation::DSG_PARALLEL_MIN_CORNER_VISITS.
 */
bool runInParallel(const misc::DSGLinInterpIndex& index);

/**
 * @brief Fills the corner ids, 1D weights and coordinates of @p context for the query point.
 * @param index     Index of the deltas sparse grid.
 * @param normPoint Query point in normalized @f$[0,1]^d@f$ coordinates.
 * @param domain    Domain of the deltas sparse grid.
 * @param context   Read-only data of the call (output).
 */
void prepareContext(const misc::DSGLinInterpIndex& index, const base::DataVector& normPoint,
                    const HyperCubeArea& domain, misc::DSGLinInterpContext& context);

/**
 * @brief @ref prepareContext for dimension @p dim: walks the levels 0 ... maxLvl(dim), brackets
 * @p x in the nodes of each level (the union of the delta nodes up to that level, as
 * @c TensorGrid::getNeighborsForLinInterpolation does in the nodes of a tensor grid) and assigns
 * each bracketing coordinate the id of its first appearance.
 *
 * @param index  Index of the deltas sparse grid.
 * @param x      Normalized query coordinate.
 * @param dim    Dimension.
 * @param domain Domain interval of @p dim.
 * @param context Read-only data of the call (output: rows of @p dim).
 */
void prepareDimension(const misc::DSGLinInterpIndex& index, double x, size_t dim,
                      std::pair<double, double> domain, misc::DSGLinInterpContext& context);

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
 * @brief Transform of dimension @p dim along the fiber @p fiber (see the file description), in
 * place in @c slots.slotWeights and @c slots.reached; the transform of dimension 0 starts from the
 * coefficients of the leaves.
 */
void transformFiber(const misc::DSGLinInterpIndex& index, const misc::DSGLinInterpContext& context,
                    misc::DSGLinInterpSlotScratch& slots, size_t dim, size_t fiber);

/**
 * @brief Transform of dimension @p dim along all its fibers. After the transforms of the
 * dimensions 0, ..., d - 1 in this order, @c slots.slotWeights and @c slots.reached are set for
 * every slot.
 *
 * @param parallel Whether the caller runs it in its own parallel region (every thread of the
 *                 region calls it): then a work-shared loop that ends with its barrier, else a
 *                 plain loop of the calling thread (which must not bind to a parallel region of
 *                 the operator's caller).
 */
void transformDimension(const misc::DSGLinInterpIndex& index,
                        const misc::DSGLinInterpContext& context,
                        misc::DSGLinInterpSlotScratch& slots, size_t dim, bool parallel);

/**
 * @brief Evaluates the source function at the reached slots and sums the values times the slot
 * weights per chunk of @c constants::lin_interpolation::DSG_SUM_CHUNK_LEAVES leaves into
 * @c slots.chunkSums: with @p parallel (see @ref transformDimension), a work-shared loop over the
 * chunks without barrier, else a plain loop of the calling thread.
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues (buffer of @p scratch).
 */
template <typename ValueType>
void evaluateChunks(ValueType values, const misc::DSGLinInterpIndex& index,
                    const misc::DSGLinInterpContext& context, misc::DSGLinInterpSlotScratch& slots,
                    misc::DSGLinInterpScratch& scratch, bool parallel);

/**
 * @brief Adds the m sums of the chunks in chunk order to @p result (m values).
 */
void sumChunks(const misc::DSGLinInterpIndex& index, const misc::DSGLinInterpSlotScratch& slots,
               std::span<double> result);

}  // namespace dsg_lin_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
