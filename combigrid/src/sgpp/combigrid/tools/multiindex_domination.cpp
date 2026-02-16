#include <cassert>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/multiindex_domination.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

bool miDominatesMI(const combigrid::MIVec& miVec, const size_t miIdx1, const size_t miIdx2) {
  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    if (miVec(miIdx1, dim) < miVec(miIdx2, dim)) {
      return false;
    }
  }

  return true;
}

/*
Should only be called with miVec.nDim == mi.nDim
*/
bool miDominatesMI(const MIVec& miVec, const size_t miVecIdx, const MI& mi) {
  assert(miVec.nDim() == mi.nDim());

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    if (miVec(miVecIdx, dim) < mi[dim]) {
      return false;
    }
  }

  return true;
}

/*
Should only be called with miVec.nDim == mi.nDim
*/
bool miVecDominatesMI(const MIVec& miVec, const MI& mi) {
  assert(miVec.nDim() == mi.nDim());

  for (size_t miVecIdx = 0; miVecIdx < miVec.nMI(); miVecIdx++) {
    if (miDominatesMI(miVec, miVecIdx, mi)) {
      return true;
    }
  }

  return false;
}

/*
Should only be called with miVec.nDim == mi.nDim
*/
bool miVecDominatesMI(const MIVec& miVec, const std::vector<size_t> miVecIdx, const MI& mi) {
  assert(miVec.nDim() == mi.nDim());

  for (const size_t idx : miVecIdx) {
    if (miDominatesMI(miVec, idx, mi)) {
      return true;
    }
  }

  return false;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
