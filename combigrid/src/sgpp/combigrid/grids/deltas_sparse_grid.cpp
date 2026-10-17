#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>

namespace sgpp {
namespace combigrid {

/// @name Constructors
/// @{

DeltasSparseGrid::DeltasSparseGrid(const SGGenInstr& genInstr)
    : SparseGridBase(genInstr.nDim()), genInstr(genInstr.clone()) {
  const auto [miVec, coeffs] = genInstr.genFullMIVecWithCoeffs();

  tools::populateDeltasSG(*this, genInstr, miVec, coeffs);
}

/// @}

/// @name Getters
/// @{

const SGGenInstr& DeltasSparseGrid::getGenInstr() const noexcept { return *genInstr; }

/// @}

/// @name Setters
/// @{

// void SparseGrid::setGenInstr(const SGGenInstr& genInstr) {
//   assert(genInstr.nDim() == nDim_);

//   this->genInstr = genInstr.clone();
// }

// void SparseGrid::setGenInstr(std::shared_ptr<const SGGenInstr>&& genInstr) {
//   assert(genInstr->nDim() == nDim_);

//   this->genInstr = std::move(genInstr);
// }

/// @}

}  // namespace combigrid
}  // namespace sgpp