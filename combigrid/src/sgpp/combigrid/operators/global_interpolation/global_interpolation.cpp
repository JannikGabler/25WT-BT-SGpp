// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <vector>

using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::tools::normalizeDataVector;
using sgpp::combigrid::tools::global_interpolation::estimateTensorGrids;
using sgpp::combigrid::tools::numerics::compensatedDotProduct;
using sgpp::combigrid::tools::numerics::compensatedDotProducts;

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid) {
  if (point.size() != sparseGrid.nDim()) {
    throw std::invalid_argument(
        "The size of the point doesn't match the dimensionality of the sparse grid.");
  }

  // The basis values are computed in normalized coordinates (see tools::global_interpolation)
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = normalizeDataVector(point, genInstr.getDomain());

  // One value per tensor grid, stored at its index
  const std::vector<double> tgValues = estimateTensorGrids(sourceFunc, normPoint, sparseGrid);
  const double result = compensatedDotProduct<double>(sparseGrid.getCoefficients(), tgValues);

  sourceFunc.finishOperatorCall();

  return result;
}

void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const SparseGrid& sparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("The size of the result is not the number of outputs");
  } else if (point.size() != sparseGrid.nDim()) {
    throw std::invalid_argument(
        "The size of the point doesn't match the dimensionality of the sparse grid.");
  }

  const base::DataVector normPoint =
      normalizeDataVector(point, sparseGrid.getGenInstr().getDomain());

  // m values per tensor grid, stored at its index
  const FlatNestedVec<double, 2> tgValues = estimateTensorGrids(sourceFunc, normPoint, sparseGrid);
  compensatedDotProducts<double>(sparseGrid.getCoefficients(), tgValues, result);

  sourceFunc.finishOperatorCall();
}

base::DataVector interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                             const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  interpolate(sourceFunc, point, sparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
