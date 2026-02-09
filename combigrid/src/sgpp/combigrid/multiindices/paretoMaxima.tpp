#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
inline bool miDominatesMI(const combigrid::MIVec<T>& miVec, size_t miIdx1, size_t miIdx2) {
  const size_t nDim = miVec.nDim();

  for (size_t dim = 0; dim < nDim; dim++) {
    if (miVec(miIdx1, dim) < miVec(miIdx2, dim)) {
      return false;
    }
  }

  return true;
}

template <typename T>
std::vector<size_t> computeParetoMaximaSerial(const combigrid::MIVec<T>& miVec,
                                              const size_t startIdx, const size_t endIdx) {
  std::vector<size_t> paretoMaxima;

  for (size_t candidateIdx = startIdx; candidateIdx <= endIdx; candidateIdx++) {
    bool dominated = false;

    for (auto iter = paretoMaxima.begin(); iter != paretoMaxima.end(); iter++) {
      if (miDominatesMI(miVec, *iter, candidateIdx)) {
        dominated = true;
        break;
      }
      if (miDominatesMI(miVec, candidateIdx, *iter)) {
        iter = paretoMaxima.erase(iter) - 1;
      }
    }

    if (!dominated) {
      paretoMaxima.push_back(candidateIdx);
    }
  }

  return paretoMaxima;
}

}  // namespace combigrid
}  // namespace sgpp
