#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/node_families/clenshaw_curtis_nodes.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/node_families/equidistant_nodes_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using sgpp::combigrid::tools::equidistant_nodes::mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
using sgpp::combigrid::tools::equidistant_nodes::
    mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;

namespace sgpp {
namespace combigrid {

namespace node_families {

/***********
Constructors
***********/


/**************
Node generation
**************/

void ClenshawCurtisNodes::genNodesWithoutBoundary(const GPCntType nNodes, base::DataVector& out,
                                                  size_t startIdx) const {
  assert(out.size() - startIdx >= nNodes);

  const double f1 = M_PI / static_cast<double>(nNodes + 1);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    out[startIdx++] = nonTransformedNode / 2 + 0.5;
  }
}

void ClenshawCurtisNodes::genDeltaNodesWithoutBoundary(const LvlType lvl,
                                                       const Lvl2GPCntFunc lvl2GPCntFunc,
                                                       const std::span<double> out) const {
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
          "ClenshawCurtisNodes::genDeltaNodesWithoutBoundary: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "ClenshawCurtisNodes::genDeltaNodesWithoutBoundary: Chebyshev nodes of the "
          "second type are not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

/******
Getters
******/

// TODO: includeBoundary is ignored
QuadRule* ClenshawCurtisNodes::getQuadRule(const GPCntType nNodes, bool includeBoundary) const {
  return getClenshawCurtisQuadRule();
}

InterpolationMethod* ClenshawCurtisNodes::getInterpolationMethod(const GPCntType nNodes) const {
  return getOptBarycentricFormula();
}

bool ClenshawCurtisNodes::isNested(const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == linearLvl2GPCntFunction) {
    return false;
  } else if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    return true;
  } else {
    throw base::not_implemented_exception(
        "ClenshawCurtisNodes::isNested: Unknown level-to-grid-point-count function.");
  }
}

/********
Utilities
********/

NodeIdxLvlMapFunc ClenshawCurtisNodes::getNodeIdxMapBetweenLvls(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    // Intentionally returns the same as EquidistantNodes
    return mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "ClenshawCurtisNodes::getNodeIdxMapBetweenLvls: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "ClenshawCurtisNodes::getNodeIdxMapBetweenLvls: Chebyshev nodes of the second "
          "type are not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

NodeIdxLvlMapFunc ClenshawCurtisNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    // Intentionally returns the same as EquidistantNodes
    return mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "ClenshawCurtisNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "ClenshawCurtisNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Chebyshev "
          "nodes of the second type are not nested w.r.t. the provided level-to-grid-point-count "
          "function.");
    }
  }
}

/********
Operators
********/

bool ClenshawCurtisNodes::operator==(const NodeFamily& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_families

}  // namespace combigrid
}  // namespace sgpp
