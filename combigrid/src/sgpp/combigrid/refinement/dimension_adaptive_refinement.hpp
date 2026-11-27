// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dimension_adaptive_refinement.hpp
 * @brief Dimension-adaptive refinement of a sparse grid in place (Gerstner-Griebel).
 */
#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/refinement/refinement_options.hpp>
#include <sgpp/combigrid/refinement/refinement_statistics.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

// Forward declaration
namespace misc {
struct RefinementState;
class RefinementNodeStorage;
}  // namespace misc

/**
 * @brief Refines a @c SparseGrid dimension-adaptively in place: adds tensor grids where the
 * hierarchical increments of a target operator are large (Gerstner-Griebel), so that afterwards
 * the grid is an ordinary sparse grid for every operator.
 *
 * The grid's index set @f$I@f$ (downwards closed, including the multi-indices with coefficient 0)
 * is split into old multi-indices (refined: their admissible forward neighbours are in @f$I@f$)
 * and active ones. Every multi-index has the increment
 * @f$\Delta_{\vec{\ell}} = \sum_{\vec{z} \in \{0,1\}^d} (-1)^{|\vec{z}|}
 * Q_{\vec{\ell} - \vec{z}}@f$ of the target's values @f$Q@f$ (@c RefinementTarget), its gain
 * @f$\|\Delta_{\vec{\ell}}\|_\infty@f$ and its cost (grid points). A decision accepts the active
 * multi-indices of highest priority (@c RefinementOptions::indicator; all with priority at least
 * @c batchThreshold times the best, at most @c maxBatchSize), makes them old and evaluates their
 * forward neighbours whose backward neighbours are all old; once evaluated, these join the grid
 * as active multi-indices (everything evaluated is used). The error estimate is the sum of the
 * gains of the active multi-indices.
 *
 * The refinement object keeps its state between calls: later @ref refine calls (also after
 * @ref setOptions, e.g. another indicator) continue without evaluating a tensor grid twice. On a
 * grid it has not refined (or one that changed since), a call starts by evaluating every tensor
 * grid of the grid's index set; the multi-indices without all forward neighbours are active.
 *
 * During a call the grid must not be used; afterwards its tensor grids are stored in
 * colexicographic order of their multi-indices (the order @c MIVecSGGenInstr gives the same index
 * set, so the operators give the same results on both), its cross-call operator data is dropped,
 * and views or iterators into it are invalid. Tensor grids are evaluated in parallel (OpenMP
 * threads); with @c RefinementSchedule::Synchronized the result does not depend on the number of
 * threads.
 *
 * The grid and the target are referenced: both must outlive the object.
 */
class DimensionAdaptiveRefinement {
 public:
  /**
   * @param grid    Grid to refine.
   * @param target  Target operator (its number of values must not change).
   * @param options Options of the calls (see @ref setOptions).
   */
  DimensionAdaptiveRefinement(SparseGrid& grid, RefinementTarget& target,
                              const RefinementOptions& options = RefinementOptions());

  ~DimensionAdaptiveRefinement();

  DimensionAdaptiveRefinement(const DimensionAdaptiveRefinement&) = delete;
  DimensionAdaptiveRefinement& operator=(const DimensionAdaptiveRefinement&) = delete;

  /**
   * @brief Refines until a stopping criterion of the options holds.
   * @return Statistics of the call.
   * @throws std::invalid_argument if no tolerance or limit is set (tolerances 0, budget, index-set
   * limit and time unlimited). Exceptions of the target are rethrown after the grid has been
   * handed back with the results evaluated until then.
   */
  RefinementStatistics refine();

  /**
   * @brief Performs one decision (always synchronized) and evaluates its candidates; on a grid
   * the object has not refined, the grid's tensor grids are evaluated first.
   * @return Statistics of the call (@c RefinementStopReason::StepDone, or why no decision was
   * possible).
   */
  RefinementStatistics step();

  /// @brief Sets the options of the next calls.
  void setOptions(const RefinementOptions& options);

  /// @brief Returns the options of the next calls.
  const RefinementOptions& getOptions() const noexcept;

  /**
   * @brief Returns the target's combination on the grid after the last call (e.g. the quadrature;
   * empty before the first call).
   */
  std::span<const double> getEstimate() const noexcept;

  /// @brief Returns the error estimate after the last call (the sum of the active gains).
  double getErrorEstimate() const noexcept;

 private:
  /// @brief Runs one call with at most @p maxDecisions decisions.
  RefinementStatistics runCall(const RefinementOptions& options, size_t maxDecisions);

  /// @brief Runs the engine of a call; returns its first exception (the statistics are filled).
  std::exception_ptr runEngine(const RefinementOptions& options, size_t maxDecisions,
                               misc::RefinementNodeStorage& nodes,
                               RefinementStatistics& statistics);

  SparseGrid& grid_;                              ///< Grid being refined.
  RefinementTarget& target_;                      ///< Target operator.
  RefinementOptions options_;                     ///< Options of the next calls.
  std::unique_ptr<misc::RefinementState> state_;  ///< State kept between calls.
  std::vector<double> estimate_;                  ///< Combination after the last call.
  double errorEstimate_ = 0.0;                    ///< Error estimate after the last call.
};

/**
 * @brief Refines @p grid in place with a temporary @c DimensionAdaptiveRefinement (see there):
 * one call of @c DimensionAdaptiveRefinement::refine.
 */
RefinementStatistics refine(SparseGrid& grid, RefinementTarget& target,
                            const RefinementOptions& options);

}  // namespace combigrid
}  // namespace sgpp
