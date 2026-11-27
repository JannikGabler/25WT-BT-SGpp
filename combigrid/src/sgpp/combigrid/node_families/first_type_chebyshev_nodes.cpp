#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/node_families/first_type_chebyshev_nodes.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/fejer_first_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_families {

/***********
Constructors
***********/


/**************
Node generation
**************/

void FirstTypeChebyshevNodes::genNodesWithoutBoundary(const GPCntType nNodes, base::DataVector& out,
                                                      size_t startIdx) const {
  assert(out.size() >= startIdx + nNodes);

  const double f1 = M_PI / (2 * static_cast<double>(nNodes));

  for (GPCntType i = 0; i < nNodes; i++) {
    const double f2 = 2 * static_cast<double>(nNodes - 1 - i) + 1;  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    out[startIdx++] = nonTransformedNode / 2 + 0.5;
  }
}

void FirstTypeChebyshevNodes::genDeltaNodesWithoutBoundary(const LvlType lvl,
                                                           const Lvl2GPCntFunc lvl2GPCntFunc,
                                                           const std::span<double> out) const {
  assert(out.size() == tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, false));

  if (isNested(lvl2GPCntFunc)) {
    throw base::not_implemented_exception("Unknown level-to-grid-point-count function.");
  } else {
    throw std::invalid_argument(
        "Chebyshev nodes of the first type are not nested w.r.t. the provided "
        "level-to-grid-point-count function.");
  }
}

/******
Getters
******/

QuadRule* FirstTypeChebyshevNodes::getQuadRule(const GPCntType nNodes,
                                               const bool includeBoundary) const {
  return getFejerFirstQuadRule(includeBoundary);
}

InterpolationMethod* FirstTypeChebyshevNodes::getInterpolationMethod(const GPCntType nNodes) const {
  return getBarycentricFormula();
}

bool FirstTypeChebyshevNodes::isNested(const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (lvl2GPCntFunc == linearLvl2GPCntFunction) {
    return false;
  } else if (lvl2GPCntFunc == doublingLvl2GPCntFunction) {
    return false;
  } else {
    throw base::not_implemented_exception(
        "FirstTypeChebyshevNodes::isNested: Unknown level-to-grid-point-count function.");
  }
}

/********
Utilities
********/

NodeIdxLvlMapFunc FirstTypeChebyshevNodes::getNodeIdxMapBetweenLvls(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (isNested(lvl2GPCntFunc)) {
    throw base::not_implemented_exception("Unknown level-to-grid-point-count function.");
  } else {
    throw std::invalid_argument(
        "Chebyshev nodes of the first type are not nested w.r.t. the provided "
        "level-to-grid-point-count function.");
  }
}

NodeIdxLvlMapFunc FirstTypeChebyshevNodes::getNodeIdxMapFromDeltaTGLvlToRegularLvl(
    const Lvl2GPCntFunc lvl2GPCntFunc) const {
  if (isNested(lvl2GPCntFunc)) {
    throw base::not_implemented_exception("Unknown level-to-grid-point-count function.");
  } else {
    throw std::invalid_argument(
        "Chebyshev nodes of the first type are not nested w.r.t. the provided "
        "level-to-grid-point-count function.");
  }
}

/********
Operators
********/

bool FirstTypeChebyshevNodes::operator==(const NodeFamily& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_families

}  // namespace combigrid
}  // namespace sgpp
