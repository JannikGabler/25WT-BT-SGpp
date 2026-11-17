// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace misc {

DSGLinInterpSlotScratch::DSGLinInterpSlotScratch(const DSGLinInterpIndex& index,
                                                 const size_t nOutputs)
    : reached(std::make_unique_for_overwrite<bool[]>(index.nSlots())) {
  const size_t nChunkSums = nChunks(index) * nOutputs;
  doubles = std::make_unique_for_overwrite<double[]>(index.nSlots() + nChunkSums);
  slotWeights = std::span<double>(doubles.get(), index.nSlots());
  chunkSums = std::span<double>(doubles.get() + index.nSlots(), nChunkSums);
}

size_t DSGLinInterpSlotScratch::nChunks(const DSGLinInterpIndex& index) {
  const size_t chunkLeaves = constants::lin_interpolation::DSG_SUM_CHUNK_LEAVES;
  return (index.nLeaves() + chunkLeaves - 1) / chunkLeaves;
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
