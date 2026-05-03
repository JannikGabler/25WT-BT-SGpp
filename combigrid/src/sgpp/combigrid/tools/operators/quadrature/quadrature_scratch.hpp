/**
 * @file quadrature_scratch.hpp
 * @brief Per-thread scratch storage reused across quadrature calls.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Pre-allocated workspace for the quadrature operator.
 *
 * Holds the buffers needed to integrate one tensor grid: a grid-point
 * coordinate buffer, the concatenated 1D weight vector for every
 * dimension, and the per-dimension quadrature-rule pointers.
 */
struct QuadScratch {
  /**
   * @brief Allocates the buffers.
   * @param nDim                  Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the
   *                              tensor grids that will be integrated; sized
   *                              once to avoid reallocations.
   */
  QuadScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim)
      : gp(nDim), weights(maxTGSumOfGPCntsPerDim), quadRules(nDim) {}

 public:
  /// @brief Reusable buffer for the current grid-point coordinates.
  base::DataVector gp;
  /// @brief Concatenated 1D quadrature weights (per-dimension blocks back-to-back).
  base::DataVector weights;
  /// @brief Per-dimension quadrature-rule pointers; non-owning.
  std::vector<QuadRule*> quadRules;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
