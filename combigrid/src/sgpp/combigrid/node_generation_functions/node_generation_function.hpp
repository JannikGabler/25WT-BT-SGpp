#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

class NodeGenFunc {
 public:
  virtual base::DataVector operator()(size_t nNodes) const;

  virtual QuadRule getQuadRule(size_t nNodes) const;
};

}  // namespace combigrid
}  // namespace sgpp
