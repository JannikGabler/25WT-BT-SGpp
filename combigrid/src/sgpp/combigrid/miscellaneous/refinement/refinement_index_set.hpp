// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_index_set.hpp
 * @brief Append-only set of level multi-indices with constant-time neighbour lookups (internal
 * structure of the dimension-adaptive refinement).
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Append-only set of level multi-indices with constant-time lookups of neighbours.
 *
 * The multi-indices are stored densely (@ref nDim levels each) in insertion order, so the index
 * of a multi-index never changes. Lookups go through an open-addressing hash table. The hash of
 * a multi-index is the sum (modulo @f$2^{64}@f$) of one pseudo-random value per
 * (dimension, level) entry (@ref entryHash), so the hash of a neighbour (one level changed by
 * @f$\pm 1@f$) or of any member of the backward box @f$\vec{\ell} - \vec{z}@f$,
 * @f$\vec{z} \in \{0, 1\}^d@f$, follows from the hash of @f$\vec{\ell}@f$ in O(1) per changed
 * entry; a candidate is compared entry by entry only if its hash matches (O(nDim)).
 *
 * Not thread-safe for concurrent inserts; concurrent lookups without inserts are safe.
 */
class RefinementIndexSet {
 public:
  /// Result of the lookups if the multi-index is not in the set.
  static constexpr size_t NOT_FOUND = std::numeric_limits<size_t>::max();

  /// @name Constructors
  /// @{

  /// @brief Constructs the empty set of @p nDim -dimensional multi-indices.
  explicit RefinementIndexSet(size_t nDim);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the dimensionality of the multi-indices.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the number of multi-indices.
  size_t size() const noexcept { return hashes_.size(); }

  /// @brief Returns the levels of multi-index @p idx (valid until the next @ref insert).
  LvlMIView levels(const size_t idx) const noexcept {
    return LvlMIView(levels_.data() + idx * nDim_, nDim_);
  }

  /// @brief Returns the hash of multi-index @p idx.
  uint64_t hash(const size_t idx) const noexcept { return hashes_[idx]; }

  /// @}

  /// @name Lookups
  /// @{

  /// @brief Returns the hash contribution of level @p lvl in dimension @p dim.
  static uint64_t entryHash(size_t dim, LvlType lvl) noexcept;

  /// @brief Returns the hash of @p mi (the sum of its entry hashes; O(nDim)).
  uint64_t hashOf(LvlMIView mi) const noexcept;

  /// @brief Returns the index of @p mi, or @ref NOT_FOUND.
  size_t find(LvlMIView mi) const noexcept;

  /**
   * @brief Returns the index of @p mi + @p step · @f$\vec{e}_{dim}@f$, or @ref NOT_FOUND
   * (also if the level would become negative).
   * @param mi   Multi-index (need not be in the set).
   * @param hash Hash of @p mi (@ref hashOf).
   * @param dim  Dimension whose level changes.
   * @param step @c +1 or @c -1.
   */
  size_t findNeighbour(LvlMIView mi, uint64_t hash, size_t dim, int step) const noexcept;

  /**
   * @brief Returns the index of @f$\vec{\ell} - \sum_{i \in M} \vec{e}_{dims[i]}@f$, where
   * @f$\vec{\ell}@f$ = @p mi and @f$M@f$ the set bits of @p mask, or @ref NOT_FOUND.
   * @param mi   Multi-index whose levels in @p dims are at least 1.
   * @param hash Hash of @p mi.
   * @param dims Distinct dimensions in ascending order (bit @c i of @p mask refers to
   *             @p dims[i]; at most 64).
   * @param mask Which of @p dims are decremented.
   */
  size_t findBackward(LvlMIView mi, uint64_t hash, std::span<const size_t> dims,
                      uint64_t mask) const noexcept;

  /// @}

  /// @name Modifiers
  /// @{

  /**
   * @brief Appends @p mi (must not be in the set) and returns its index (= the previous size).
   * @param mi Multi-index; must not view the set's own storage (copy @ref levels first).
   */
  size_t insert(LvlMIView mi);

  /// @brief Reserves memory for @p nMI multi-indices.
  void reserve(size_t nMI);

  /// @}

 private:
  /// @brief Returns the index whose hash is @p hash and for which @p matches(levels) holds.
  template <typename Matches>
  size_t lookup(uint64_t hash, const Matches& matches) const noexcept;

  /// @brief Enters multi-index @p idx into the hash table.
  void place(size_t idx) noexcept;

  /// @brief Doubles the hash table (keeps the load factor at most 1/2).
  void growTable();

  size_t nDim_;                   ///< Dimensionality.
  std::vector<LvlType> levels_;   ///< nDim_ levels per multi-index, in insertion order.
  std::vector<uint64_t> hashes_;  ///< Hash per multi-index.
  std::vector<size_t> table_;     ///< Open-addressing slots: index + 1, 0 = empty.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
