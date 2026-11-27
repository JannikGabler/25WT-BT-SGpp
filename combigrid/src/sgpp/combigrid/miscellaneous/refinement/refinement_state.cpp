// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

RefinementState::RefinementState(const size_t nDim, const size_t nValues)
    : nValues(nValues), indexSet(nDim) {}

size_t RefinementState::add(const LvlMIView mi, const RefinementIndexState state,
                            const size_t cost) {
  const size_t idx = indexSet.insert(mi);
  states.push_back(state);
  evaluated.push_back(0);
  coefficients.push_back(0);
  values.resize(values.size() + nValues, 0.0);
  gains.push_back(-1.0);
  costs.push_back(cost);
  return idx;
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
