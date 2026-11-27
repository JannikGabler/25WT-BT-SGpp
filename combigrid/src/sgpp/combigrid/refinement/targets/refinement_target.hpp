// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_target.hpp
 * @brief Interface of the operators that guide a dimension-adaptive refinement.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief The operator (with its arguments, e.g. the source function) whose combination a
 * dimension-adaptive refinement improves: an operator applied to one tensor grid.
 *
 * A combination-technique operator combines per-tensor-grid results,
 * @f$\mathrm{Op}(f) \approx \sum_{\vec{\ell}} c_{\vec{\ell}}\, \mathrm{Op}_{\vec{\ell}}(f)@f$. A
 * target computes @f$\mathrm{Op}_{\vec{\ell}}(f) \in \mathbb{R}^k@f$ for single tensor grids
 * (@ref estimate); the refinement forms the hierarchical increments
 * @f$\Delta_{\vec{\ell}} = \sum_{\vec{z} \in \{0, 1\}^d} (-1)^{|\vec{z}|}\,
 * \mathrm{Op}_{\vec{\ell} - \vec{z}}(f)@f$ from them, ranks the candidates by their norm and
 * combines the results. Any operator that can be applied to a single tensor grid can guide a
 * refinement by implementing this interface; the library provides the quadrature
 * (@c QuadratureRefinementTarget), global interpolation (@c GlobalInterpolationRefinementTarget)
 * and multilinear interpolation (@c LinearInterpolationRefinementTarget) at given points.
 *
 * Calling sequence per refinement call: @ref beginRefinementCall (serial), then @ref estimate
 * concurrently from up to @c nWorkers threads (each with its own worker number), then
 * @ref endRefinementCall (serial). @ref nValues must not change while a refinement object uses
 * the target.
 */
class RefinementTarget {
 public:
  virtual ~RefinementTarget() = default;

  /// @brief Returns the number @f$k@f$ of values per tensor grid.
  virtual size_t nValues() const = 0;

  /**
   * @brief Prepares a refinement call (called before any @ref estimate of the call).
   *
   * @param setup         Setup of the sparse grid to refine.
   * @param unitNodeTable 1D nodes on the unit interval of every (dimension, level) the call may
   *                      use. Its rows do not move during the call, but rows are generated during
   *                      the call: a row may be read only from @ref estimate of a tensor grid
   *                      with that level (per-row data is therefore computed on first use).
   * @param nWorkers      Number of worker threads (worker numbers 0 ... nWorkers - 1).
   */
  virtual void beginRefinementCall(const SparseGridSetup& setup, const NodeTable& unitNodeTable,
                                   size_t nWorkers) = 0;

  /**
   * @brief Computes the @ref nValues values of one tensor grid.
   *
   * Called concurrently by distinct workers; calls with the same @p worker are sequential.
   *
   * @param domainTG Tensor grid with its nodes on the domain (the evaluation points).
   * @param unitTG   The same tensor grid with its nodes on the unit interval.
   * @param worker   Number of the calling worker.
   * @param out      The @ref nValues values.
   */
  virtual void estimate(const TensorGridView& domainTG, const TensorGridView& unitTG, size_t worker,
                        std::span<double> out) = 0;

  /**
   * @brief Maps a combination of tensor-grid values (the estimate of the grid, or an increment)
   * to the target's result in place, e.g. the quadrature multiplies by the volume of the domain.
   * The default keeps the values.
   */
  virtual void finishCombination(std::span<double> /*values*/) const {}

  /// @brief Ends a refinement call (serial), e.g. notifies the source function's cache.
  virtual void endRefinementCall() = 0;
};

}  // namespace combigrid
}  // namespace sgpp
