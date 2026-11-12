// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/deltas_sparse_grid_global_interpolation_tools.hpp>
#include <span>
#include <stdexcept>

using sgpp::combigrid::misc::DSGGlobalInterpIndex;
using sgpp::combigrid::tools::dsg_global_interpolation::evaluateInterpolant;
using sgpp::combigrid::tools::dsg_global_interpolation::getIndex;
using sgpp::combigrid::tools::dsg_global_interpolation::runInParallel;

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const DeltasSparseGrid& deltasSparseGrid) {
  assert(point.size() == deltasSparseGrid.nDim());
  double result = 0.0;

  if (deltasSparseGrid.nDim() > 0) {
    const DSGGlobalInterpIndex& index = getIndex(deltasSparseGrid);
    const HyperCubeArea& domain = deltasSparseGrid.getGenInstr().getDomain();
    result = evaluateInterpolant(index, deltasSparseGrid, tools::normalizeDataVector(point, domain),
                                 sourceFunc, runInParallel(index));
  }

  sourceFunc.finishOperatorCall();

  return result;
}

void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const DeltasSparseGrid& deltasSparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("interpolate: The size of the result is not the number of outputs");
  }
  assert(point.size() == deltasSparseGrid.nDim());
  std::ranges::fill(result, 0.0);

  if (deltasSparseGrid.nDim() > 0) {
    const DSGGlobalInterpIndex& index = getIndex(deltasSparseGrid);
    const HyperCubeArea& domain = deltasSparseGrid.getGenInstr().getDomain();
    evaluateInterpolant(index, deltasSparseGrid, tools::normalizeDataVector(point, domain),
                        sourceFunc, runInParallel(index), result);
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                             const DeltasSparseGrid& deltasSparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  interpolate(sourceFunc, point, deltasSparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
