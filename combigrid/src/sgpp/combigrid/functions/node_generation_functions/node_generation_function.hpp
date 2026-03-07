#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

class NodeGenFunc {
 public:
  NodeGenFunc(const uint64_t id) : id_(id) {}

  virtual ~NodeGenFunc() = default;

  virtual base::DataVector genGPs(size_t nNodes) const = 0;

  /******
  Getters
  ******/
  virtual QuadRule* getQuadRule(size_t nNodes) const = 0;

  virtual InterpolationMethod* getInterpolationMethod(size_t nNodes) const = 0;

  uint64_t id() const { return id_; }

  /********
  Operators
  ********/
  virtual bool operator==(const NodeGenFunc& other) const = 0;

 private:
  const uint64_t id_;
};

}  // namespace combigrid
}  // namespace sgpp
