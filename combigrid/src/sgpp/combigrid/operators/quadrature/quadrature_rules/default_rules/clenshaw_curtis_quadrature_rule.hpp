#pragma once

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

class ClenshawCurtisQuadRule : public QuadRule {
 public:
  ClenshawCurtisQuadRule() : QuadRule(tools::fnv1aHash("Clenshaw Curtis Quadrature Rule")) {}

  /*
  Quadrature weight computations based on Waldvogel, von Winckel construction
  */
  base::DataVector getWeights(const size_t nNodes) const override;

 private:
  base::DataVector genWeightsNaive(const size_t nNodes) const;
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp