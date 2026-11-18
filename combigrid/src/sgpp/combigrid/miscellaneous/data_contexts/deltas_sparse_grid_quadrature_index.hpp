// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_quadrature_index.hpp
 * @brief Defines @c DSGQuadIndex, the query-independent data of the quadrature on a deltas sparse
 * grid.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>

namespace sgpp {
namespace combigrid {

class DeltasSparseGrid;

namespace misc {

/**
 * @brief The data of the quadrature on a deltas sparse grid that depends on the grid only: the 1D
 * quadrature weights of every (dimension, level) in delta order
 * (@c tools::dsg_quadrature::comp1DWeights) and the trie of the combination coefficients.
 */
class DSGQuadIndex {
 public:
  /**
   * @brief Builds the data of @p deltasSparseGrid.
   * @param deltasSparseGrid Deltas sparse grid.
   */
  explicit DSGQuadIndex(const DeltasSparseGrid& deltasSparseGrid);

  /// @brief Returns @c weights1D[dim][lvl]: the 1D quadrature weights of level @c lvl in delta
  /// order.
  const FlatNestedVec<double, 3>& weights1D() const noexcept { return weights1D_; }

  /// @brief Returns the trie over the levels of the tensor grids with a non-zero coefficient.
  const CTCoeffTrie& coeffTrie() const noexcept { return coeffTrie_; }

  /// @brief Returns the memory of the data in bytes (the object and its heap memory).
  size_t memoryBytes() const noexcept {
    return sizeof(*this) + weights1D_.heapBytes() + coeffTrie_.heapBytes();
  }

 private:
  FlatNestedVec<double, 3> weights1D_;  ///< See @ref weights1D.
  CTCoeffTrie coeffTrie_;               ///< See @ref coeffTrie.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
