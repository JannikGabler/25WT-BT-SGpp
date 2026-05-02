#pragma once

#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

struct LinInterpScratch {
  LinInterpScratch(const size_t nDim) : curGridPoint(nDim), neighborsPerDim(nDim) {}

 public:
  base::DataVector curGridPoint;
  std::vector<std::pair<double, double>> neighborsPerDim;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp