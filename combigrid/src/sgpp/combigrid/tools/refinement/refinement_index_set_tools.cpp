// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cstddef>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_index_set_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

using sgpp::combigrid::misc::RefinementIndexSet;

namespace sgpp {
namespace combigrid {
namespace tools {
namespace refinement {

void computeSupport(const LvlMIView mi, std::vector<size_t>& support) {
  support.clear();
  for (size_t dim = 0; dim < mi.size(); dim++) {
    if (mi[dim] > 0) {
      support.push_back(dim);
    }
  }
}

void addToCombinationCoefficients(const RefinementIndexSet& set, const size_t idx,
                                  std::vector<CTCoeffType>& coefficients,
                                  std::vector<size_t>& support) {
  forEachBackwardBoxMember(set, idx, support, [&](const size_t member, const int sign) {
    coefficients[member] += sign;
  });
}

bool hasAllForwardNeighbours(const RefinementIndexSet& set, const size_t idx,
                             const LvlType maxLvl) {
  const LvlMIView mi = set.levels(idx);
  const uint64_t hash = set.hash(idx);

  for (size_t dim = 0; dim < set.nDim(); dim++) {
    if (mi[dim] < maxLvl && set.findNeighbour(mi, hash, dim, 1) == RefinementIndexSet::NOT_FOUND) {
      return false;
    }
  }
  return true;
}

std::vector<size_t> sortColexicographically(const RefinementIndexSet& set,
                                            std::vector<size_t> indices) {
  const size_t nDim = set.nDim();
  std::ranges::sort(indices, [&](const size_t a, const size_t b) {
    const LvlMIView miA = set.levels(a);
    const LvlMIView miB = set.levels(b);
    for (size_t dim = nDim; dim-- > 0;) {
      if (miA[dim] != miB[dim]) {
        return miA[dim] < miB[dim];
      }
    }
    return false;
  });
  return indices;
}

RefinementIndexSet toIndexSet(const LvlMIVec& miVec) {
  RefinementIndexSet set(miVec.nDim());
  set.reserve(miVec.nMI());
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    set.insert(miVec[miIdx]);
  }
  return set;
}

}  // namespace refinement
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
