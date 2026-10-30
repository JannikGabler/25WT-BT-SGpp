// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <stdexcept>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;
using namespace sgpp::combigrid::tools::quadrature;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const size_t nDim = sparseGrid.nDim();

  const QuadratureContext context{.sparseGrid = sparseGrid,
                                  .domain = genInstr.getDomain(),
                                  .nodeGenFuncs = genInstr.getNodeGenFuncs()};

  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    QuadratureScratch scratch(nDim, sparseGrid.getMaxTGSumOverGPCntsPerDim());

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      result +=
          tgData.coefficient * processTensorGrid(context, sourceFunc, tgData.tensorGrid, scratch);
    }
  }

  return genInstr.getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument("quadrature: The size of the result is not the number of outputs");
  }

  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const QuadratureContext context{.sparseGrid = sparseGrid,
                                  .domain = genInstr.getDomain(),
                                  .nodeGenFuncs = genInstr.getNodeGenFuncs()};

  combineTensorGrids(context, sourceFunc, result);

  const double volume = genInstr.getVolumeOfDomain();
  for (double& value : result) {
    value *= volume;
  }
}

base::DataVector quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  quadrature(sourceFunc, sparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
