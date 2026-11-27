// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <memory>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadWeightTable;
using sgpp::combigrid::tools::quadrature::estimateTensorGrids;
using sgpp::combigrid::tools::quadrature::getWeightTable;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                  const QuadratureOptions& options) {
  const SparseGridSetup& setup = sparseGrid.getSetup();
  const std::shared_ptr<const QuadWeightTable> weightTable =
      getWeightTable(sparseGrid, options.useCrossCallOperatorDataCache);
  const QuadratureContext context{.sparseGrid = sparseGrid, .weightTable = *weightTable};

  // One estimate per tensor grid, stored at its index
  const std::vector<double> tgEstimates = estimateTensorGrids(context, sourceFunc);

  const double result =
      tools::numerics::compensatedDotProduct<double>(sparseGrid.getCoefficients(), tgEstimates);

  sourceFunc.finishOperatorCall();

  return setup.getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                const std::span<double> result, const QuadratureOptions& options) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("quadrature: The size of the result is not the number of outputs");
  }

  const std::shared_ptr<const QuadWeightTable> weightTable =
      getWeightTable(sparseGrid, options.useCrossCallOperatorDataCache);
  const QuadratureContext context{.sparseGrid = sparseGrid, .weightTable = *weightTable};

  // One estimate per tensor grid, stored at its index
  const misc::FlatNestedVec<double, 2> tgEstimates = estimateTensorGrids(context, sourceFunc);

  tools::numerics::compensatedDotProducts<double>(sparseGrid.getCoefficients(), tgEstimates,
                                                  result);

  const double volume = sparseGrid.getSetup().getVolumeOfDomain();
  for (double& value : result) {
    value *= volume;
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                            const QuadratureOptions& options) {
  base::DataVector result(sourceFunc.nOutputs());
  quadrature(sourceFunc, sparseGrid, result, options);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
