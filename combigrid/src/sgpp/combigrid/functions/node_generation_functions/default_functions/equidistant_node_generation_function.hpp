#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

class EquidistantNodeGenFunc : public NodeGenFunc {
 public:
  /***********
  Constructors
  ***********/
  EquidistantNodeGenFunc();

  /**************
  Node generation
  **************/
  void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const override;

  /***********
  SG Operators
  ***********/
  QuadRule* getQuadRule(GPCntType nNodes) const override;

  InterpolationMethod* getInterpolationMethod(GPCntType nNodes) const override;

  /********
  Operators
  ********/
  bool operator==(const NodeGenFunc& other) const override;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
