/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Read-only data of one quadrature call, shared by all functions and threads.
 */
struct QuadratureContext {
  const SparseGrid& sparseGrid;
  const SourceFunc& sourceFunc;

  const HyperCubeArea& domain;
  const std::vector<NodeGenFunc*>& nodeGenFuncs;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp