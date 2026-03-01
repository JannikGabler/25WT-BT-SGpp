#pragma once

#include <cmath>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

bool nearly_equal(float a, float b, float abs_tol = 1e-6f, float rel_tol = 1e-6f);

bool nearly_equal(double a, double b, double abs_tol = 1e-12, double rel_tol = 1e-12);

bool nearly_equal(const std::vector<float>& a, const std::vector<float>& b, float abs_tol = 1e-6f,
                  float rel_tol = 1e-6f);

bool nearly_equal(const std::vector<double>& a, const std::vector<double>& b,
                  double abs_tol = 1e-12, double rel_tol = 1e-12);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
