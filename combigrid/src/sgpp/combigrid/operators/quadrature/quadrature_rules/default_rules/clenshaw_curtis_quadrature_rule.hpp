#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

class ClenshawCurtisQuadRule : public QuadRule {
 public:
  ClenshawCurtisQuadRule() : QuadRule(tools::fnv1aHash("Clenshaw Curtis Quadrature Rule")) {}

  base::DataVector getWeights(const size_t nNodes) const override;

 private:
  /*
  Fallback if neither the Eigen nor the Armadillo library is available.
  */
  base::DataVector genWeightsNaive(const size_t nNodes) const;

  /*
  More efficient method by Waldvogel using the Inverse Fourier Transformation.
  Requires Eigen or Armadillo.
  */
  std::vector<double> clenshawCurtisWeightsFFT(const size_t nNodes);
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp