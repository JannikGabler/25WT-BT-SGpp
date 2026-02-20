#include <omp.h>
#include <algorithm>
#include <cassert>
#include <limits>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <sgpp/combigrid/tools/math/math_operations.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

MIVec genMIVecForFullSG(const MIType maxLvl, const size_t nDim) {
  if (nDim == 0) {
    return {0, 0};
  } else if (nDim == 1) {
    return {{maxLvl}};
  }

  const MIType minSum = full_sg_gen::getMinComponentSum(maxLvl, nDim);
  const std::vector<size_t> nMIs = full_sg_gen::nMICntPerComponentSum(minSum, maxLvl, nDim);

  MIVec miVec(nDim, nMIs[nMIs.size() - 1]);
  full_sg_gen::populateMIVec(miVec, minSum, maxLvl, nMIs);
  return miVec;
}

std::vector<CTCoeffType> genCoeffForFullSG(const MIType maxLvl, const size_t nDim) {
  if (nDim == 0) {
    return {};
  } else if (nDim == 1) {
    return {1};
  }

  const MIType minSum = tools::full_sg_gen::getMinComponentSum(maxLvl, nDim);
  const std::vector<size_t> nMIs = tools::full_sg_gen::nMICntPerComponentSum(minSum, maxLvl, nDim);
  const std::vector<CTCoeffType> binomials =
      full_sg_gen::getBinomialsForCTCoeffs(minSum, maxLvl, nDim);

  std::vector<CTCoeffType> coeff(nMIs[nMIs.size() - 1]);

  for (MIType sumIdx = 0; sumIdx <= maxLvl - minSum; sumIdx++) {
    const size_t startIdx = sumIdx == 0 ? 0 : nMIs[sumIdx - 1];
    const size_t endIdx = (sumIdx == maxLvl - minSum ? coeff.size() : nMIs[sumIdx]) - 1;

    std::fill(coeff.begin() + startIdx, coeff.begin() + endIdx, binomials[sumIdx]);
  }

  return coeff;
}

