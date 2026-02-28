#ifndef COMBIGRID_SPARSE_GRID_HPP
#define COMBIGRID_SPARSE_GRID_HPP

#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class SparseGrid {
 public:
  /**********
  Constructor
  **********/
  SparseGrid(size_t nDim);

  SparseGrid(size_t nDim, size_t nTG);

  SparseGrid(const SGGenInstr& genInstruction);

  /*****
  Getter
  *****/
  size_t nDim() const;

  size_t nTG() const;

  const TensorGridCTData& getTensorGrid(size_t idx) const;

  const TensorGridCTData& getTensorGrid(const LvlMI& mi) const;

  const std::vector<TensorGridCTData>& getTensorGrids() const;

  /*****
  Setter
  *****/
  void addTensorGrid(const TensorGridCTData& tg);

  void addTensorGrid(TensorGridCTData&& tg);

  void setTensorGrid(size_t idx, const TensorGridCTData& tg);

  void setTensorGrid(size_t idx, TensorGridCTData&& tg);

 private:
  const size_t nDim_;
  std::vector<TensorGridCTData> tensorGridData;
};

}  // namespace combigrid
}  // namespace sgpp

#endif