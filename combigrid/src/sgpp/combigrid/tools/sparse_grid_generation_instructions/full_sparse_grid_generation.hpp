/**
 * @file full_sparse_grid_generation.hpp
 * @brief Helpers that build the level-multi-index set and the combination
 * coefficients for a "full" combination-technique sparse grid.
 *
 * A full combination-technique sparse grid in @f$d@f$ dimensions with
 * level @f$L@f$ contains all level multi-indices @f$\vec\ell@f$ with
 * @f$\|\vec\ell\|_1 \le L@f$. The combination coefficients are the
 * standard
 * @f$c_{\vec\ell} = (-1)^{L - \|\vec\ell\|_1}\,\binom{d-1}{L - \|\vec\ell\|_1}@f$.
 */
#ifndef COMBIGRID_TOOLS_FULL_SPARSE_GRID_GENERATION_HPP
#define COMBIGRID_TOOLS_FULL_SPARSE_GRID_GENERATION_HPP

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Generates the level multi-indices of the full sparse grid.
 *
 * Contains every multi-index @f$\vec\ell\in\mathbb{N}^d@f$ with
 * @f$\|\vec\ell\|_1 \le \mathrm{maxLvl}@f$ (those that contribute a
 * non-zero combination coefficient).
 *
 * @param maxLvl Largest @f$\ell_1@f$ norm of any included multi-index.
 * @param nDim   Spatial dimensionality.
 * @return Level-multi-index vector.
 */
LvlMIVec genMIVecForFullSG(LvlType maxLvl, size_t nDim);

/**
 * @brief Generates the combination coefficients matching @ref genMIVecForFullSG.
 *
 * Coefficient @c i corresponds to the multi-index returned at position
 * @c i by @ref genMIVecForFullSG (same @p maxLvl and @p nDim).
 *
 * @param maxLvl Largest @f$\ell_1@f$ norm of any included multi-index.
 * @param nDim   Spatial dimensionality.
 * @return Vector of integer combination coefficients.
 */
std::vector<CTCoeffType> genCoeffForFullSG(LvlType maxLvl, size_t nDim);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the full sparse-grid generator.
 *
 * The generator enumerates multi-indices grouped by their @f$\ell_1@f$
 * sum: for a fixed sum @c s, every multi-index of @f$\mathbb N^d@f$ with
 * that sum corresponds to a unique placement of @f$d-1@f$ "bars" among
 * @c s + d - 1 positions ("stars-and-bars"). The functions in this
 * sub-namespace implement that bijection together with bookkeeping for
 * efficient parallel enumeration.
 */
namespace full_sg_gen {

/**
 * @brief Smallest @f$\ell_1@f$ norm whose multi-indices have a non-zero
 * combination coefficient.
 *
 * Equals @f$\max(0, \mathrm{maxSum} - (d-1))@f$ where @f$d@f$ is @p nDim.
 *
 * @param maxSum Maximum considered @f$\ell_1@f$ norm.
 * @param nDim   Spatial dimensionality.
 */
LvlType getMinComponentSum(LvlType maxSum, size_t nDim);

/**
 * @brief Number of multi-indices with each fixed @f$\ell_1@f$ norm in
 * the range @c [minSum, maxSum].
 *
 * Entry @c k of the returned vector contains the count for sum
 * @c minSum + k.
 *
 * @param minSum Smallest considered sum.
 * @param maxSum Largest considered sum.
 * @param nDim   Spatial dimensionality.
 */
std::vector<size_t> nMICntPerComponentSum(LvlType minSum, LvlType maxSum, size_t nDim);

/**
 * @brief Translates a stars-and-bars bar position into the corresponding
 * multi-index slot of @p miVec.
 *
 * @param miVec     Output multi-index vector.
 * @param miIdx     Slot in @p miVec to populate.
 * @param barPos    Bar positions (size @c nDim - 1).
 * @param maxBarPos Maximum permissible bar position (= sum + d - 1).
 */
void addBarPosAsMI(LvlMIVec& miVec, size_t miIdx, const std::vector<size_t>& barPos,
                   size_t maxBarPos);

/**
 * @brief Returns the index of the @f$\ell_1@f$-sum block to which the
 * @p miIdx -th multi-index belongs.
 *
 * @param miIdx Multi-index index.
 * @param nMIs  Cumulative-count helper (entry @c k gives the count up to and
 *              including sum @c minSum + k).
 */
size_t getSumIdxOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs);

/**
 * @brief Maximum bar position for the given @f$\ell_1@f$ norm.
 * @param sum  Component sum.
 * @param nDim Dimensionality.
 */
size_t getMaxBarPos(LvlType sum, size_t nDim);

/**
 * @brief Initial bar position vector (the lexicographically smallest
 * configuration) for dimension @p nDim.
 */
std::vector<size_t> initBarPos(size_t nDim);

/**
 * @brief Advances @p barPos to the next valid configuration in
 * lexicographic order.
 * @param barPos    In/out: bar positions.
 * @param maxBarPos Maximum permissible bar position.
 */
void incrementBarPos(std::vector<size_t>& barPos, size_t maxBarPos);

/**
 * @brief Reconstructs the bar positions for the @p miIdx -th multi-index.
 * @param miIdx     Target multi-index index.
 * @param nMIs      Cumulative-count helper.
 * @param sumIdx    Block index into @p nMIs.
 * @param maxBarPos Maximum bar position for this block.
 * @param nDim      Dimensionality.
 */
std::vector<size_t> getBarPosOfMIIdx(size_t miIdx, const std::vector<size_t>& nMIs, size_t sumIdx,
                                     size_t maxBarPos, size_t nDim);

/**
 * @brief Populates @p miVec with all multi-indices whose @f$\ell_1@f$ norm
 * lies in @c [minSum, maxSum].
 *
 * @param miVec  Output vector (sized appropriately by the caller).
 * @param minSum Smallest considered sum.
 * @param maxSum Largest considered sum.
 * @param nMIs   Per-sum count helper from @ref nMICntPerComponentSum.
 */
void populateMIVec(LvlMIVec& miVec, LvlType minSum, LvlType maxSum,
                   const std::vector<size_t>& nMIs);

/**
 * @brief Precomputes the binomials needed for full-SG combination coefficients.
 *
 * Returns a vector containing @f$\binom{d-1}{k}@f$ for the values of
 * @f$k@f$ that appear in the active sum range.
 *
 * @param minSum Smallest active sum.
 * @param maxSum Largest active sum.
 * @param nDim   Dimensionality.
 */
std::vector<CTCoeffType> getBinomialsForCTCoeffs(LvlType minSum, LvlType maxSum, size_t nDim);

}  // namespace full_sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
