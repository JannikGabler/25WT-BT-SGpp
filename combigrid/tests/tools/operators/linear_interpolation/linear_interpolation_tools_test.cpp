// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_tools_test.cpp
 * @brief Boost.Test cases pinning the per-tensor-grid kernel of the linear interpolation
 * (@c tools::lin_interpolation::processTensorGrid) against a direct implementation of the
 * definition: corner @c c of the grid cell containing the query point (bit k of @c c selects
 * the upper neighbor in dimension k) has the weight
 * @f$\prod_{k=0}^{d-1} w_k@f$ (multiplied in this order) with
 * @f$w_k = (b_k - x_k) / (b_k - a_k)@f$ resp. @f$(x_k - a_k) / (b_k - a_k)@f$, or 0.5 if the
 * cell has zero extent in dimension k, and the corners are visited in the order of @c c.
 *
 * Pinned bitwise: the sequence of evaluated points (count, order and coordinates after the
 * mapping onto the domain) and every corner weight. A weight is observed exactly as the result
 * for a source function that is 1 at the corner and 0 at all others (no rounding in the sum,
 * independent of FMA contraction). The interpolated values are compared within round-off.
 * Configurations: equidistant and Clenshaw-Curtis nodes, boundary level offsets 0 / 1 / 2, unit
 * and shifted domains, d = 1-4 (complete and anisotropic MIVec grids) and d = 12, query points
 * in the interior, on nodes and on the boundary of the domain, scalar and vector-valued (m = 3)
 * source functions.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

constexpr double EPS = std::numeric_limits<double>::epsilon();

/// Non-polynomial, not separable test function.
double testFunc(const DataVector& x) {
  double sqDist = 0.0;
  for (size_t i = 0; i < x.size(); i++) {
    const double c = 0.3 + 0.1 * static_cast<double>(i);
    sqDist += (x[i] - c) * (x[i] - c);
  }
  return std::exp(-sqDist) * (1.0 + 0.5 * x[0]) - 0.4 + 0.25 * x[x.size() - 1];
}

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

/// Anisotropic MIs (closure computed by MIVecSGGenInstr): 3 e_0, (1, ..., 1), 2 e_{d-1}.
LvlMIVec anisotropicMIs(const size_t nDim) {
  LvlMI first(nDim, 0), ones(nDim, 1), last(nDim, 0);
  first[0] = 3;
  last[nDim - 1] = 2;
  return LvlMIVec(std::vector<LvlMI>{first, ones, last});
}

/// Generation instruction (complete grid of level @p maxLvl, or anisotropicMIs stored in
/// @p miVec, which must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const std::string& nodes, const LvlType boundary,
                                         const bool unitDomain, const size_t nDim, const bool mivec,
                                         const LvlType maxLvl, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (mivec) {
    miVec = anisotropicMIs(nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(maxLvl, nDim);
  }
  genInstr->setNodeGenFunc(nodes == "equi" ? getEquidistantNodeGenFunc()
                                           : getClenshawCurtisNodeGenFunc());
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(boundary);
  genInstr->setDomain(unitDomain ? HyperCubeArea(nDim, {0.0, 1.0}) : shiftedDomain(nDim));
  return genInstr;
}

/// Query points (problem coordinates): generic interior, first coordinate on a node (the
/// midpoint), all coordinates on nodes, the lower and the upper corner of the domain, and a
/// point with the first coordinate on the lower and the others on the upper boundary.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  DataVector generic(nDim), lower(nDim), upper(nDim);
  for (size_t i = 0; i < nDim; i++) {
    const double t = 0.137 + 0.211 * static_cast<double>(i);
    generic[i] = domain[i].first + (t - std::floor(t)) * (domain[i].second - domain[i].first);
    lower[i] = domain[i].first;
    upper[i] = domain[i].second;
  }
  DataVector onNode(generic), allOnNodes(nDim), mixed(upper);
  onNode[0] = 0.5 * (domain[0].first + domain[0].second);
  for (size_t i = 0; i < nDim; i++) {
    allOnNodes[i] = domain[i].first + 0.25 * (domain[i].second - domain[i].first);
  }
  mixed[0] = domain[0].first;
  return {generic, onNode, allOnNodes, lower, upper, mixed};
}

