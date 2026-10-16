#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/simpson_quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

SimpsonQuadRule::SimpsonQuadRule() : QuadRule(tools::fnv1aHash("Simpson Quadrature Rule")) {}

base::DataVector SimpsonQuadRule::genWeights(const GPCntType nNodes) const {
  base::DataVector weights(nNodes);

  genWeightsInplace(weights);

  return weights;
}

void SimpsonQuadRule::genWeightsInplace(const std::span<double> out) const {
  const size_t nNodes = out.size();

  if ((nNodes & 1) == 0) {  // Is nNodes even?
    throw std::invalid_argument("The argument 'nNodes' has to be odd.");
  } else if (nNodes == 1) {
    out[0] = 1.0;
    return;
  }

  const double w1 = 1 / (3 * static_cast<double>(nNodes - 1));
  const double w2 = 4 * w1;
  const double w3 = 2 * w1;

  size_t i = 0;
  out[i++] = w1;
  for (size_t k = 0; k < (nNodes - 3) / 2; k++) {
    out[i++] = w2;
    out[i++] = w3;
  }
  out[i++] = w2;
  out[i] = w1;
}

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp