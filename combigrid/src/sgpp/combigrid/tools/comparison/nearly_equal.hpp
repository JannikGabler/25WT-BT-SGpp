/**
 * @file nearly_equal.hpp
 * @brief Robust floating-point and vector equality with combined absolute
 * and relative tolerances.
 */
#pragma once

#include <cmath>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Tests if two @c float values are nearly equal.
 *
 * Returns @c true if @c |a - b| is below @p abs_tol or below @c rel_tol *
 * @c max(|a|,|b|). Combining absolute and relative tolerance avoids
 * pathological behavior near zero (where pure relative tolerances fail)
 * and at large magnitudes (where pure absolute tolerances fail).
 *
 * @param a       First value.
 * @param b       Second value.
 * @param abs_tol Absolute tolerance (default @c 1e-6).
 * @param rel_tol Relative tolerance (default @c 1e-6).
 */
bool nearlyEqual(float a, float b, float abs_tol = 1e-6f, float rel_tol = 1e-6f);

/// @copydoc nearlyEqual(float,float,float,float)
bool nearlyEqual(double a, double b, double abs_tol = 1e-12, double rel_tol = 1e-12);

/**
 * @brief Element-wise nearly-equal comparison of @c float vectors.
 *
 * Returns @c false if the sizes differ.
 *
 * @param a       First vector.
 * @param b       Second vector.
 * @param abs_tol Absolute tolerance.
 * @param rel_tol Relative tolerance.
 */
bool nearlyEqual(const std::vector<float>& a, const std::vector<float>& b, float abs_tol = 1e-6f,
                 float rel_tol = 1e-6f);

/// @copydoc nearlyEqual(const std::vector<float>&, const std::vector<float>&, float, float)
bool nearlyEqual(const std::vector<double>& a, const std::vector<double>& b, double abs_tol = 1e-12,
                 double rel_tol = 1e-12);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
