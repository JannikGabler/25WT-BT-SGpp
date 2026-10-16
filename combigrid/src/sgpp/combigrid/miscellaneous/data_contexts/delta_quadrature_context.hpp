/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Read-only data of one delta-quadrature call, shared by all functions and threads.
 */
struct DeltaQuadratureContext {
  // Top-level arguments
  const SparseGrid& sparseGrid;
  const SourceFunc& sourceFunc;
  const HyperCubeArea& domain;
  // const std::vector<NodeGenFunc*>& nodeGenFuncs;
  // const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs;

  // Precomputed values
  const FlatNestedVec<double, 3>& weights1D;  ///< weights1D[tgIdx][dim][nodeIdx]
  const std::vector<NodeIdxLvlMapFunc>& nodeIdxLvlMapFuncs;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
