#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

class FirstTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  FirstTypeChebyshevNodeGenFunc();

  base::DataVector genGPs(GPCntType nNodes, bool addBoundary) const override;

  QuadRule* getQuadRule(const GPCntType nNodes) const override;

  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

  bool operator==(const NodeGenFunc& other) const override;

 private:
  base::DataVector genGPsWithBoundary(GPCntType GPCntType) const;
  base::DataVector genGPsWithoutBoundary(GPCntType GPCntType) const;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
