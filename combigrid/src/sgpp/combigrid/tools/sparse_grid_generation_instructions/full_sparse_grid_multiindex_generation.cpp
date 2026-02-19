#include <algorithm>
#include <cassert>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_multiindex_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace {

std::pair<MIType, MIType> minAndMaxComponentSum(const MIType maxLvl, const size_t nDim) {
  const MIType maxSum = maxLvl;
  const size_t minSum = std::max(MIType(0), maxSum - (MIType)nDim + 1);
  return {minSum, maxSum};
}

std::pair<std::vector<size_t>, size_t> nMICntPerComponentSum(const MIType minSum,
                                                             const MIType maxSum,
                                                             const size_t nDim) {
  std::vector<size_t> numbers{maxSum - minSum + 1};
  size_t totalNumber = 0;

  for (MIType sum = minSum; sum <= maxSum; sum++) {
    const size_t n = numberOfMIWithComponentSum(nDim, sum);
    numbers[sum - minSum] = n;
    totalNumber += n;
  }

  return {numbers, totalNumber};
}

std::vector<size_t> miVecOffsetPerComponentSum(const std::vector<size_t>& nMIPerComponentSum) {
  std::vector<size_t> miVecOffsets(nMIPerComponentSum.size(), 0);

  for (size_t i = 1; i < nMIPerComponentSum.size(); i++) {
    miVecOffsets[i] = miVecOffsets[i - 1] + nMIPerComponentSum[i - 1];
  }

  return miVecOffsets;
}

std::vector<MIType> initBarPos(const size_t nDim) {
  assert(nDim >= 1);

  std::vector<MIType> barPos(nDim - 1, 0);

  for (size_t dim = 0; dim < nDim - 1; dim++) {
    barPos[dim] = dim;
  }

  return barPos;
}

void incrementBarPos(std::vector<MIType>& barPos, const MIType maxBarPos) {
  for (size_t barIdx = barPos.size() - 1; barIdx >= 0; barIdx--) {
    const size_t mostRightPos = maxBarPos - (barPos.size() - 1 - barIdx);

    if (barPos[barIdx] < mostRightPos) {
      barPos[barIdx]++;

      for (size_t rightBarIdx = barIdx + 1; rightBarIdx < barPos.size(); rightBarIdx++) {
        barPos[rightBarIdx] = barPos[rightBarIdx - 1] + 1;
      }
      return;
    }
  }
}

void addBarPosAsMI(MIVec& miVec, const size_t miIdx, const std::vector<MIType>& barPos,
                   const MIType maxBarPos) {
  assert(barPos.size() >= 2);

  miVec(miIdx, 0) = barPos[0];

  for (size_t dim = 1; dim < miVec.nDim() - 1; dim++) {
    miVec(miIdx, dim) = barPos[dim] - barPos[dim - 1] - 1;
  }

  miVec(miIdx, miVec.nDim() - 1) = maxBarPos - barPos[miVec.nDim() - 2];
}

/*
Based on the mathematical stars and bars concept
*/
void populateMIVec(MIVec& miVec, const std::vector<size_t>& miVecOffsets, const MIType minSum,
                   const MIType maxSum, const std::vector<size_t>& numberOfMIs) {
#pragma omp parallel for schedule(dynamic, 1)
  for (MIType sum = minSum; sum <= maxSum; sum++) {
    const size_t miVecOffset = miVecOffsets[sum - minSum];
    const size_t nMI = numberOfMIs[sum - minSum];
    const MIType maxBarPos = sum + (MIType)miVec.nDim() - 2;

    if (sum == 0) {  // Only possible mi is (0, ..., 0)
      for (size_t dim = 0; dim < miVec.nDim(); dim++) miVec(miVecOffset, dim) = 0;
      continue;
    }

    std::vector<MIType> barPos = initBarPos(miVec.nDim());

    for (size_t i = 0; i < nMI; i++) {
      addBarPosAsMI(miVec, miVecOffset + i, barPos, maxBarPos);
      incrementBarPos(barPos, maxBarPos);
    }
  }
}

}  // namespace

MIVec genMIVecForFullSG(const MIType maxLvl, const size_t nDim) {
  if (nDim == 0) {
    return {0, 0};
  } else if (nDim == 1) {
    return {{maxLvl}};
  }

  const std::pair<MIType, MIType> minMaxSum = minAndMaxComponentSum(maxLvl, nDim);
  const std::pair<std::vector<size_t>, size_t> numberOfMIs =
      nMICntPerComponentSum(minMaxSum.first, minMaxSum.second, nDim);
  const std::vector<size_t> miVecOffsets = miVecOffsetPerComponentSum(numberOfMIs.first);

  MIVec miVec(nDim, numberOfMIs.second);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp