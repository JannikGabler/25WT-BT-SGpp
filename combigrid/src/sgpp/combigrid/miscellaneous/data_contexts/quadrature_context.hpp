// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Read-only data of one quadrature call, shared by all functions and threads.
 *
 * The source function (@c SourceFunc or @c VectorSourceFunc) is passed separately, so that the
 * same context serves both value types of the kernels.
 */
struct QuadratureContext {
  const SparseGrid& sparseGrid;  ///< The grid (its node table holds the nodes on the domain).

  /// The 1D quadrature weights of the grid, shared by all threads (computed on first use).
  const QuadWeightTable& weightTable;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
