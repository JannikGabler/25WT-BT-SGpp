// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/global_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid) {
  assert(point.size() == sparseGrid.nDim());

  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    misc::GlobalInterpScratch scratch(sparseGrid.nDim(), sparseGrid.getMaxTGSumOverGPCntsPerDim());

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      const double v = tools::global_interpolation::processTensorGrid(
          sourceFunc, normPoint, tgData.tensorGrid, genInstr, scratch);
      result += tgData.coefficient * v;
    }
  }

  sourceFunc.finishOperatorCall();

  return result;
}

void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const SparseGrid& sparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("interpolate: The size of the result is not the number of outputs");
  }
  assert(point.size() == sparseGrid.nDim());

  const base::DataVector normPoint =
      tools::normalizeDataVector(point, sparseGrid.getGenInstr().getDomain());

  tools::global_interpolation::combineTensorGrids(sourceFunc, normPoint, sparseGrid, result);

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
