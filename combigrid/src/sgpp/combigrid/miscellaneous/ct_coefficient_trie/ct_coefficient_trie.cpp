// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

CTCoeffTrie::CTCoeffTrie(const std::span<const TensorGridCTData> tgData, const size_t nDim)
    : nDim_(nDim) {
  std::vector<const TensorGridCTData*> sortedTGs;
  sortedTGs.reserve(tgData.size());

  for (const TensorGridCTData& tg : tgData) {
    assert(tg.mi.size() == nDim);

    if (tg.coefficient != 0) {
      sortedTGs.push_back(&tg);
    }
  }

  // Lexicographic order (dimension 0 most significant). Not MI::operator<, which is the
  // component-wise partial order.
  std::ranges::sort(sortedTGs, [](const TensorGridCTData* a, const TensorGridCTData* b) {
    return std::ranges::lexicographical_compare(a->mi, b->mi);
  });

  // firstDiffDim[i]: first dimension in which the i-th multi-index differs from the (i-1)-th one.
  // The i-th multi-index starts a new node of depth k iff firstDiffDim[i] <= k.
  std::vector<size_t> firstDiffDim(sortedTGs.size(), 0);

  for (size_t i = 1; i < sortedTGs.size(); i++) {
    const LvlMI& mi = sortedTGs[i]->mi;
    firstDiffDim[i] =
        static_cast<size_t>(std::ranges::mismatch(mi, sortedTGs[i - 1]->mi).in1 - mi.begin());

    assert(firstDiffDim[i] < nDim && "The level multi-indices have to be pairwise distinct");
  }

  nodes.reserveSubVecs(1, nDim);

  for (size_t depth = 0; depth < nDim; depth++) {
    nodes.appendSubVec(1);  // nodes[depth]

    const bool isLeafDepth = (depth + 1 == nDim);
    size_t nChildren = 0;  // Number of nodes of depth + 1 up to the current multi-index

    for (size_t i = 0; i < sortedTGs.size(); i++) {
      if (firstDiffDim[i] <= depth) {
        nodes.push_back(Node{sortedTGs[i]->mi[depth], 0});
      }

      if (!isLeafDepth) {
        if (firstDiffDim[i] <= depth + 1) {
          nChildren++;
        }

        nodes.back().childEnd = nChildren;  // The last node contains the i-th multi-index
      }
    }
  }

  leafCoeffs_.reserve(sortedTGs.size());

  for (const TensorGridCTData* tg : sortedTGs) {
    leafCoeffs_.push_back(static_cast<double>(tg->coefficient));
  }
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
