/**
 * @file quadrature_scratch.hpp
 * @brief Per-thread scratch storage reused across quadrature calls.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace for the quadrature operator.
 *
 * Holds the buffers needed to integrate one tensor grid: the
 * concatenated 1D weight vector for every dimension, and the
 * per-dimension quadrature-rule pointers.
 */
struct QuadratureScratch {
  /**
   * @brief Allocates the buffers.
   * @param nDim                  Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the
   *                              tensor grids that will be integrated; sized
   *                              once to avoid reallocations.
   */
  QuadratureScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim)
      : weights1D(maxTGSumOfGPCntsPerDim), quadRules(nDim) {}

 public:
  /// @brief Concatenated 1D quadrature weights (per-dimension blocks back-to-back).
  base::DataVector weights1D;
  /// @brief Per-dimension quadrature-rule pointers; non-owning.
  std::vector<QuadRule*> quadRules;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
