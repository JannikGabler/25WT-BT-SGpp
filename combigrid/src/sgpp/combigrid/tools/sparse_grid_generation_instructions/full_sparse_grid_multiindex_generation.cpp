#include <cassert>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/math/math_operations.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_multiindex_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace {

MIType minComponentSum(const MIType maxSum, const size_t nDim) {
  if (nDim > maxSum) {
    return 0;
  }

  return maxSum - (MIType)nDim + 1 <= 0;
}

std::vector<size_t> nMICntPerComponentSum(const MIType minSum, const MIType maxSum,
                                          const size_t nDim) {
  assert(maxSum >= minSum);
  std::vector<size_t> nMIs(maxSum - minSum + 1);

  size_t acc = 0;
  for (MIType sum = minSum; sum <= maxSum; sum++) {
    acc += numberOfMIWithComponentSum(nDim, sum);
    nMIs[sum - minSum] = acc;
  }

  return nMIs;
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
  assert(barPos.size() >= 1);

  miVec(miIdx, 0) = barPos[0];

  for (size_t dim = 1; dim < miVec.nDim() - 1; dim++) {
    miVec(miIdx, dim) = barPos[dim] - barPos[dim - 1] - 1;
  }

  miVec(miIdx, miVec.nDim() - 1) = maxBarPos - barPos[miVec.nDim() - 2];
}

/*
nMIs: #MIs with at most sum of the idx
*/
size_t getSumIdxOfMIIdx(const size_t miIdx, const std::vector<size_t>& nMIs) {
  assert(nMIs.size() >= 1);

  for (size_t sumIdx = 0; sumIdx < nMIs.size(); sumIdx++) {
    if (miIdx < nMIs[sumIdx]) {
      return sumIdx;
    }
  }

  throw std::out_of_range("Specified mi idx is out of range (>= total number of mis)!");
}

/*
Writes into barPos
nMIs: #MIs with at most sum of the idx
*/
void getBarPosOfMIIdx(std::vector<MIType>& barPos, const size_t miIdx,
                      const std::vector<size_t>& nMIs, const size_t sumIdx,
                      const MIType maxBarPos) {
  size_t miIdxLeft = miIdx - (sumIdx == 0 ? 0 : nMIs[sumIdx - 1]);

  MIType minCurBarPos = 0;
  for (size_t barIdx = 0; barIdx < barPos.size(); barIdx++) {
    for (size_t curBarPos = minCurBarPos; true; curBarPos++) {
      const size_t barsToTheRight = barPos.size() - 1 - barIdx;
      const size_t slotsLeftForTheRight = maxBarPos - curBarPos;
      const size_t nMIForCurSelection = binomial(slotsLeftForTheRight, barsToTheRight);

      if (miIdxLeft < nMIForCurSelection) {  // Did we find the pos of the bar with idx 'barIdx'?
        minCurBarPos = curBarPos + 1;
        barPos[barIdx] = curBarPos;
        break;
      } else {
        miIdxLeft -= nMIForCurSelection;
      }
    }
  }
}

/*
Based on the mathematical stars and bars concept
*/
// void populateMIVecSerial(MIVec& miVec, const std::vector<size_t>& miVecOffsets, const MIType
// minSum,
//                          const MIType maxSum, const std::vector<size_t>& numberOfMIs) {
//   for (MIType sum = minSum; sum <= maxSum; sum++) {
//     const size_t miVecOffset = miVecOffsets[sum - minSum];
//     const size_t nMI = numberOfMIs[sum - minSum];
//     const MIType maxBarPos = sum + (MIType)miVec.nDim() - 2;

//     if (sum == 0) {  // Only possible mi is (0, ..., 0)
//       for (size_t dim = 0; dim < miVec.nDim(); dim++) miVec(miVecOffset, dim) = 0;
//       continue;
//     }

//     std::vector<MIType> barPos = initBarPos(miVec.nDim());

//     for (size_t i = 0; i < nMI; i++) {
//       addBarPosAsMI(miVec, miVecOffset + i, barPos, maxBarPos);
//       incrementBarPos(barPos, maxBarPos);
//     }
//   }
// }

/*
Based on the mathematical stars and bars concept
*/
void populateMIVec(MIVec& miVec, const MIType minSum, const MIType maxSum,
                   const std::vector<size_t>& nMIs) {
  const size_t totalNumberOfMIs = nMIs[nMIs.size() - 1];

  // #pragma omp parallel if (totalNumberOfMIs > 5)  // TODO
  {
    std::vector<MIType> localBarPos(miVec.nDim() - 1);

#pragma omp for schedule(static)
    for (size_t miIdx = 0; miIdx < totalNumberOfMIs; miIdx++) {
      const size_t sumIdx = getSumIdxOfMIIdx(miIdx, nMIs);
      const MIType maxBarPos = minSum + sumIdx + miVec.nDim() - 2;

      getBarPosOfMIIdx(localBarPos, miIdx, nMIs, sumIdx, maxBarPos);
      addBarPosAsMI(miVec, miIdx, localBarPos, maxBarPos);
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

  const MIType minSum = minComponentSum(maxLvl, nDim);
  const std::vector<size_t> nMIs = nMICntPerComponentSum(minSum, maxLvl, nDim);

  MIVec miVec(nDim, nMIs[nMIs.size() - 1]);
  populateMIVec(miVec, minSum, maxLvl, nMIs);
  return miVec;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp