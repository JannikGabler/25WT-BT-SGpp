#include <algorithm>
#include <cmath>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

bool nearly_equal(const float a, const float b, const float abs_tol, const float rel_tol) {
  const float diff = std::fabs(a - b);
  if (diff <= abs_tol) return true;

  return diff <= rel_tol * std::max(std::fabs(a), std::fabs(b));
}

bool nearly_equal(const double a, const double b, const double abs_tol, const double rel_tol) {
  const double diff = std::fabs(a - b);
  if (diff <= abs_tol) return true;

  return diff <= rel_tol * std::max(std::fabs(a), std::fabs(b));
}

bool nearly_equal(const std::vector<float>& a, const std::vector<float>& b, const float abs_tol,
                  const float rel_tol) {
  if (a.size() != b.size()) {
    return false;
  }

  for (size_t i = 0; i < a.size(); i++) {
    if (!nearly_equal(a[i], b[i], abs_tol, rel_tol)) {
      return false;
    }
  }

  return true;
}

bool nearly_equal(const std::vector<double>& a, const std::vector<double>& b, double abs_tol,
                  double rel_tol) {
  if (a.size() != b.size()) {
    return false;
  }

  for (size_t i = 0; i < a.size(); i++) {
    if (!nearly_equal(a[i], b[i], abs_tol, rel_tol)) {
      return false;
    }
  }

  return true;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