/******************
Internal operations
******************/
namespace full_sg_gen {

MIType getMinComponentSum(const MIType maxSum, const size_t nDim) {
  if (nDim > maxSum) {
    return 0;
  }

  return maxSum - (MIType)nDim + 1;
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

void addBarPosAsMI(MIVec& miVec, const size_t miIdx, const std::vector<size_t>& barPos,
                   const size_t maxBarPos) {
  assert(barPos.size() >= 1);

  miVec(miIdx, 0) = (MIType)barPos[0];

  for (size_t dim = 1; dim < miVec.nDim() - 1; dim++) {
    miVec(miIdx, dim) = (MIType)(barPos[dim] - barPos[dim - 1] - 1);
  }

  miVec(miIdx, miVec.nDim() - 1) = (MIType)(maxBarPos - barPos[miVec.nDim() - 2]);
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

size_t getMaxBarPos(const MIType sum, const size_t nDim) {
  assert(sum + nDim >= 2);

  return sum + nDim - 2;
}

std::vector<size_t> initBarPos(const size_t nDim) {
  assert(nDim >= 1);

  std::vector<size_t> barPos(nDim - 1, 0);

  for (size_t dim = 0; dim < nDim - 1; dim++) {
    barPos[dim] = dim;
  }

  return barPos;
}

void incrementBarPos(std::vector<size_t>& barPos, const size_t maxBarPos) {
  for (size_t barIdx = barPos.size() - 1; barIdx != std::numeric_limits<size_t>::max(); barIdx--) {
    const size_t mostRightPos = maxBarPos - (barPos.size() - 1 - barIdx);

    if (barIdx < 0 || barIdx >= barPos.size()) {
      const int tmp = 0;
    }

    if (barPos[barIdx] < mostRightPos) {
      barPos[barIdx]++;

      for (size_t rightBarIdx = barIdx + 1; rightBarIdx < barPos.size(); rightBarIdx++) {
        barPos[rightBarIdx] = barPos[rightBarIdx - 1] + 1;
      }
      return;
    }
  }
}

/*
Writes into barPos
nMIs: #MIs with at most sum of the idx
*/
std::vector<size_t> getBarPosOfMIIdx(const size_t miIdx, const std::vector<size_t>& nMIs,
                                     const size_t sumIdx, const size_t maxBarPos,
                                     const size_t nDim) {
  std::vector<size_t> barPos(nDim - 1);
  size_t miIdxLeft = miIdx - (sumIdx == 0 ? 0 : nMIs[sumIdx - 1]);

  size_t minCurBarPos = 0;
  for (size_t barIdx = 0; barIdx < barPos.size(); barIdx++) {
    const size_t barsToTheRight = barPos.size() - 1 - barIdx;
    const size_t maxCurBarPos = maxBarPos - barsToTheRight;

    for (size_t curBarPos = minCurBarPos; curBarPos <= maxCurBarPos; curBarPos++) {
      const size_t slotsLeftForTheRight = maxBarPos - curBarPos;
      const size_t nMIForCurSelection = binomial(slotsLeftForTheRight, barsToTheRight);

      if (miIdxLeft < nMIForCurSelection ||
          curBarPos == maxCurBarPos) {  // Did we find the pos of the bar with idx 'barIdx'?
        minCurBarPos = curBarPos + 1;
        barPos[barIdx] = curBarPos;
        break;
      } else {
        miIdxLeft -= nMIForCurSelection;
      }
    }
  }

  return barPos;
}

void populateMIVecSerial(MIVec& miVec, const size_t startIdx, const size_t endIdx,
                         const MIType minSum, const std::vector<size_t>& nMIs) {
  size_t sumIdx = getSumIdxOfMIIdx(startIdx, nMIs);
  size_t maxBarPos = getMaxBarPos(minSum + (MIType)sumIdx, miVec.nDim());
  std::vector<size_t> barPos = getBarPosOfMIIdx(startIdx, nMIs, sumIdx, maxBarPos, miVec.nDim());

  for (size_t miIdx = startIdx; miIdx <= endIdx; miIdx++) {
    if (miIdx >= nMIs[sumIdx]) {  // Go to the next sum
      sumIdx++;
      maxBarPos++;
      barPos = initBarPos(miVec.nDim());
    }

    addBarPosAsMI(miVec, miIdx, barPos, maxBarPos);
    incrementBarPos(barPos, maxBarPos);
  }
}

/*
Based on the mathematical stars and bars concept
*/
void populateMIVec(MIVec& miVec, const MIType minSum, const MIType maxSum,
                   const std::vector<size_t>& nMIs) {
  const size_t totalNumberOfMIs = nMIs[nMIs.size() - 1];
  const std::vector<size_t> part = tools::partitionRangeForConcurrency(
      totalNumberOfMIs, constants::sg_gen_instr::FSG_MIN_MI_FOR_CONCURRENCY,
      constants::sg_gen_instr::FSG_MIN_MI_PER_THREAD);

#pragma omp parallel num_threads(part.size() - 1) if (part.size() > 2)
  {
    const size_t threadId = (size_t)omp_get_thread_num();
    populateMIVecSerial(miVec, part[threadId], part[threadId + 1] - 1, minSum, nMIs);
  }
}

std::vector<CTCoeffType> getBinomialsForCTCoeffs(const MIType minSum, const MIType maxSum,
                                                 const size_t nDim) {
  std::vector<CTCoeffType> binomials(maxSum - minSum + 1);

  for (MIType sum = minSum; sum <= maxSum; sum++) {
    const CTCoeffType distanceToMaxSum = (CTCoeffType)(maxSum - sum);

    if (distanceToMaxSum % 2 == 0) {
      binomials[sum - minSum] = tools::binomial((CTCoeffType)(nDim - 1), distanceToMaxSum);
    } else {
      binomials[sum - minSum] = -tools::binomial((CTCoeffType)(nDim - 1), distanceToMaxSum);
    }
  }

  return binomials;
}

}  // namespace full_sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp