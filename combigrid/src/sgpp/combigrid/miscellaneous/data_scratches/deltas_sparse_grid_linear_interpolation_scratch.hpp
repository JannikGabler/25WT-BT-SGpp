// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dsg_lin_interp_scratch.hpp
 * @brief Per-thread scratch storage of the linear interpolation operator on a deltas sparse grid.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Pre-allocated workspace of a thread of the linear interpolation on a deltas sparse grid
 * (see @c tools::dsg_lin_interpolation): the state of the walk over the corners of a tensor grid,
 * the corner weights of the tensor grid (as @c misc::LinInterpScratch) and the grid point of the
 * evaluation pass.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c misc::VectorValues of the passes: the m values of a slot, the m sums of the current tensor
 * grid and the m sums of the thread.
 */
struct DSGLinInterpScratch {
  /// @brief Accumulator slot of the sums over the corners of a tensor grid.
  static constexpr size_t TG_ACC = 0;
  /// @brief Accumulator slot of the combination of the tensor grids of the thread.
  static constexpr size_t THREAD_ACC = 1;
  /// @brief Number of accumulator slots.
  static constexpr size_t N_ACCS = 2;

  /**
   * @brief Allocates the buffers for a deltas sparse grid of dimension @p nDim.
   * @param nDim     Number of dimensions d.
   * @param nOutputs Number of outputs m of a vector-valued source function, 0 for a scalar source
   *                 function (no @ref valueBuffer).
   */
  explicit DSGLinInterpScratch(const size_t nDim, const size_t nOutputs = 0)
      : node(nDim + 1),
        sub(nDim + 1),
        cornerIds(2 * nDim),
        factors(2 * nDim),
        tableDim(std::min(nDim, constants::lin_interpolation::WEIGHT_TABLE_MAX_DIM)),
        weightTable(size_t{1} << tableDim),
        highFactors(nDim - tableDim),
        multiDims(nDim),
        multiRows(nDim),
        gp(nDim),
        valueBuffer(VectorValues::bufferSize(nOutputs, N_ACCS)) {}

  /// @brief Walk state: trie node after the dimensions d - 1, ..., j (entry d: the root; entry 0:
  /// the leaf of the current corner).
  std::vector<uint32_t> node;
  /// @brief Walk state: sub-index of the slot after the dimensions d - 1, ..., j.
  std::vector<size_t> sub;
  /// @brief Corner ids of the current tensor grid: lower / upper neighbor of dimension j at 2j /
  /// 2j + 1.
  std::vector<uint32_t> cornerIds;
  /// @brief 1D weights of the current tensor grid: lower / upper neighbor of dimension k at 2k /
  /// 2k + 1.
  std::vector<double> factors;
  /// @brief Number of dimensions covered by @ref weightTable: min(d,
  /// @c constants::lin_interpolation::WEIGHT_TABLE_MAX_DIM).
  size_t tableDim;
  /// @brief Corner weight table of the first @ref tableDim dimensions (as
  /// @c misc::LinInterpScratch::weightTable).
  std::vector<double> weightTable;
  /// @brief 1D weights of the dimensions @ref tableDim ... d - 1 of the current block of corners.
  std::vector<double> highFactors;
  /// @brief Evaluation pass: the dimensions with two new coordinates at the level of the current
  /// block of slots (in ascending order: bit i of the sub-index belongs to multiDims[i]).
  std::vector<size_t> multiDims;
  /// @brief Evaluation pass: the rows of the dimensions of @ref multiDims.
  std::vector<size_t> multiRows;
  /// @brief Evaluation pass: coordinates of the current slot (on the domain).
  base::DataVector gp;
  /// @brief Buffer of the @c misc::VectorValues (empty for a scalar source function).
  std::vector<double> valueBuffer;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
