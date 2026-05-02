#pragma once

#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<double> barycentricFormulaWeights(const std::vector<double>& nodes);

}
}  // namespace combigrid
}  // namespace sgpp