// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_operators_test.cpp
 * @brief Boost.Test cases for the textbook operators (@c textbookQuadrature,
 * @c textbookInterpolate, @c textbookInterpolateLinear): equal to the optimized @c SparseGrid
 * operators up to round-off (scalar and vector-valued, node families, boundary-level offsets,
 * domains, complete and anisotropic grids), exact on functions of the grid's span, the same
 * evaluation points (bitwise, as multisets), one @c finishOperatorCall per call, and the
 * argument checks.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_global_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Relative tolerance of textbook vs. optimized (same sums in a different order).
constexpr double REL_TOL = 1e-11;

/**
 * Relative tolerance of the global interpolation with 33 equidistant nodes (level 4 with boundary
 * nodes): the Lebesgue constant is about 1e7 near the ends of the interval, so the product
 * formula of the textbook operator and the barycentric formula of interpolate differ by about
 * eps * 1e7 * |f| (measured: up to 3e-8 relative at the point near the lower corner).
 */
constexpr double EQUI_33_NODES_REL_TOL = 1e-6;

/// Output k: exp(-|x - c_k|^2) * (1 + 0.5 k x_0) - 0.5 + 0.25 k with c_k = 0.2 + 0.15 k.
double component(const DataVector& x, const size_t k) {
  const double center = 0.2 + 0.15 * static_cast<double>(k);
  double sqDist = 0.0;
  for (const double v : x) sqDist += (v - center) * (v - center);
  const double kd = static_cast<double>(k);
  return std::exp(-sqDist) * (1.0 + 0.5 * kd * x[0]) - 0.5 + 0.25 * kd;
}

void components(const DataVector& x, std::span<double> out) {
  for (size_t k = 0; k < out.size(); k++) out[k] = component(x, k);
}

double scalarFunc(const DataVector& x) { return component(x, 0); }

void checkClose(const double textbook, const double optimized, const std::string& what,
                const double relTol = REL_TOL) {
  BOOST_CHECK_MESSAGE(std::abs(textbook - optimized) <= relTol * std::max(1.0, std::abs(optimized)),
                      what << ": textbook " << textbook << " vs. optimized " << optimized);
}

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

NodeGenFunc* nodeGenFuncByName(const std::string& nodes) {
  if (nodes == "equi") return getEquidistantNodeGenFunc();
  if (nodes == "cc") return getClenshawCurtisNodeGenFunc();
  return getFirstTypeChebyshevNodeGenFunc();
}

/// One test configuration: grid generation parameters (complete grid of @c level, or the
/// closure of the anisotropic MIs 3 e_0, (1, ..., 1), 2 e_{d-1} if @c mivec).
struct Config {
  std::string nodes;
  size_t nDim;
  LvlType level;
  LvlType boundary;
  bool unitDomain;
  bool mivec;

  std::string name() const {
    return nodes + " d" + std::to_string(nDim) + (mivec ? " mivec" : " L" + std::to_string(level)) +
           " b" + std::to_string(boundary) + (unitDomain ? " unit" : " shifted");
  }
};

/// Builds the sparse grid of @p config (@p miVec must outlive the grid).
SparseGrid makeGrid(const Config& config, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.mivec) {
    LvlMI first(config.nDim, 0), ones(config.nDim, 1), last(config.nDim, 0);
    first[0] = 3;
    last[config.nDim - 1] = 2;
    miVec = LvlMIVec(std::vector<LvlMI>{first, ones, last});
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(config.level, config.nDim);
  }
  genInstr->setNodeGenFunc(nodeGenFuncByName(config.nodes));
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundary);
  genInstr->setDomain(config.unitDomain ? HyperCubeArea(config.nDim, {0.0, 1.0})
                                        : shiftedDomain(config.nDim));
  return SparseGrid(*genInstr);
}

/// Query points: generic, first coordinate on the domain's midpoint (a node of most grids),
/// near the lower corner (outside the inner nodes of grids without boundary nodes), the upper
/// corner.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  std::vector<DataVector> points(4, DataVector(nDim));
  for (size_t i = 0; i < nDim; i++) {
    const double lower = domain[i].first, width = domain[i].second - domain[i].first;
    const double t = 0.137 + 0.211 * static_cast<double>(i);
    points[0][i] = lower + (t - std::floor(t)) * width;
    points[1][i] = points[0][i];
    points[2][i] = lower + 0.01 * width;
    points[3][i] = domain[i].second;
  }
  points[1][0] = domain[0].first + 0.5 * (domain[0].second - domain[0].first);
  return points;
}

