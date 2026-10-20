// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
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
  const DeltasSparseGrid& deltasSparseGrid;
  const SourceFunc& sourceFunc;
  const HyperCubeArea& domain;

  // Precomputed values
  /// @brief @c weights1D[dim][lvl]: 1D quadrature weights of level @c lvl in delta order (see
  /// @c delta_quadrature_operator::comp1DWeights).
  const FlatNestedVec<double, 3>& weights1D;
  /// @brief Trie over the levels of the tensor grids with a non-zero coefficient.
  const CTCoeffTrie& coeffTrie;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
