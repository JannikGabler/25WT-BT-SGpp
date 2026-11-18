// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_quadrature_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/deltas_sparse_grid_weighted_sum_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>

using sgpp::combigrid::misc::DeltaWeightedSumScratch;
using sgpp::combigrid::misc::DSGQuadIndex;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::tools::dsg_quadrature::combineDeltaTensorGrids;
using sgpp::combigrid::tools::dsg_quadrature::getIndex;
using sgpp::combigrid::tools::dsg_weighted_sum::processDeltaTensorGrid;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid,
                  const QuadratureOptions& options) {
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const size_t nDim = deltasSparseGrid.nDim();

  const std::shared_ptr<const DSGQuadIndex> index =
      getIndex(deltasSparseGrid, options.useCrossCallOperatorDataCache);
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .weights1D = index->weights1D(),
                                          .coeffTrie = index->coeffTrie()};

  double result = 0.0;

#pragma omp parallel reduction(+ : result)
  {
    DeltaWeightedSumScratch scratch(nDim, deltasSparseGrid.getMaxTGSumOverGPCntsPerDim());

    // The delta tensor grids differ in size by orders of magnitude -> Dynamic scheduling
#pragma omp for schedule(dynamic) nowait
    for (size_t tgIdx = 0; tgIdx < deltasSparseGrid.nTG(); tgIdx++) {
      result += processDeltaTensorGrid(context, sourceFunc,
                                       deltasSparseGrid.getDomainTensorGrid(tgIdx), scratch);
    }
  }

  sourceFunc.finishOperatorCall();

  return genInstr.getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid,
                const std::span<double> result, const QuadratureOptions& options) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("quadrature: The size of the result is not the number of outputs");
  }

  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();

  const std::shared_ptr<const DSGQuadIndex> index =
      getIndex(deltasSparseGrid, options.useCrossCallOperatorDataCache);
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .weights1D = index->weights1D(),
                                          .coeffTrie = index->coeffTrie()};

  combineDeltaTensorGrids(context, sourceFunc, result);

  const double volume = genInstr.getVolumeOfDomain();
  for (double& value : result) {
    value *= volume;
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector quadrature(const VectorSourceFunc& sourceFunc,
                            const DeltasSparseGrid& deltasSparseGrid,
                            const QuadratureOptions& options) {
  base::DataVector result(sourceFunc.nOutputs());
  quadrature(sourceFunc, deltasSparseGrid, result, options);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
