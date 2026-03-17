#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

struct QuadScratch {
  QuadScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim)
      : gp(nDim), weights(maxTGSumOfGPCntsPerDim), quadRules(nDim) {}

 public:
  base::DataVector gp;
  base::DataVector weights;
  std::vector<QuadRule*> quadRules;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp