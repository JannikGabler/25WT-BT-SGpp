#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/equidistant_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/functions/node_generation_functions/equidistant_node_generation_function_utilities.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>

using sgpp::combigrid::tools::equidistant_node_gen_function::
    mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
using sgpp::combigrid::tools::equidistant_node_gen_function::
    mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

/***********
Constructors
***********/

EquidistantNodeGenFunc::EquidistantNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("Equidistant Node Generation Function")) {}

/**************
Node generation
**************/

void EquidistantNodeGenFunc::genNodesWithoutBoundary(const GPCntType nNodes, base::DataVector& out,
                                                     size_t startIdx) const {
  assert(out.size() >= startIdx + nNodes);

  const double factor = 1.0 / static_cast<double>(nNodes + 1);

  for (GPCntType i = 1; i <= nNodes; i++) {
    out[startIdx++] = static_cast<double>(i) * factor;
  }
}

/******
Getters
******/

QuadRule* EquidistantNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  if (nNodes & 1) {  // nNodes is odd
    return getSimpsonQuadRule();
  } else {  // nNodes is even
    return getTrapezoidalQuadRule();
  }
}

InterpolationMethod* EquidistantNodeGenFunc::getInterpolationMethod(const GPCntType nNodes) const {
  return getBarycentricFormula(this);
}

bool EquidistantNodeGenFunc::isNested(const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == linearLvl2GPCntFunction) {
    return false;
  } else if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    return true;
  } else [[unlikely]] {
    throw base::not_implemented_exception(
        "EquidistantNodeGenFunc::isNested: Unknown Lvl2GPCntFunc function.");
  }
}

/********
Utilities
********/

NodeIdxLvlMapFunc EquidistantNodeGenFunc::getNodeIdxMapBetweenLvls(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    return mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "EquidistantNodeGenFunc::getNodeIdxMapBetweenLvls: Unknown level-to-grid-point-count "
          "function.");
    } else {
      throw std::invalid_argument(
          "EquidistantNodeGenFunc::getNodeIdxMapBetweenLvls: Equidistant nodes are not nested "
          "w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

NodeIdxLvlMapFunc EquidistantNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    return mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "EquidistantNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "EquidistantNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Equidistant nodes are "
          "not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

/********
Operators
********/

bool EquidistantNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