/// Configurations: d 1-4, levels 0-4 (smaller at higher d), b0 / b1 / b2, unit and shifted
/// domains, equidistant / Clenshaw-Curtis / first-type Chebyshev nodes, complete and anisotropic.
std::vector<Config> configs() {
  std::vector<Config> result;
  for (const std::string nodes : {"equi", "cc", "cheb1"}) {
    for (size_t nDim = 1; nDim <= 4; nDim++) {
      const LvlType maxLevel = (nDim <= 2) ? 4 : 3;
      for (LvlType level = 0; level <= maxLevel; level++) {
        for (const LvlType boundary : {0, 1, 2}) {
          for (const bool unitDomain : {true, false}) {
            result.push_back({nodes, nDim, level, boundary, unitDomain, false});
          }
        }
      }
      if (nDim >= 2) {
        result.push_back({nodes, nDim, 0, 1, false, true});
        result.push_back({nodes, nDim, 0, 0, true, true});
      }
    }
  }
  return result;
}

/// Records every evaluation point (bit patterns of its coordinates; thread-safe).
class PointRecorder {
 public:
  double record(const DataVector& x) {
    std::vector<uint64_t> bits;
    for (const double v : x) bits.push_back(std::bit_cast<uint64_t>(v));
    const std::lock_guard<std::mutex> lock(mutex_);
    points_.push_back(std::move(bits));
    return 1.0;
  }

  /// Returns the recorded points (sorted) and starts a new recording.
  std::vector<std::vector<uint64_t>> points() {
    std::vector<std::vector<uint64_t>> result;
    result.swap(points_);
    std::ranges::sort(result);
    return result;
  }

 private:
  std::mutex mutex_;
  std::vector<std::vector<uint64_t>> points_;
};

/// A source function that counts the calls of its end-of-operator hook.
class FinishCountingFunc : public SourceFunc {
 public:
  FinishCountingFunc() : SourceFunc(scalarFunc) {}
  void finishOperatorCall() const override { nFinished++; }
  mutable size_t nFinished = 0;
};

/// The vector-valued counterpart of @ref FinishCountingFunc (m = 2).
class FinishCountingVectorFunc : public VectorSourceFunc {
 public:
  FinishCountingVectorFunc() : VectorSourceFunc(components, 2) {}
  void finishOperatorCall() const override { nFinished++; }
  mutable size_t nFinished = 0;
};

}  // namespace

BOOST_AUTO_TEST_SUITE(textbook_operators)

BOOST_AUTO_TEST_CASE(quadrature_equals_optimized) {
  const SourceFunc f(scalarFunc);
  const VectorSourceFunc vf(components, 3);
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const SparseGrid sg = makeGrid(config, miVec);
    checkClose(textbookQuadrature(f, sg), quadrature(f, sg), "quadrature " + config.name());
    const DataVector textbook = textbookQuadrature(vf, sg), optimized = quadrature(vf, sg);
    for (size_t k = 0; k < 3; k++) {
      checkClose(textbook[k], optimized[k], "vector quadrature " + config.name());
    }
  }
}

