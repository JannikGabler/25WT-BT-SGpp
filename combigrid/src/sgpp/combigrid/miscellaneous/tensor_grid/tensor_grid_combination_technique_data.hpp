#pragma once

#include <sgpp/combigrid/grids/tensor_grid.hpp>

namespace sgpp {
namespace combigrid {

struct TensorGridCTData {
  LvlMI mi;
  CTCoeffType coefficient;
  TensorGrid tensorGrid;

  bool operator==(const TensorGridCTData& other) const;
};

}  // namespace combigrid
}  // namespace sgpp