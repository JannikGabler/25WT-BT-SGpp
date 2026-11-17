// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_slot_scratch.hpp
 * @brief Defines @c DSGLinInterpSlotScratch, the per-slot values that the passes of one call of
 * the linear interpolation operator on a deltas sparse grid compute (shared by all threads).
 */
#pragma once

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Workspace of one call of the linear interpolation on a deltas sparse grid that the passes
 * write (see @c tools::dsg_lin_interpolation), one per call and shared by all threads: within a
 * pass, every slot and every chunk is written by one thread only (the fibers of a dimension and
 * the chunks of leaves are disjoint), and the barrier after the pass publishes it to the next.
 *
 * Per slot: @ref slotWeights, the weight of the slot's corner in the combination technique
 * interpolant, and @ref reached, true if the slot is a corner of some combination tensor grid (the
 * points that are evaluated); both are set in place by the transforms of the dimensions. Per chunk
 * of leaves: @ref chunkSums, the m sums of the evaluation pass. The per-thread workspace is
 * @c DSGLinInterpScratch.
 */
struct DSGLinInterpSlotScratch {
  /**
   * @brief Allocates the workspace of a call (uninitialized; every pass writes what it reads
   * later).
   * @param index    Index of the deltas sparse grid.
   * @param nOutputs Number of outputs m.
   */
  DSGLinInterpSlotScratch(const DSGLinInterpIndex& index, size_t nOutputs);

  /// @brief Returns the number of chunks of leaves of the final sum (see
  /// @c constants::lin_interpolation::DSG_SUM_CHUNK_LEAVES).
  static size_t nChunks(const DSGLinInterpIndex& index);

  std::unique_ptr<bool[]> reached;  ///< Per slot: whether it is evaluated.
  /// @brief Storage of @ref slotWeights and @ref chunkSums (one allocation).
  std::unique_ptr<double[]> doubles;
  std::span<double> slotWeights;  ///< Per slot: weight of its corner.
  std::span<double> chunkSums;    ///< Per chunk of leaves: m sums.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
