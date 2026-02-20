#ifndef COMBIGRID_TOOLS_FULL_SPARSE_GRID_MULTIINDEX_GENERATION_HPP
#define COMBIGRID_TOOLS_FULL_SPARSE_GRID_MULTIINDEX_GENERATION_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

MIVec genMIVecForFullSG(MIType maxLvl, size_t nDim);

std::vector<CTCoeffType> genCoeffForFullSG(MIType maxLvl, size_t nDim);

/******************
Internal operations
******************/
namespace full_sg_gen {

MIType getMinComponentSum(MIType maxSum, size_t nDim);

std::vector<size_t> nMICntPerComponentSum(MIType minSum, MIType maxSum, size_t nDim);

/*
nMIs: #MIs with at most sum of the idx
*/
size_t getSumIdxOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs);

size_t getMaxBarPos(MIType sum, size_t nDim);

std::vector<size_t> initBarPos(size_t nDim);

void incrementBarPos(std::vector<size_t>& barPos, size_t maxBarPos);

std::vector<size_t> getBarPosOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs, size_t sumIdx,
                                     size_t maxBarPos, size_t nDim);

void populateMIVec(MIVec& miVec, MIType minSum, MIType maxSum, const std::vector<size_t>& nMIs);

std::vector<CTCoeffType> getBinomialsForCTCoeffs(MIType minSum, MIType maxSum, size_t nDim);

}  // namespace full_sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif