// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/projected_sub_trie_builder.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using namespace sgpp::combigrid::tools::dsg_quadrature;
using sgpp::combigrid::misc::CTCoeffTrie;
using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::DeltaQuadratureScratch;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid) {
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const size_t nDim = deltasSparseGrid.nDim();

  const FlatNestedVec<double, 3> weights1D = comp1DWeights(deltasSparseGrid);
  const CTCoeffTrie coeffTrie(deltasSparseGrid.getTensorGrids(), nDim);

  const DeltaQuadratureContext context{.deltasSparseGrid = deltasSparseGrid,
                                       .sourceFunc = sourceFunc,
                                       .domain = genInstr.getDomain(),
                                       .weights1D = weights1D,
                                       .coeffTrie = coeffTrie};

  double result = 0.0;

#pragma omp parallel reduction(+ : result)
  {
    DeltaQuadratureScratch scratch(nDim, deltasSparseGrid.getMaxTGSumOverGPCntsPerDim());

    // The delta tensor grids differ in size by orders of magnitude. Dynamic scheduling was up to
    // 20% faster than guided scheduling (which hands out large chunks first) for large sparse
    // grids; processing them largest-first gave no further gain.
#pragma omp for schedule(dynamic) nowait
    for (const TensorGridCTData& tgData : deltasSparseGrid) {
      result += processDeltaTensorGrid(context, tgData, scratch);
    }
  }

  return genInstr.getVolumeOfDomain() * result;
}

}  // namespace combigrid
}  // namespace sgpp
