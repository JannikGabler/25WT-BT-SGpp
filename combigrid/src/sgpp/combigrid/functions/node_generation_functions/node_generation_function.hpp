#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class NodeGenFunc {
 public:
  NodeGenFunc(const uint64_t id) : id_(id) {}

  virtual ~NodeGenFunc() = default;

  /*
  Should not be used in hot loops because it requires memory allocations (-> genNodesInplace).
  */
  base::DataVector genNodes(GPCntType nNodes, bool addBoundary) const;

  /*
  Without boundary
  */
  virtual void genNodesInplace(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const = 0;

  /******
  Getters
  ******/
  virtual QuadRule* getQuadRule(GPCntType nNodes) const = 0;

  virtual InterpolationMethod* getInterpolationMethod(GPCntType nNodes) const = 0;

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