/// One corner of the reference: point on the domain and weight.
struct RefCorner {
  DataVector point;
  double weight;
};

/// The corners of the cell of @p normPoint in @p tg, in the order of their index (definition
/// in the file comment), mapped onto @p domain with tools::denormalizeDataVector.
std::vector<RefCorner> referenceCorners(const TensorGrid& tg, const DataVector& normPoint,
                                        const HyperCubeArea& domain) {
  const size_t nDim = tg.nDim();
  std::vector<std::pair<double, double>> nb(nDim);
  tg.getNeighborsForLinInterpolation(normPoint, nb);
  std::vector<RefCorner> corners;

  for (size_t idx = 0; idx < (size_t{1} << nDim); idx++) {
    DataVector point(nDim);
    double weight = 1.0;
    for (size_t k = 0; k < nDim; k++) {
      const double length = nb[k].second - nb[k].first;
      const bool upper = (idx >> k) & 1;
      if (length == 0.0) {
        weight *= 0.5;
        point[k] = nb[k].first;
      } else if (upper) {
        weight *= (normPoint[k] - nb[k].first) / length;
        point[k] = nb[k].second;
      } else {
        weight *= (nb[k].second - normPoint[k]) / length;
        point[k] = nb[k].first;
      }
    }
    tools::denormalizeDataVector(point, domain);
    corners.push_back({point, weight});
  }
  return corners;
}

/// Bitwise equality of two points.
bool bitwiseEqual(const DataVector& a, const DataVector& b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::bit_cast<uint64_t>(a[i]) != std::bit_cast<uint64_t>(b[i])) return false;
  }
  return true;
}

/// Bitwise equality of two doubles (BOOST_CHECK with both values in the message).
void checkBitwise(const double actual, const double expected) {
  BOOST_CHECK_MESSAGE(std::bit_cast<uint64_t>(actual) == std::bit_cast<uint64_t>(expected),
                      "actual " << actual << " expected " << expected);
}

/// Corner indices whose weights are checked: all for up to 64 corners, else 64 spread ones
/// (including the first and the last).
std::vector<size_t> checkedCorners(const size_t nCorners) {
  std::vector<size_t> result;
  const size_t stride = nCorners <= 64 ? 1 : nCorners / 63;
  for (size_t c = 0; c < nCorners; c += stride) result.push_back(c);
  if (result.back() != nCorners - 1) result.push_back(nCorners - 1);
  return result;
}

/// Scalar kernel: evaluated points (bitwise, in order), weights (bitwise) and value; returns
/// the kernel's value and its round-off bound.
std::pair<double, double> checkScalarKernel(const TensorGridCTData& tgData,
                                            const SGGenInstr& genInstr, const DataVector& normPoint,
                                            const std::vector<RefCorner>& ref) {
  misc::LinInterpScratch scratch(tgData.tensorGrid.nDim());
  std::vector<DataVector> points;
  const SourceFunc recording([&points](const DataVector& x) {
    points.push_back(x);
    return testFunc(x);
  });
  const double value =
      tools::lin_interpolation::processTensorGrid(recording, normPoint, tgData, genInstr, scratch);

  BOOST_REQUIRE_EQUAL(points.size(), ref.size());
  double expected = 0.0, scale = 0.0;
  for (size_t c = 0; c < ref.size(); c++) {
    BOOST_CHECK_MESSAGE(bitwiseEqual(points[c], ref[c].point), "corner " << c);
    expected += ref[c].weight * testFunc(ref[c].point);
    scale += std::abs(ref[c].weight * testFunc(ref[c].point));
  }
  BOOST_CHECK_SMALL(value - expected, 4.0 * EPS * scale + 1e-300);
  const std::pair<double, double> result = {value, 4.0 * EPS * scale};

  for (const size_t corner : checkedCorners(ref.size())) {
    size_t count = 0;
    const SourceFunc indicator(
        [&count, corner](const DataVector&) { return (count++ == corner) ? 1.0 : 0.0; });
    BOOST_TEST_CONTEXT("corner " << corner) {
      checkBitwise(tools::lin_interpolation::processTensorGrid(indicator, normPoint, tgData,
                                                               genInstr, scratch),
                   ref[corner].weight);
    }
  }
  return result;
}

