// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>

namespace sgpp {
namespace combigrid {

TensorGridCTData TensorGridCTDataView::toTensorGridCTData() const {
  return {mi.toMI(), coefficient, tensorGrid.toTensorGrid()};
}

bool TensorGridCTDataView::operator==(const TensorGridCTDataView& other) const {
  return mi == other.mi && coefficient == other.coefficient && tensorGrid == other.tensorGrid;
}

}  // namespace combigrid
}  // namespace sgpp
