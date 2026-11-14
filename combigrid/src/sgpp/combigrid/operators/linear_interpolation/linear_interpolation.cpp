// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <omp.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <vector>

using LinInterpScratch = sgpp::combigrid::misc::LinInterpScratch;

namespace sgpp {
namespace combigrid {

double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid) {
  assert(point.size() == sparseGrid.nDim());

  const size_t nDim = sparseGrid.nDim();

  // The cell is computed in normalized coordinates, with the nodes on the unit interval
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  const NodeTable& unitNodes = tools::node_table_generation::getUnitNodeTable(sparseGrid);
  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    LinInterpScratch scratch(nDim);

#pragma omp for schedule(static) nowait
    for (size_t tgIdx = 0; tgIdx < sparseGrid.nTG(); tgIdx++) {
      const TensorGridCTDataView tgData = sparseGrid.getTensorGrid(tgIdx);
      result += tgData.coefficient *
                tools::lin_interpolation::processTensorGrid(
                    sourceFunc, normPoint, TensorGridView(tgData.mi, unitNodes), genInstr, scratch);
    }
  }

  sourceFunc.finishOperatorCall();

  return result;
}

void interpolateLinear(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const SparseGrid& sparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument(
        "interpolateLinear: The size of the result is not the number of outputs");
  }
  assert(point.size() == sparseGrid.nDim());

  const size_t nDim = sparseGrid.nDim();
  const size_t nOutputs = result.size();
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  const NodeTable& unitNodes = tools::node_table_generation::getUnitNodeTable(sparseGrid);

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    LinInterpScratch scratch(nDim, nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

#pragma omp for schedule(static) nowait
    for (size_t tgIdx = 0; tgIdx < sparseGrid.nTG(); tgIdx++) {
      const TensorGridCTDataView tgData = sparseGrid.getTensorGrid(tgIdx);
      tools::lin_interpolation::processTensorGrid(
          sourceFunc, normPoint, TensorGridView(tgData.mi, unitNodes), genInstr, scratch, tgResult);
      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgData.coefficient * tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector interpolateLinear(const VectorSourceFunc& sourceFunc,
                                   const base::DataVector& point, const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  interpolateLinear(sourceFunc, point, sparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
