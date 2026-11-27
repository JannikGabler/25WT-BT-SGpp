// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_index_set_tools.hpp
 * @brief Operations of the dimension-adaptive refinement on a @c misc::RefinementIndexSet:
 * backward boxes, incremental combination coefficients, forward neighbours, canonical order.
 */
#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {
namespace refinement {

/**
 * @brief Writes the dimensions in which @p mi has a positive level to @p support (ascending).
 */
void computeSupport(LvlMIView mi, std::vector<size_t>& support);

/**
 * @brief Calls @p visit(memberIdx, sign) for every member @f$\vec{\ell} - \vec{z}@f$,
 * @f$\vec{z} \in \{0, 1\}^d@f$ with @f$\vec{z} \le \vec{\ell}@f$, of the backward box of
 * multi-index @p idx, where @c sign @f$= (-1)^{|\vec{z}|}@f$.
 *
 * The members are visited in a fixed order (the bit patterns of @f$\vec{z}@f$ over the
 * dimensions of the support in increasing numeric order; @f$\vec{z} = 0@f$, i.e. @p idx itself,
 * first), so sums over them are reproducible. Every member must be in @p set (holds in a
 * downwards-closed set). Cost: @f$2^{s}@f$ lookups for a support of size @f$s@f$, at most the
 * number of grid points of the tensor grid of @p idx for growth functions with at least two
 * nodes from level 1 on.
 *
 * @param set     Index set.
 * @param idx     Multi-index whose backward box is visited.
 * @param support Scratch (overwritten with the support of @p idx).
 * @param visit   Callable @c (size_t memberIdx, int sign).
 */
template <typename Visit>
void forEachBackwardBoxMember(const misc::RefinementIndexSet& set, const size_t idx,
                              std::vector<size_t>& support, const Visit& visit) {
  const LvlMIView mi = set.levels(idx);
  const uint64_t hash = set.hash(idx);
  computeSupport(mi, support);
  assert(support.size() < 64);

  const uint64_t nMembers = uint64_t{1} << support.size();
  for (uint64_t mask = 0; mask < nMembers; mask++) {
    const size_t member = set.findBackward(mi, hash, support, mask);
    assert(member != misc::RefinementIndexSet::NOT_FOUND);
    visit(member, (std::popcount(mask) & 1) ? -1 : 1);
  }
}

/**
 * @brief Adds the changes of the combination coefficients caused by adding multi-index @p idx
 * to the combination: @f$c_{\vec{\ell} - \vec{z}} \mathrel{+}= (-1)^{|\vec{z}|}@f$ for every
 * member of its backward box.
 *
 * Adding the indices of a downwards-closed set one by one in any order that keeps every prefix
 * downwards closed yields the coefficients of @c tools::computeCTCoeffs.
 *
 * @param set          Index set (contains the backward box of @p idx).
 * @param idx          Added multi-index.
 * @param coefficients Coefficient per index of @p set (at least @c set.size() entries).
 * @param support      Scratch.
 */
void addToCombinationCoefficients(const misc::RefinementIndexSet& set, size_t idx,
                                  std::vector<CTCoeffType>& coefficients,
                                  std::vector<size_t>& support);

/**
 * @brief Returns whether every forward neighbour @f$\vec{\ell} + \vec{e}_k@f$ of multi-index
 * @p idx with level @f$\ell_k + 1 \le@f$ @p maxLvl is in @p set.
 */
bool hasAllForwardNeighbours(const misc::RefinementIndexSet& set, size_t idx, LvlType maxLvl);

/**
 * @brief Returns @p indices sorted in colexicographic order of their multi-indices (last
 * dimension most significant), the order of @c LvlMIVec::downwardsClosure.
 */
std::vector<size_t> sortColexicographically(const misc::RefinementIndexSet& set,
                                            std::vector<size_t> indices);

/**
 * @brief Returns the index set of the multi-indices of @p miVec, inserted in their order.
 * @param miVec Multi-indices without duplicates.
 */
misc::RefinementIndexSet toIndexSet(const LvlMIVec& miVec);

}  // namespace refinement
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
