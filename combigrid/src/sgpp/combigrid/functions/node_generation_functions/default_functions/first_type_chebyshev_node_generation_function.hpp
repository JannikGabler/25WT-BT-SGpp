#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

class FirstTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  /***********
  Constructors
  ***********/
  FirstTypeChebyshevNodeGenFunc();

  /**************
  Node generation
  **************/
  void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const override;

  /***********
  SG Operators
  ***********/
  QuadRule* getQuadRule(const GPCntType nNodes) const override;

  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

  /********
  Operators
  ********/
  bool operator==(const NodeGenFunc& other) const override;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
