// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file closure_trie_test.cpp
 * @brief Boost.Test cases for @c misc::ClosureTrie (leaf ranks, lookups, rejected inputs).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <vector>

using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::TensorGridCTData;
using sgpp::combigrid::misc::ClosureTrie;

namespace {

/// Tensor-grid data with the given multi-indices (coefficients and tensor grids unused).
std::vector<TensorGridCTData> tgDataOf(const std::vector<LvlMI>& mis) {
  std::vector<TensorGridCTData> result(mis.size());
  for (size_t i = 0; i < mis.size(); i++) result[i].mi = mis[i];
  return result;
}

/// All multi-indices of d components with 1-norm <= maxSum, in an order that is not the
/// lexicographic one (dimension 0 slowest).
void completeMIs(const size_t nDim, const LvlType maxSum, LvlMI& mi, const size_t dim,
                 std::vector<LvlMI>& out) {
  if (dim == nDim) {
    out.push_back(mi);
    return;
  }
  LvlType used = 0;
  for (size_t k = 0; k < dim; k++) used += mi[k];
  for (LvlType lvl = 0; lvl + used <= maxSum; lvl++) {
    mi[dim] = lvl;
    completeMIs(nDim, maxSum, mi, dim + 1, out);
  }
  mi[dim] = 0;
}

/// true if a precedes b lexicographically with dimension d - 1 most significant.
bool precedes(const LvlMI& a, const LvlMI& b) {
  for (size_t j = a.size(); j-- > 0;) {
    if (a[j] != b[j]) return a[j] < b[j];
  }
  return false;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(closure_trie)

BOOST_AUTO_TEST_CASE(Leaves_Are_Lexicographic_Ranks) {
  for (size_t nDim = 1; nDim <= 4; nDim++) {
    std::vector<LvlMI> mis;
    LvlMI mi(nDim, 0);
    completeMIs(nDim, 4, mi, 0, mis);
    const std::vector<TensorGridCTData> tgData = tgDataOf(mis);
    const ClosureTrie trie(tgData, nDim);

    BOOST_CHECK_EQUAL(trie.nDim(), nDim);
    BOOST_REQUIRE_EQUAL(trie.nLeaves(), mis.size());
    for (size_t leaf = 0; leaf + 1 < trie.nLeaves(); leaf++) {
      BOOST_CHECK(precedes(mis[trie.leafItem(leaf)], mis[trie.leafItem(leaf + 1)]));
    }
    for (size_t i = 0; i < mis.size(); i++) {
      const size_t leaf = trie.findLeaf(mis[i]);
      BOOST_REQUIRE(leaf != ClosureTrie::NOT_FOUND);
      BOOST_CHECK_EQUAL(trie.leafItem(leaf), i);
    }
  }
}

BOOST_AUTO_TEST_CASE(Child_Walk_Equals_Find) {
  // Anisotropic downwards-closed set in 3 dimensions
  std::vector<LvlMI> mis;
  for (LvlType a = 0; a <= 3; a++) {
    for (LvlType b = 0; b <= 2; b++) {
      for (LvlType c = 0; c <= 1; c++) {
        if (a + 2 * b + c <= 4) mis.push_back(LvlMI{a, b, c});
      }
    }
  }
  const std::vector<TensorGridCTData> tgData = tgDataOf(mis);
  const ClosureTrie trie(tgData, 3);

  for (const LvlMI& mi : mis) {
    uint32_t node = ClosureTrie::ROOT;
    for (size_t j = 3; j-- > 0;) {
      BOOST_REQUIRE_LT(mi[j], trie.nChildren(node));
      node = trie.child(node, mi[j]);
    }
    BOOST_CHECK_EQUAL(node, trie.findLeaf(mi));
  }
  BOOST_CHECK_EQUAL(trie.findLeaf(LvlMI{4, 0, 0}), ClosureTrie::NOT_FOUND);
  BOOST_CHECK_EQUAL(trie.findLeaf(LvlMI{0, 0, 2}), ClosureTrie::NOT_FOUND);
  BOOST_CHECK_EQUAL(trie.findLeaf(LvlMI{3, 1, 0}), ClosureTrie::NOT_FOUND);
}

BOOST_AUTO_TEST_CASE(Invalid_Sets_Are_Rejected) {
  const std::vector<TensorGridCTData> gap = tgDataOf({LvlMI{0, 0}, LvlMI{2, 0}});
  BOOST_CHECK_THROW(ClosureTrie(gap, 2), std::invalid_argument);
  const std::vector<TensorGridCTData> duplicate = tgDataOf({LvlMI{0, 0}, LvlMI{0, 0}});
  BOOST_CHECK_THROW(ClosureTrie(duplicate, 2), std::invalid_argument);
  BOOST_CHECK_THROW(ClosureTrie(gap, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
