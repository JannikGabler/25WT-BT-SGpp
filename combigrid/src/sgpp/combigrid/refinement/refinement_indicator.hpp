// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_indicator.hpp
 * @brief The indicator that ranks the candidates of the dimension-adaptive refinement.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Ranks the active multi-indices of a dimension-adaptive refinement (which one is
 * refined next; see @c DimensionAdaptiveRefinement).
 *
 * With @f$\Delta_{\vec{\ell}}@f$ the hierarchical increment of the refinement target on
 * multi-index @f$\vec{\ell}@f$ (k values) and @f$n_{\vec{\ell}}@f$ the number of grid points of
 * its tensor grid:
 */
enum class RefinementIndicator {
  /// Gain @f$\|\Delta_{\vec{\ell}}\|_\infty@f$: the largest increment first.
  Gain,
  /// Cost @f$n_{\vec{\ell}}@f$: the cheapest tensor grid first (grows the grid breadth-first).
  Cost,
  /// Profit @f$\|\Delta_{\vec{\ell}}\|_\infty / n_{\vec{\ell}}@f$: the largest gain per grid point
  /// first.
  Profit
};

}  // namespace combigrid
}  // namespace sgpp
