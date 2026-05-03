/**
 * @file linear_interpolation_scratch.hpp
 * @brief Per-thread scratch storage reused across linear-interpolation calls.
 */
#pragma once

#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Pre-allocated workspace for the linear interpolation operator.
 *
 * Holding these buffers per thread (instead of per call) avoids repeatedly
 * allocating @c base::DataVector instances inside the inner loop.
 */
struct LinInterpScratch {
  /**
   * @brief Allocates the buffers for a problem of dimension @p nDim.
   * @param nDim Spatial dimension of the linear interpolation problem.
   */
  LinInterpScratch(const size_t nDim) : curGridPoint(nDim), neighborsPerDim(nDim) {}

 public:
  /// @brief Reusable buffer for the current corner grid-point coordinates.
  base::DataVector curGridPoint;
  /// @brief Per-dimension @c (lower, upper) neighbor pairs surrounding the query point.
  std::vector<std::pair<double, double>> neighborsPerDim;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
