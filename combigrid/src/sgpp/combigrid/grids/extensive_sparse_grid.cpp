
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>

namespace sgpp {
namespace combigrid {

ExtensiveSparseGrid::ExtensiveSparseGrid(const SGGenInstr& genInstr) : SparseGrid(genInstr.nDim()) {
  this->genInstr = genInstr.clone();

  const std::pair<LvlMIVec, std::vector<CTCoeffType>> p = genInstr.genReducedMIVecWithCoeffs();

  tensorGridData.resize(p.first.nMI());
  tools::populateSG(genInstr, p.first, p.second, *this);
}

}  // namespace combigrid
}  // namespace sgpp