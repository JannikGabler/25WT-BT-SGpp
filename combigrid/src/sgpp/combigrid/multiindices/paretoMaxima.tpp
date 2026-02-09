#include <omp.h>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
inline void updateParetoMaxima(const MIVec<T> miVec, std::vector<size_t>& paretoMaxima,
                               const size_t candidateIdx) {
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
    updateParetoMaxima(miVec, paretoMaxima, candidateIdx);
  }

  return paretoMaxima;
}

template <typename T>
std::vector<size_t> computeParetoMaximaParallel(const MIVec<T>& miVec) {
  const size_t length = miVec.nMI() * miVec.nDim();

  if (length < ParetoMaxima::MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
    return computeParetoMaximaSerial(miVec, 0, miVec.nMI() - 1);
  }

  const std::vector<size_t> partitioning =
      tools::partitionRange(length, ParetoMaxima::MIN_MIVEC_BATCH_LENGTH_PER_THREAD);
  std::vector<std::vector<size_t>> localParetoMaxima(partitioning.size() - 1);

#pragma omp parallel num_threads(partitioning.size() - 1)
  {
    const int threadId = omp_get_num_threads();

    localParetoMaxima[threadId] =
        computeParetoMaximaSerial(miVec, partitioning[threadId], partitioning[threadId + 1]);
  }

  return mergeParetoMaxima(miVec, localParetoMaxima);
}

template <typename T>
std::vector<size_t> mergeParetoMaxima(const MIVec<T> miVec,
                                      std::vector<std::vector<size_t>> localParetoMaxima) {
  std::vector<size_t> merged;

  for (const std::vector<size_t>& local : localParetoMaxima) {
    for (const size_t candidateIdx : local) {
      updateParetoMaxima(miVec, merged, candidateIdx);
    }
  }
}

}  // namespace combigrid
}  // namespace sgpp
