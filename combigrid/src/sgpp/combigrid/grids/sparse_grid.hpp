#ifndef COMBIGRID_SPARSE_GRID_HPP
#define COMBIGRID_SPARSE_GRID_HPP

#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>
#include "sgpp/combigrid/node_generation_functions/node_generation_function.hpp"

namespace sgpp {
namespace combigrid {

class SparseGrid {
  using iterator = std::vector<TensorGridCTData>::iterator;
  using const_iterator = std::vector<TensorGridCTData>::const_iterator;
  using reverse_iterator = std::vector<TensorGridCTData>::reverse_iterator;
  using const_reverse_iterator = std::vector<TensorGridCTData>::const_reverse_iterator;

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

  /*
  Not optimized (O(n) runtime)
  */
  const_iterator getTensorGrid(const LvlMI& mi) const;

  const std::vector<TensorGridCTData>& getTensorGrids() const;

  const std::vector<NodeGenFunc>& getNodeGenFuncs() const;

  /*****
  Setter
  *****/
  void addTensorGrid(const TensorGridCTData& tg);

  void addTensorGrid(TensorGridCTData&& tg);

  void setTensorGrid(size_t idx, const TensorGridCTData& tg);

  void setTensorGrid(size_t idx, TensorGridCTData&& tg);

  void setNodeGenFuncs(const std::vector<NodeGenFunc>& nodeGenFuncs);

  void setNodeGenFuncs(std::vector<NodeGenFunc>&& nodeGenFuncs);

  /*******
  Iterator
  *******/
  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  /*******
  Operator
  *******/

  /*
  Compares the content of this sg with the other sparse grid.
  This operation is inefficient and should be used carefully.
  */
  bool operator==(const SparseGrid& other) const;

 private:
  const size_t nDim_;
  std::vector<TensorGridCTData> tensorGridData;
  std::vector<NodeGenFunc> nodeGenFuncs;
};

}  // namespace combigrid
}  // namespace sgpp

#endif