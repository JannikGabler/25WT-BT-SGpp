#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/second_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/functions/node_generation_functions/equidistant_node_generation_function_utilities.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>

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

SecondTypeChebyshevNodeGenFunc::SecondTypeChebyshevNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("Second Type Chebyshev Node Generation Function")) {}

/**************
Node generation
**************/

void SecondTypeChebyshevNodeGenFunc::genNodesWithoutBoundary(const GPCntType nNodes,
                                                             base::DataVector& out,
                                                             size_t startIdx) const {
  assert(out.size() - startIdx >= nNodes);

  const double f1 = M_PI / static_cast<double>(nNodes + 1);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    out[startIdx++] = nonTransformedNode / 2 + 0.5;
  }
}

void SecondTypeChebyshevNodeGenFunc::genDeltaNodesWithoutBoundary(
    const LvlType lvl, const Lvl2GPCntFunc lvl2GPCntFunc, const std::span<double> out) const {
  assert(out.size() == tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, false));

  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    const GPCntType nNodesOnLvl = lvl2GPCntFunc(lvl);
    const double f1 = M_PI / static_cast<double>(nNodesOnLvl + 1);

    for (GPCntType i = 1; i <= (nNodesOnLvl + 1) / 2; i++) {
      // Generate every second node f(i) = 1 + 2 * (i-1)
      const double f2 = static_cast<double>(nNodesOnLvl - 2 * i + 2);  // Reverse order
      const double nonTransformedNode = std::cos(f1 * f2);
      out[i - 1] = nonTransformedNode / 2 + 0.5;
    }
  } else [[unlikely]] {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "SecondTypeChebyshevNodeGenFunc::genDeltaNodesWithoutBoundary: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "SecondTypeChebyshevNodeGenFunc::genDeltaNodesWithoutBoundary: Chebyshev nodes of the "
          "second type are not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

/******
Getters
******/

QuadRule* SecondTypeChebyshevNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  return getClenshawCurtisQuadRule();
}

InterpolationMethod* SecondTypeChebyshevNodeGenFunc::getInterpolationMethod(
    const GPCntType nNodes) const {
  return getOptBarycentricFormula();
}

bool SecondTypeChebyshevNodeGenFunc::isNested(const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == linearLvl2GPCntFunction) {
    return false;
  } else if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    return true;
  } else {
    throw base::not_implemented_exception(
        "SecondTypeChebyshevNodeGenFunc::isNested: Unknown level-to-grid-point-count function.");
  }
}

/********
Utilities
********/

NodeIdxLvlMapFunc SecondTypeChebyshevNodeGenFunc::getNodeIdxMapBetweenLvls(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    // Intentionally returns the same as EquidistantNodeGenFunc
    return mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "SecondTypeChebyshevNodeGenFunc::getNodeIdxMapBetweenLvls: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "SecondTypeChebyshevNodeGenFunc::getNodeIdxMapBetweenLvls: Chebyshev nodes of the second "
          "type are not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

NodeIdxLvlMapFunc SecondTypeChebyshevNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    // Intentionally returns the same as EquidistantNodeGenFunc
    return mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "SecondTypeChebyshevNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "SecondTypeChebyshevNodeGenFunc::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Chebyshev "
          "nodes of the second type are not nested w.r.t. the provided level-to-grid-point-count "
          "function.");
    }
  }
}

/********
Operators
********/

bool SecondTypeChebyshevNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
