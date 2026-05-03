/**
 * @file combitech_coefficients.hpp
 * @brief Computation of the combination-technique coefficients
 * @f$c_{\vec{\ell}}@f$ for a downwards-closed level multi-index set.
 */
#ifndef COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP
#define COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Computes the combination-technique coefficients of a downwards-closed
 * multi-index set.
 *
 * For a downwards-closed set @f$I@f$ the coefficients are
 * @f[
 *   c_{\vec\ell} = \sum_{\vec{e}\in\{0,1\}^d, \vec\ell+\vec{e}\in I}
 *                   (-1)^{\|\vec{e}\|_1}.
 * @f]
 * Each coefficient can be evaluated independently, which makes the
 * computation embarrassingly parallel.
 *
 * @param miVec Downwards-closed level multi-indices.
 * @return Combination coefficient for every entry of @p miVec.
 *
 * @note Runtime is @f$O(|\mathrm{miVec}| \cdot d)@f$. The implementation
 * is OpenMP-parallel above @c constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY.
 * @pre @p miVec must be downwards closed; otherwise the result is undefined.
 */
std::vector<CTCoeffType> computeCTCoeffs(const LvlMIVec& miVec);

/**
 * @brief Naive variant that does not assume downwards closedness.
 *
 * Slower than @ref computeCTCoeffs and only intended as a reference / fallback.
 *
 * @param miVec Level multi-indices.
 * @return Combination coefficient for every entry of @p miVec.
 */
std::vector<CTCoeffType> computeCTCoeffsNaive(const LvlMIVec& miVec);

/**
 * @brief Computes the combination-technique coefficient for a single
 * level multi-index.
 *
 * Iterates the @f$2^d@f$ corners of the unit forward cube around @p mi
 * and counts membership in @p miVec.
 *
 * @param mi    Level multi-index whose coefficient is requested.
 * @param miVec Containing multi-index set.
 * @return Combination coefficient @f$c_{\vec\ell}@f$.
 *
 * @note Runtime is @f$O(2^d)@f$. Serial.
 */
CTCoeffType computeCTCoeffSingle(const LvlMI& mi, const LvlMIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
