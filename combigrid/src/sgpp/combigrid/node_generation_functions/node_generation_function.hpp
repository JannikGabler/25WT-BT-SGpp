#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

class NodeGenFunc {
 public:
  //   NodeGenFunc(const uint64_t id) : id_(id) {}

  virtual base::DataVector operator()(size_t nNodes) const;

  virtual QuadRule getQuadRule(size_t nNodes) const;

  //   uint64_t id() { return id_; }

  //  private:
  //   const uint64_t id_;
};

}  // namespace combigrid
}  // namespace sgpp