/// Vector kernel (m = 3): output k is the indicator of corner (k * nCorners) / 3; the outputs
/// must equal those weights bitwise, the evaluated points those of the reference.
void checkVectorKernel(const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       const DataVector& normPoint, const std::vector<RefCorner>& ref) {
  constexpr size_t N_OUTPUTS = 3;
  misc::LinInterpScratch scratch(tgData.tensorGrid.nDim(), N_OUTPUTS);
  std::vector<DataVector> points;
  const size_t nCorners = ref.size();
  const VectorSourceFunc indicators(
      [&points, nCorners](const DataVector& x, std::span<double> out) {
        const size_t c = points.size();
        points.push_back(x);
        for (size_t k = 0; k < out.size(); k++) out[k] = (c == k * nCorners / 3) ? 1.0 : 0.0;
      },
      N_OUTPUTS);
  std::vector<double> out(N_OUTPUTS, std::numeric_limits<double>::quiet_NaN());
  tools::lin_interpolation::processTensorGrid(indicators, normPoint, tgData, genInstr, scratch,
                                              out);

  BOOST_REQUIRE_EQUAL(points.size(), nCorners);
  for (size_t c = 0; c < nCorners; c++) {
    BOOST_CHECK_MESSAGE(bitwiseEqual(points[c], ref[c].point), "corner " << c);
  }
  for (size_t k = 0; k < N_OUTPUTS; k++) checkBitwise(out[k], ref[k * nCorners / 3].weight);
}

/// Checks every tensor grid of @p sg at @p point, and the operator against the combination of
/// the kernel values (any order of the combination).
void checkGrid(const SparseGrid& sg, const DataVector& point) {
  const SGGenInstr& genInstr = sg.getGenInstr();
  const DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  double expected = 0.0, absSum = 0.0, tgBounds = 0.0;

  for (const TensorGridCTData& tgData : sg) {
    const std::vector<RefCorner> ref =
        referenceCorners(tgData.tensorGrid, normPoint, genInstr.getDomain());
    BOOST_TEST_CONTEXT("mi " << tgData.mi.size() << "-dim, coefficient " << tgData.coefficient) {
      const auto [tgValue, tgBound] = checkScalarKernel(tgData, genInstr, normPoint, ref);
      checkVectorKernel(tgData, genInstr, normPoint, ref);
      expected += tgData.coefficient * tgValue;
      absSum += std::abs(tgData.coefficient * tgValue);
      tgBounds += std::abs(tgData.coefficient) * tgBound;
    }
  }

  const double value = interpolateLinear(SourceFunc(testFunc), point, sg);
  const double nTG = static_cast<double>(sg.nTG());
  BOOST_CHECK_SMALL(value - expected, 2.0 * nTG * EPS * absSum + tgBounds + 1e-300);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(linear_interpolation_tools)

BOOST_AUTO_TEST_CASE(Kernel_Equals_Definition_Low_Dimensions) {
  for (const std::string nodes : {"equi", "cc"}) {
    for (const LvlType boundary : {0u, 1u, 2u}) {
      for (const bool unitDomain : {true, false}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (const bool mivec : {false, true}) {
            LvlMIVec miVec(nDim, 0);
            const std::unique_ptr<SGGenInstr> genInstr =
                makeGenInstr(nodes, boundary, unitDomain, nDim, mivec, 3, miVec);
            const SparseGrid sg(*genInstr);
            for (const DataVector& point : testPoints(genInstr->getDomain())) {
              BOOST_TEST_CONTEXT(nodes << " b" << boundary << " unit " << unitDomain << " d" << nDim
                                       << " mivec " << mivec << " point " << point.toString()) {
                checkGrid(sg, point);
              }
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Kernel_Equals_Definition_High_Dimension) {
  constexpr size_t N_DIM = 12;
  for (const LvlType boundary : {0u, 1u}) {
    LvlMIVec miVec(N_DIM, 0);
    const std::unique_ptr<SGGenInstr> genInstr =
        makeGenInstr("equi", boundary, false, N_DIM, false, 1, miVec);
    const SparseGrid sg(*genInstr);
    const std::vector<DataVector> points = testPoints(genInstr->getDomain());
    for (const size_t p : {0, 1, 4}) {
      BOOST_TEST_CONTEXT("b" << boundary << " point " << points[p].toString()) {
        checkGrid(sg, points[p]);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
