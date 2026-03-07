#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

// TODO: Optimize (caching) (double long could be used in pre-computation)
std::vector<double> barycentricFormulaWeights(const std::vector<double>& nodes) {
  assert(nodes.size() >= 2);

  const size_t nNodes = nodes.size();

  std::vector<double> logSums(nNodes, 0);  // logSums[i] = - sum_{j \neq i} |nodes[i] - nodes[j]|
  std::vector<int> signs(nNodes, 1);

  for (size_t i = 0; i < nNodes; i++) {
    const double nodeI = nodes[i];
    double logSum = 0;
    int sign = 1;

    for (size_t j = 0; j < nNodes; j++) {
      if (i != j) {
        const double diff = nodeI - nodes[j];
        const double aDiff = std::fabs(diff);
        assert(aDiff > 1e-12);
        logSum -= std::log(aDiff);
        if (diff < 0) sign *= -1;
      }
    }

    logSums[i] = logSum;
    signs[i] = sign;
  }

  const double maxLogSum = *std::max_element(logSums.begin(), logSums.end());
  std::vector<double> weights(nNodes);

  for (size_t i = 0; i < nNodes; i++) {
    weights[i] = signs[i] * std::exp(logSums[i] - maxLogSum);
  }

  return weights;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp