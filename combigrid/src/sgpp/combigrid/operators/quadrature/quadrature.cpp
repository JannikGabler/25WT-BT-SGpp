// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <memory>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/compensated_sum.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <span>
#include <stdexcept>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadWeightTable;
using sgpp::combigrid::tools::quadrature::estimateTensorGrids;
using sgpp::combigrid::tools::quadrature::getWeightTable;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                  const QuadratureOptions& options) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const size_t nTG = sparseGrid.nTG();

  const std::shared_ptr<const QuadWeightTable> weightTable =
      getWeightTable(sparseGrid, options.useCrossCallOperatorDataCache);
  const QuadratureContext context{.sparseGrid = sparseGrid, .weightTable = *weightTable};

  // One estimate per tensor grid (nTG doubles), combined serially in the order of the tensor
  // grids: results bitwise independent of the number of threads and the schedule. Compensated
  // summation: the coefficients are large integers of alternating sign (round-off of a plain loop
  // up to 2e-8 relative on complete grids with 5e4 tensor grids; compensated: about one rounding)
  const std::unique_ptr<double[]> tgEstimates = std::make_unique_for_overwrite<double[]>(nTG);
  estimateTensorGrids(context, sourceFunc, std::span<double>(tgEstimates.get(), nTG));
  const double result = tools::compensatedDotProduct(
      sparseGrid.getCoefficients(), std::span<const double>(tgEstimates.get(), nTG));

  sourceFunc.finishOperatorCall();

  return genInstr.getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                const std::span<double> result, const QuadratureOptions& options) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("quadrature: The size of the result is not the number of outputs");
  }

  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const std::shared_ptr<const QuadWeightTable> weightTable =
      getWeightTable(sparseGrid, options.useCrossCallOperatorDataCache);
  const QuadratureContext context{.sparseGrid = sparseGrid, .weightTable = *weightTable};

  // As in the scalar operator: estimates per tensor grid (nTG m doubles), combined in order
  const size_t nEstimates = sparseGrid.nTG() * result.size();
  const std::unique_ptr<double[]> tgEstimates =
      std::make_unique_for_overwrite<double[]>(nEstimates);
  estimateTensorGrids(context, sourceFunc, std::span<double>(tgEstimates.get(), nEstimates));
  tools::compensatedDotProducts(sparseGrid.getCoefficients(),
                                std::span<const double>(tgEstimates.get(), nEstimates), result);

  const double volume = genInstr.getVolumeOfDomain();
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
