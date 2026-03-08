#pragma once

#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

class SecondTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  SecondTypeChebyshevNodeGenFunc();

  base::DataVector genGPs(GPCntType nNodes, bool addBoundary) const override;

  QuadRule* getQuadRule(const GPCntType nNodes) const override;

  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

  bool operator==(const NodeGenFunc& other) const override;

 private:
  base::DataVector genGPsWithBoundary(GPCntType nNodes) const;
  base::DataVector genGPsWithoutBoundary(GPCntType nNodes) const;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
