#ifndef COMBIGRID_SPARSE_GRID_HPP
#define COMBIGRID_SPARSE_GRID_HPP

#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class SparseGrid {
 public:
  SparseGrid(const SGGenInstr& genInstruction);

  size_t nDim() const;

  const TensorGrid& getTensorGrid(size_t idx) const;

  const TensorGrid& getTensorGrid(const MI& mi) const;

  const std::vector<TensorGrid>& getTensorGrids() const;

 private:
  const size_t nDim_;
  std::vector<TensorGrid> tensorGrids;
};

}  // namespace combigrid
}  // namespace sgpp

#endif