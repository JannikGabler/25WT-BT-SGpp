// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_scratch.hpp
 * @brief Per-thread scratch storage reused across linear-interpolation calls.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
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
 * The corner data of a tensor grid share one buffer (@ref cornerData, one allocation): the
 * corner weight table (@ref weightTable), the bracketing coordinates on the domain
 * (@ref bracket), the 1D weights (@ref factors) and the factors of the dimensions beyond the
 * table for the current block of corners (@ref highFactors).
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
        valueBuffer(VectorValues::bufferSize(nOutputs, N_ACCS)),
        tableDim_(std::min(nDim, constants::lin_interpolation::WEIGHT_TABLE_MAX_DIM)),
        cornerData((size_t{1} << tableDim_) + 5 * nDim - tableDim_) {}

  /// @brief Number of dimensions covered by the corner weight table: min(d,
  /// @c constants::lin_interpolation::WEIGHT_TABLE_MAX_DIM).
  size_t tableDim() const { return tableDim_; }

  /// @brief Corner weight table: entry j (j < 2^@ref tableDim) is the product of the 1D weights
  /// of dimensions 0 ... tableDim - 1 of the corner whose bit k selects the upper neighbor in
  /// dimension k.
  double* weightTable() { return cornerData.data(); }

  /// @brief Bracketing coordinates on the domain: lower / upper neighbor of dimension k at
  /// 2k / 2k + 1.
  double* bracket() { return weightTable() + (size_t{1} << tableDim_); }

  /// @brief 1D weights of the lower / upper neighbor of dimension k at 2k / 2k + 1.
  double* factors() { return bracket() + 2 * neighborsPerDim.size(); }

  /// @brief 1D weights of dimensions @ref tableDim ... d - 1 of the current block of corners.
  double* highFactors() { return factors() + 2 * neighborsPerDim.size(); }

  /// @brief Reusable buffer for the current corner grid-point coordinates (on the domain).
  base::DataVector curGridPoint;
  /// @brief Per-dimension @c (lower, upper) neighbor pairs surrounding the query point.
  std::vector<std::pair<double, double>> neighborsPerDim;
  /// @brief Buffer of the @c misc::VectorValues of a vector-valued source function (empty for a
  /// scalar one; accumulator slot @ref RESULT_ACC).
  std::vector<double> valueBuffer;

 private:
  /// @brief Number of dimensions covered by the corner weight table.
  size_t tableDim_;
  /// @brief Corner data (see the class description).
  std::vector<double> cornerData;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
