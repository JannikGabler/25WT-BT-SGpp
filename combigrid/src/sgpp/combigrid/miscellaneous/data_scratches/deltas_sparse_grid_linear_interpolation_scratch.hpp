// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_scratch.hpp
 * @brief Per-thread scratch storage of the linear interpolation operator on a deltas sparse grid.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace of a thread of the linear interpolation on a deltas sparse grid
 * (see @c tools::dsg_lin_interpolation): the grid point of the evaluation pass and the dimensions
 * that set it from the sub-index of a slot.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c misc::VectorValues of the evaluation pass: the m values at a slot and the m sums of the
 * current chunk of leaves. The workspace shared by the threads is @c DSGLinInterpSlotScratch.
 */
struct DSGLinInterpScratch {
  /// @brief Accumulator slot of the sums of a chunk of leaves.
  static constexpr size_t CHUNK_ACC = 0;
  /// @brief Number of accumulator slots.
  static constexpr size_t N_ACCS = 1;

  /**
   * @brief Allocates the buffers for a deltas sparse grid of dimension @p nDim.
   * @param nDim     Number of dimensions d.
   * @param nOutputs Number of outputs m of a vector-valued source function, 0 for a scalar source
   *                 function (no @ref valueBuffer).
   */
  explicit DSGLinInterpScratch(const size_t nDim, const size_t nOutputs = 0)
      : multiDimsAndRows(2 * nDim),
        gp(nDim),
        valueBuffer(VectorValues::bufferSize(nOutputs, N_ACCS)) {}

  /// @brief The dimensions with two new coordinates at the level of the current block of slots
  /// (in ascending order: bit i of the sub-index belongs to multiDims()[i]).
  size_t* multiDims() { return multiDimsAndRows.data(); }
  /// @brief The rows of the dimensions of @ref multiDims.
  size_t* multiRows() { return multiDimsAndRows.data() + gp.size(); }

  /// @brief Storage of @ref multiDims and @ref multiRows (d entries each).
  std::vector<size_t> multiDimsAndRows;
  /// @brief Coordinates of the current slot (on the domain).
  base::DataVector gp;
  /// @brief Buffer of the @c misc::VectorValues (empty for a scalar source function).
  std::vector<double> valueBuffer;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
