/**
 * @file barycentric_formula.hpp
 * @brief Helper for the barycentric Lagrange interpolation method.
 */
#pragma once

#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Computes the barycentric weights @f$w_i@f$ for a generic node set.
 *
 * The returned vector @f$w@f$ satisfies @f$w_i = 1 / \prod_{j\neq i} (x_i - x_j)@f$
 * (up to a common factor) and is the input to the barycentric formula of the
 * second kind.
 *
 * @param nodes 1D node positions.
 * @return Newly allocated vector with the same length as @p nodes.
 */
std::vector<double> barycentricFormulaWeights(const std::vector<double>& nodes);

}
}  // namespace combigrid
}  // namespace sgpp
