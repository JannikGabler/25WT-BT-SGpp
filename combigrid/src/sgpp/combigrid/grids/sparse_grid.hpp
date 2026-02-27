#ifndef COMBIGRID_SPARSE_GRID_HPP
#define COMBIGRID_SPARSE_GRID_HPP

#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>
#include "sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp"

namespace sgpp {
namespace combigrid {

class SparseGrid {
 public:
  SparseGrid(size_t nDim);

  SparseGrid(const SGGenInstr& genInstruction);

  size_t nDim() const;

  const TensorGridCTData& getTensorGrid(size_t idx) const;

  const TensorGridCTData& getTensorGrid(const MI& mi) const;

  const std::vector<TensorGridCTData>& getTensorGrids() const;

 private:
  const size_t nDim_;
  std::vector<TensorGridCTData> tensorGridData;
};

}  // namespace combigrid
}  // namespace sgpp

#endif