BOOST_AUTO_TEST_CASE(global_interpolation_equals_optimized) {
  const SourceFunc f(scalarFunc);
  const VectorSourceFunc vf(components, 3);
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const SparseGrid sg = makeGrid(config, miVec);
    const bool equi33Nodes = config.nodes == "equi" && config.level == 4 && !config.mivec;
    const double relTol = equi33Nodes ? EQUI_33_NODES_REL_TOL : REL_TOL;
    for (const DataVector& point : testPoints(sg.getSetup().getDomain())) {
      checkClose(textbookInterpolate(f, point, sg), interpolate(f, point, sg),
                 "interpolate " + config.name() + " at " + point.toString(), relTol);
      const DataVector textbook = textbookInterpolate(vf, point, sg);
      const DataVector optimized = interpolate(vf, point, sg);
      for (size_t k = 0; k < 3; k++) {
        checkClose(textbook[k], optimized[k], "vector interpolate " + config.name(), relTol);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(linear_interpolation_equals_optimized) {
  const SourceFunc f(scalarFunc);
  const VectorSourceFunc vf(components, 3);
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const SparseGrid sg = makeGrid(config, miVec);
    for (const DataVector& point : testPoints(sg.getSetup().getDomain())) {
      checkClose(textbookInterpolateLinear(f, point, sg), interpolateLinear(f, point, sg),
                 "interpolateLinear " + config.name() + " at " + point.toString());
      const DataVector textbook = textbookInterpolateLinear(vf, point, sg);
      const DataVector optimized = interpolateLinear(vf, point, sg);
      for (size_t k = 0; k < 3; k++) {
        checkClose(textbook[k], optimized[k], "vector interpolateLinear " + config.name());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(scalar_equals_vector_output) {
  // Output 0 of the vector-valued function is the scalar function: one computation for both
  const SourceFunc f(scalarFunc);
  const VectorSourceFunc vf(components, 2);
  LvlMIVec miVec(3, 0);
  const SparseGrid sg = makeGrid({"cc", 3, 3, 1, false, false}, miVec);
  const DataVector point = testPoints(sg.getSetup().getDomain())[0];
  BOOST_CHECK_EQUAL(textbookQuadrature(f, sg), textbookQuadrature(vf, sg)[0]);
  BOOST_CHECK_EQUAL(textbookInterpolate(f, point, sg), textbookInterpolate(vf, point, sg)[0]);
  BOOST_CHECK_EQUAL(textbookInterpolateLinear(f, point, sg),
                    textbookInterpolateLinear(vf, point, sg)[0]);
}

BOOST_AUTO_TEST_CASE(exact_on_the_span_of_the_grid) {
  // Complete Clenshaw-Curtis grid of level 3 in 2D with boundary: the tensor grid (2, 1) has 9 x 5
  // nodes, so x^7 y^3 lies in the span; Clenshaw-Curtis with n nodes integrates degree n - 1
  const SourceFunc poly([](const DataVector& x) { return std::pow(x[0], 7) * std::pow(x[1], 3); });
  LvlMIVec miVec(2, 0);
  const SparseGrid sg = makeGrid({"cc", 2, 3, 0, false, false}, miVec);
  const HyperCubeArea& domain = sg.getSetup().getDomain();
  const auto [a0, b0] = domain[0];
  const auto [a1, b1] = domain[1];
  const double exactIntegral =
      (std::pow(b0, 8) - std::pow(a0, 8)) / 8 * (std::pow(b1, 4) - std::pow(a1, 4)) / 4;
  BOOST_CHECK_CLOSE_FRACTION(textbookQuadrature(poly, sg), exactIntegral, 1e-13);

  // Absolute tolerance: |x^7 y^3| <= 1.2^10 < 7 on the domain, values near 0 occur
  for (const DataVector& point : testPoints(domain)) {
    BOOST_CHECK_SMALL(textbookInterpolate(poly, point, sg) - poly.evaluate(point), 1e-11);
  }

  // Multilinear functions: every tensor grid reproduces them, also with cells that reach to the
  // boundary (equidistant nodes without boundary nodes)
  const SourceFunc multilinear(
      [](const DataVector& x) { return 1.0 + 2.0 * x[0] - x[1] + 3.0 * x[0] * x[1] * x[2]; });
  const SparseGrid sgLin = makeGrid({"equi", 3, 3, 1, false, false}, miVec);
  for (const DataVector& point : testPoints(sgLin.getSetup().getDomain())) {
    BOOST_CHECK_CLOSE_FRACTION(textbookInterpolateLinear(multilinear, point, sgLin),
                               multilinear.evaluate(point), 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(same_evaluation_points_as_optimized) {
  for (const Config& config : configs()) {
    if (config.nDim > 3) continue;
    LvlMIVec miVec(config.nDim, 0);
    const SparseGrid sg = makeGrid(config, miVec);
    const DataVector point = testPoints(sg.getSetup().getDomain())[1];
    PointRecorder textbook, optimized;
    const SourceFunc textbookFunc([&](const DataVector& x) { return textbook.record(x); });
    const SourceFunc optimizedFunc([&](const DataVector& x) { return optimized.record(x); });

    textbookQuadrature(textbookFunc, sg);
    quadrature(optimizedFunc, sg);
    BOOST_CHECK_MESSAGE(textbook.points() == optimized.points(), "quadrature " + config.name());

    textbookInterpolate(textbookFunc, point, sg);
    interpolate(optimizedFunc, point, sg);
    BOOST_CHECK_MESSAGE(textbook.points() == optimized.points(), "interpolate " + config.name());

    textbookInterpolateLinear(textbookFunc, point, sg);
    interpolateLinear(optimizedFunc, point, sg);
    BOOST_CHECK_MESSAGE(textbook.points() == optimized.points(),
                        "interpolateLinear " + config.name());
  }
}

BOOST_AUTO_TEST_CASE(finish_operator_call_once_per_call) {
  LvlMIVec miVec(2, 0);
  const SparseGrid sg = makeGrid({"cc", 2, 2, 0, true, false}, miVec);
  const DataVector point(2, 0.3);
  FinishCountingFunc f;
  FinishCountingVectorFunc vf;

  textbookQuadrature(f, sg);
  textbookInterpolate(f, point, sg);
  textbookInterpolateLinear(f, point, sg);
  BOOST_CHECK_EQUAL(f.nFinished, 3);

  textbookQuadrature(vf, sg);
  textbookInterpolate(vf, point, sg);
  textbookInterpolateLinear(vf, point, sg);
  BOOST_CHECK_EQUAL(vf.nFinished, 3);
}

BOOST_AUTO_TEST_CASE(argument_checks) {
  LvlMIVec miVec(2, 0);
  const SparseGrid sg = makeGrid({"cc", 2, 2, 0, true, false}, miVec);
  const SourceFunc f(scalarFunc);
  const DataVector wrongPoint(3, 0.3);

  BOOST_CHECK_THROW(textbookInterpolate(f, wrongPoint, sg), std::invalid_argument);
  BOOST_CHECK_THROW(textbookInterpolateLinear(f, wrongPoint, sg), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
