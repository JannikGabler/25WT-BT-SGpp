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
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature_options.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <vector>

using sgpp::combigrid::misc::DSGQuadIndex;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::tools::dsg_quadrature::estimateDeltaTensorGrids;
using sgpp::combigrid::tools::dsg_quadrature::getIndex;
using sgpp::combigrid::tools::numerics::compensatedSum;
using sgpp::combigrid::tools::numerics::compensatedSums;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid,
                  const DeltasSparseGridQuadratureOptions& options) {
  const std::shared_ptr<const DSGQuadIndex> index =
      getIndex(deltasSparseGrid, options.useCrossCallOperatorDataCache);
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .weights1D = index->weights1D(),
                                          .coeffTrie = index->coeffTrie()};

  // One value per delta tensor grid, stored at its index
  const std::vector<double> tgValues = estimateDeltaTensorGrids(context, sourceFunc);
  const double result = compensatedSum<double>(tgValues);

  sourceFunc.finishOperatorCall();

  return deltasSparseGrid.getSetup().getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid,
                const std::span<double> result, const DeltasSparseGridQuadratureOptions& options) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("The size of the result is not the number of outputs");
  }

  const std::shared_ptr<const DSGQuadIndex> index =
      getIndex(deltasSparseGrid, options.useCrossCallOperatorDataCache);
  const GenericDSGOperatorContext context{.deltasSparseGrid = deltasSparseGrid,
                                          .weights1D = index->weights1D(),
                                          .coeffTrie = index->coeffTrie()};

  // One value per delta tensor grid, stored at its index
  const misc::FlatNestedVec<double, 2> tgValues = estimateDeltaTensorGrids(context, sourceFunc);
  compensatedSums<double>(tgValues, result);

  const double volume = deltasSparseGrid.getSetup().getVolumeOfDomain();
  for (double& value : result) {
    value *= volume;
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector quadrature(const VectorSourceFunc& sourceFunc,
                            const DeltasSparseGrid& deltasSparseGrid,
                            const DeltasSparseGridQuadratureOptions& options) {
  base::DataVector result(sourceFunc.nOutputs());
  quadrature(sourceFunc, deltasSparseGrid, result, options);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
