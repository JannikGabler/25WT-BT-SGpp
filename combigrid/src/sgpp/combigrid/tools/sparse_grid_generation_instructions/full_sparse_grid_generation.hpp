#ifndef COMBIGRID_TOOLS_FULL_SPARSE_GRID_GENERATION_HPP
#define COMBIGRID_TOOLS_FULL_SPARSE_GRID_GENERATION_HPP

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

LvlMIVec genMIVecForFullSG(LvlType maxLvl, size_t nDim);

std::vector<CTCoeffType> genCoeffForFullSG(LvlType maxLvl, size_t nDim);

/******************
Internal operations
******************/
namespace full_sg_gen {

LvlType getMinComponentSum(LvlType maxSum, size_t nDim);

std::vector<size_t> nMICntPerComponentSum(LvlType minSum, LvlType maxSum, size_t nDim);

void addBarPosAsMI(LvlMIVec& miVec, size_t miIdx, const std::vector<size_t>& barPos,
                   size_t maxBarPos);

/*
nMIs: #MIs with at most sum of the idx
*/
size_t getSumIdxOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs);

size_t getMaxBarPos(LvlType sum, size_t nDim);

std::vector<size_t> initBarPos(size_t nDim);

void incrementBarPos(std::vector<size_t>& barPos, size_t maxBarPos);

std::vector<size_t> getBarPosOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs, size_t sumIdx,
                                     size_t maxBarPos, size_t nDim);

void populateMIVec(LvlMIVec& miVec, LvlType minSum, LvlType maxSum,
                   const std::vector<size_t>& nMIs);

std::vector<CTCoeffType> getBinomialsForCTCoeffs(LvlType minSum, LvlType maxSum, size_t nDim);

}  // namespace full_sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif