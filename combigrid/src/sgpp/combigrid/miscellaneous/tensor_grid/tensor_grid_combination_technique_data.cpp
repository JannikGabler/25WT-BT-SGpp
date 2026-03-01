#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>

namespace sgpp {
namespace combigrid {

bool TensorGridCTData::operator==(const TensorGridCTData& other) const {
  return mi == other.mi && coefficient == other.coefficient && tensorGrid == other.tensorGrid;
}

}  // namespace combigrid
}  // namespace sgpp