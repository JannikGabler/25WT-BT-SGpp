/**
 * @file multiindex_utilities.hpp
 * @brief Combinatorial utilities related to multi-index sets.
 */
#ifndef COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP

#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Number of multi-indices @f$\vec\ell \in \mathbb{N}_0^d@f$ whose
 * components sum to @p sumOfComponents.
 *
 * Equals the well-known stars-and-bars count
 * @f$\binom{s + d - 1}{d - 1}@f$ where @f$s@f$ is the requested sum and
 * @f$d@f$ is @p nDim.
 *
 * @tparam T              Type of the per-component sum.
 * @param nDim            Dimensionality.
 * @param sumOfComponents Required component sum.
 * @return Number of multi-indices with that component sum.
 */
template <typename T>
size_t numberOfMIWithComponentSum(const size_t nDim, const T sumOfComponents) {
  return binomial(sumOfComponents + nDim - 1, nDim - 1);  // nDim = 0 should work fine
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
