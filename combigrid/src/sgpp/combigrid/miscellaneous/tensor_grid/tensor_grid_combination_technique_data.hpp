#ifndef COMBIGRID_MISC_TENSOR_GRID_CT_DATA_HPP
#define COMBIGRID_MISC_TENSOR_GRID_CT_DATA_HPP

#include <sgpp/combigrid/grids/tensor_grid.hpp>

namespace sgpp {
namespace combigrid {

struct TensorGridCTData {
  LvlMI mi;
  CTCoeffType coefficient;
  TensorGrid tensorGrid;
};

}  // namespace combigrid
}  // namespace sgpp

#endif