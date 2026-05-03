/**
 * @file sparse_grid_generation.hpp
 * @brief Top-level helpers that assemble a @c SparseGrid from a generation
 * instruction.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
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
 * @brief Populates a sparse grid from a level multi-index vector and
 * matching combination coefficients.
 *
 * Builds one tensor grid per multi-index in @p miVec (sharing per-dim
 * node lookups across tensor grids) and stores the result in @p out.
 *
 * @param genInstr Generation instruction (provides node generators, growth
 *                 functions, domain, and boundary handling).
 * @param miVec    Level multi-indices of the participating tensor grids.
 * @param coeffs   Combination coefficients aligned with @p miVec.
 * @param out      Output sparse grid; expected to already be sized to
 *                 @c miVec.nMI().
 */
void populateSG(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                const std::vector<CTCoeffType>& coeffs, SparseGrid& out);

/**
 * @brief Constructs a single tensor grid for the given level multi-index.
 *
 * @param mi       Level multi-index.
 * @param genInstr Generation instruction.
 * @return Tensor grid with the proper per-dim node counts and node positions.
 */
TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the sparse-grid populator.
 */
namespace sg_gen {

/**
 * @brief Translates a level multi-index into the per-dimension grid-point
 * counts of the corresponding tensor grid.
 *
 * Applies the per-dim @ref Lvl2GPCntFunc and the boundary level offset
 * provided by @p genInstr.
 *
 * @param mi       Level multi-index.
 * @param genInstr Generation instruction.
 */
GPMI getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr);

/**
 * @brief Returns the discrete bounding box used to iterate over all grid
 * points of a tensor grid with the given per-dim counts.
 *
 * @param gpCntPerDim Per-dim grid-point counts.
 */
misc::DiscRectBB<GPCntType> getBBForIteration(GPMI gpCntPerDim);

/**
 * @brief Concatenates the 1D node positions for every dimension of the
 * tensor grid identified by @p mi.
 *
 * @param mi          Level multi-index.
 * @param genInstr    Generation instruction.
 * @param gpCntPerDim Per-dim grid-point counts (must already match @p mi).
 * @return Flat vector of size @f$\sum_k n_k@f$ holding the per-dim node
 *         positions back-to-back.
 */
base::DataVector getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                     const GPMI& gpCntPerDim);

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
