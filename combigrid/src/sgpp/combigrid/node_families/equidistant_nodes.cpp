#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/node_families/equidistant_nodes.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/tools/node_families/equidistant_nodes_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>

using sgpp::combigrid::tools::equidistant_nodes::mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
using sgpp::combigrid::tools::equidistant_nodes::
    mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;

namespace sgpp {
namespace combigrid {

namespace node_families {

/***********
Constructors
***********/

EquidistantNodes::EquidistantNodes()
    : NodeFamily(tools::fnv1aHash("Equidistant Node Generation Function")) {}

/**************
Node generation
**************/

void EquidistantNodes::genNodesWithoutBoundary(const GPCntType nNodes, base::DataVector& out,
                                               size_t startIdx) const {
  assert(out.size() >= startIdx + nNodes);

  const double factor = 1.0 / static_cast<double>(nNodes + 1);

  for (GPCntType i = 1; i <= nNodes; i++) {
    out[startIdx++] = static_cast<double>(i) * factor;
  }
}

void EquidistantNodes::genDeltaNodesWithoutBoundary(const LvlType lvl,
                                                    const Lvl2GPCntFunc lvl2GPCntFunc,
                                                    const std::span<double> out) const {
  assert(out.size() == tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, false));

  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    const GPCntType nNodesOnLvl = lvl2GPCntFunc(lvl);
    const double factor = 1.0 / static_cast<double>(nNodesOnLvl + 1);

    for (GPCntType i = 1; i <= (nNodesOnLvl + 1) / 2; i++) {
      // Generate every second node
      out[i - 1] = static_cast<double>(2 * i - 1) * factor;
    }
  } else [[unlikely]] {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "EquidistantNodes::genDeltaNodesWithoutBoundary: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "EquidistantNodes::genDeltaNodesWithoutBoundary: Equidistant nodes of the are not "
          "nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

/******
Getters
******/

// TODO: includeBoundary is currently ignored
QuadRule* EquidistantNodes::getQuadRule(const GPCntType nNodes, bool includeBoundary) const {
  if (nNodes & 1) {  // nNodes is odd
    return getSimpsonQuadRule();
  } else {  // nNodes is even
    return getTrapezoidalQuadRule();
  }
}

InterpolationMethod* EquidistantNodes::getInterpolationMethod(const GPCntType nNodes) const {
  return getBarycentricFormula(this);
}

bool EquidistantNodes::isNested(const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == linearLvl2GPCntFunction) {
    return false;
  } else if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    return true;
  } else [[unlikely]] {
    throw base::not_implemented_exception(
        "EquidistantNodes::isNested: Unknown Lvl2GPCntFunc function.");
  }
}

/********
Utilities
********/

NodeIdxLvlMapFunc EquidistantNodes::getNodeIdxMapBetweenLvls(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    return mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "EquidistantNodes::getNodeIdxMapBetweenLvls: Unknown level-to-grid-point-count "
          "function.");
    } else {
      throw std::invalid_argument(
          "EquidistantNodes::getNodeIdxMapBetweenLvls: Equidistant nodes are not nested "
          "w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

NodeIdxLvlMapFunc EquidistantNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == doublingLvl2GPCntFunction) [[likely]] {
    return mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;
  } else {
    if (isNested(lvl2GPCntFunc)) {
      throw base::not_implemented_exception(
          "EquidistantNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Unknown "
          "level-to-grid-point-count function.");
    } else {
      throw std::invalid_argument(
          "EquidistantNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl: Equidistant nodes are "
          "not nested w.r.t. the provided level-to-grid-point-count function.");
    }
  }
}

/********
Operators
********/

bool EquidistantNodes::operator==(const NodeFamily& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_families

}  // namespace combigrid
}  // namespace sgpp
