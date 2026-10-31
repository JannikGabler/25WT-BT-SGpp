// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_scratch.hpp
 * @brief Per-thread scratch storage reused across linear-interpolation calls.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace for the linear interpolation operator.
 *
 * Holding these buffers per thread (instead of per call) avoids repeatedly
 * allocating @c base::DataVector instances inside the inner loop.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c misc::VectorValues of the corner loop: the m values at the current corner and the m sums.
 */
struct LinInterpScratch {
  /// @brief Accumulator slot of the sums over the corners (@c misc::VectorValues::zeroAcc).
  static constexpr size_t RESULT_ACC = 0;
  /// @brief Number of accumulator slots of the corner loop.
  static constexpr size_t N_ACCS = 1;

  /**
   * @brief Allocates the buffers for a problem of dimension @p nDim.
   * @param nDim     Spatial dimension of the linear interpolation problem.
   * @param nOutputs Number of outputs m of a vector-valued source function, 0 for a scalar source
   *                 function (no @ref valueBuffer).
   */
  explicit LinInterpScratch(const size_t nDim, const size_t nOutputs = 0)
      : curGridPoint(nDim),
        neighborsPerDim(nDim),
        valueBuffer(VectorValues::bufferSize(nOutputs, N_ACCS)) {}

 public:
  /// @brief Reusable buffer for the current corner grid-point coordinates.
  base::DataVector curGridPoint;
  /// @brief Per-dimension @c (lower, upper) neighbor pairs surrounding the query point.
  std::vector<std::pair<double, double>> neighborsPerDim;
  /// @brief Buffer of the @c misc::VectorValues of a vector-valued source function (empty for a
  /// scalar one; accumulator slot @ref RESULT_ACC).
  std::vector<double> valueBuffer;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
