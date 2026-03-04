#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

class NodeGenFunc {
 public:
  virtual ~NodeGenFunc() = default;

  virtual base::DataVector genGPs(size_t nNodes) const = 0;

  virtual QuadRule* getQuadRule(size_t nNodes) const = 0;

  virtual bool operator==(const NodeGenFunc& other) const = 0;
};

}  // namespace combigrid
}  // namespace sgpp
