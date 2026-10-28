// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

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
