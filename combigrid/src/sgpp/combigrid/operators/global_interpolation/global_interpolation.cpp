// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid) {
  assert(point.size() == sparseGrid.nDim());

  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  double result = 0;

#pragma omp parallel for reduction(+ : result) schedule(guided)
  for (const TensorGridCTData& tgData : sparseGrid) {
    const double v =
        tools::global_interpolation::processTensorGrid(sourceFunc, normPoint, tgData, genInstr);
    result += tgData.coefficient * v;
  }

  return result;
}

void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const SparseGrid& sparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("interpolate: The size of the result is not the number of outputs");
  }
  assert(point.size() == sparseGrid.nDim());

  const size_t nOutputs = result.size();
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      tools::global_interpolation::processTensorGrid(sourceFunc, normPoint, tgData, genInstr,
                                                     tgResult);

      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgData.coefficient * tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }
}

base::DataVector interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                             const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  interpolate(sourceFunc, point, sparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
