/**
 * @file sparse_grid_generation.hpp
 * @brief Top-level helpers that assemble a @c SparseGrid from a generation
 * instruction.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * TODO: Document
 * Important requirement: The tensor grids represented through miVec and coeffs must be ordered such
 * that all tensor grids with a CT coeff of zero are at the front of the vector.
 */
void populateExtensiveSG(ExtensiveSparseGrid& sg, const SGGenInstr& genInstr, const LvlMIVec& miVec,
                         const std::vector<CTCoeffType>& coeffs);

/// @brief Internal implementation details of the extensive-sparse-grid generation.
namespace extensive_sparse_grid_generation {

/**
 * TODO: Document
 */
size_t partitionForExtensiveSG(LvlMIVec& miVec, std::vector<CTCoeffType>& coeffs);

/**
 * TODO: Document
 */
bool areTGsCorrectlyPartitionedForExtensiveSG(const std::vector<CTCoeffType>& coeffs);

}  // namespace extensive_sparse_grid_generation

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
