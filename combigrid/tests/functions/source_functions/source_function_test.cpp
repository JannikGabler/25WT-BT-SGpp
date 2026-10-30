// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function_test.cpp
 * @brief Boost.Test cases for @c SourceFunc and for the points the operators pass to it.
 *
 * The second suite pins which bit patterns the operators pass to the source function for the
 * same grid point (the property a cache keyed by the user-domain point relies on, WP-25): SG and
 * DSG quadrature use identical patterns, global and multilinear interpolation too; between the
 * two families they are identical on the unit domain only.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <bit>
#include <cstdint>
#include <functional>
#include <mutex>
#include <set>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;

namespace {

double weightedSum(const DataVector& x) {
  double sum = 0;
  for (size_t k = 0; k < x.size(); k++) sum += static_cast<double>(k + 1) * x[k];
  return sum;
}

/// Dimension k: [-0.5 + 0.1 k, 1.0 + 0.2 k] (as the shifted domain of the perf suite).
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t k = 0; k < nDim; k++) {
    domain[k] = {-0.5 + 0.1 * static_cast<double>(k), 1.0 + 0.2 * static_cast<double>(k)};
  }
  return domain;
}

/// Bit patterns of every point a source function was evaluated at (thread-safe).
class PointRecorder {
 public:
  SourceFunc func() {
    return SourceFunc([this](const DataVector& x) {
      std::vector<uint64_t> bits(x.size());
      for (size_t k = 0; k < x.size(); k++) bits[k] = std::bit_cast<uint64_t>(x[k]);
      const std::lock_guard<std::mutex> guard(mutex_);
      points_.insert(std::move(bits));
      evaluations_++;
      return 1.0;
    });
  }

  const std::set<std::vector<uint64_t>>& points() const { return points_; }
  size_t evaluations() const { return evaluations_; }

 private:
  std::mutex mutex_;
  std::set<std::vector<uint64_t>> points_;
  size_t evaluations_ = 0;
};

/// Whether a coordinate of @p bits is a bound of @p domain (as mapped from 0 and 1).
bool onDomainBoundary(const std::vector<uint64_t>& bits, const HyperCubeArea& domain) {
  for (size_t k = 0; k < bits.size(); k++) {
    const double x = std::bit_cast<double>(bits[k]);
    const double width = domain[k].second - domain[k].first;
    if (x == domain[k].first || x == width + domain[k].first) return true;
  }
  return false;
}

void checkOperatorsEvaluateIdenticalPoints(NodeGenFunc* nodes, const LvlType boundary,
                                           const size_t nDim, const LvlType maxLvl,
                                           const HyperCubeArea& domain) {
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(nodes);
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(boundary);
  genInstr.setDomain(domain);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  bool unitDomain = true;
  for (const auto& [lower, upper] : domain) unitDomain = unitDomain && lower == 0.0 && upper == 1.0;

  PointRecorder quad, deltaQuad, interp, interpLin;
  sgpp::combigrid::quadrature(quad.func(), sg);
  sgpp::combigrid::quadrature(deltaQuad.func(), dsg);

  DataVector point(nDim);
  for (size_t k = 0; k < nDim; k++) {
    point[k] = domain[k].first + 0.3 * (domain[k].second - domain[k].first);
  }
  sgpp::combigrid::interpolate(interp.func(), point, sg);
  for (const double t : {0.0, 0.17, 0.5, 0.81, 1.0}) {
    for (size_t k = 0; k < nDim; k++) {
      point[k] = domain[k].first + t * (domain[k].second - domain[k].first);
    }
    sgpp::combigrid::interpolateLinear(interpLin.func(), point, sg);
  }

  // The DSG evaluates every distinct point exactly once, SG quadrature the same points (with
  // repetitions; both map the nodes of a tensor grid onto the domain with u * width + lower).
  BOOST_CHECK_EQUAL(deltaQuad.evaluations(), deltaQuad.points().size());
  BOOST_CHECK(quad.points() == deltaQuad.points());

  // Global interpolation evaluates every grid point with one bit pattern (shared points of
  // different tensor grids are identical). It maps with tools::denormalizeDataVector, which is
  // not contracted to an FMA (the store to the point may alias the domain), while the quadrature
  // mappings are where the target has FMA: on non-unit domains the patterns differ by up to one
  // ulp between the two families (judged build); on the unit domain both are exact.
  BOOST_CHECK_EQUAL(interp.points().size(), quad.points().size());
  if (unitDomain) BOOST_CHECK(interp.points() == quad.points());

  // Multilinear interpolation maps like global interpolation and evaluates grid points and, in
  // tensor grids without boundary nodes in a dimension, points on the domain boundary there (it
  // interpolates towards the boundary), which are no grid points.
  for (const std::vector<uint64_t>& p : interpLin.points()) {
    BOOST_CHECK(interp.points().count(p) == 1 || (boundary > 0 && onDomainBoundary(p, domain)));
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(source_function)

BOOST_AUTO_TEST_CASE(evaluate_forwards_to_the_callable) {
  const std::function<double(const DataVector&)> callable(weightedSum);
  const SourceFunc byCopy(callable);
  const SourceFunc byMove([](const DataVector& x) { return weightedSum(x); });
  const DataVector x{0.25, -1.5, 3.0};

  BOOST_CHECK_EQUAL(byCopy.evaluate(x), weightedSum(x));
  BOOST_CHECK_EQUAL(byMove.evaluate(x), weightedSum(x));
}

BOOST_AUTO_TEST_CASE(copies_call_copies_of_the_callable) {
  // State captured by value is not shared between copies (a cache shared by copies must be
  // held through a pointer).
  const SourceFunc original([count = 0](const DataVector&) mutable { return ++count; });
  const SourceFunc copy(original);
  const DataVector x{0.5};

  BOOST_CHECK_EQUAL(original.evaluate(x), 1.0);
  BOOST_CHECK_EQUAL(original.evaluate(x), 2.0);
  BOOST_CHECK_EQUAL(copy.evaluate(x), 1.0);
}

BOOST_AUTO_TEST_CASE(evaluate_normalized_maps_onto_the_domain) {
  const SourceFunc f(weightedSum);

  for (const size_t nDim : {1, 3}) {
    const HyperCubeArea domain = shiftedDomain(nDim);
    DataVector unit(nDim);
    DataVector mapped(nDim);
    for (size_t k = 0; k < nDim; k++) {
      unit[k] = 0.125 + 0.25 * static_cast<double>(k);
      mapped[k] = unit[k] * (domain[k].second - domain[k].first) + domain[k].first;
    }

    const DataVector unitBefore(unit);
    const double outOfPlace = f.evaluateNormalizedOutOfPlace(unit, domain);
    BOOST_CHECK(unit == unitBefore);  // input unchanged

    const double inPlace = f.evaluateNormalizedInPlace(unit, domain);
    BOOST_CHECK_EQUAL(inPlace, outOfPlace);
    for (size_t k = 0; k < nDim; k++) {
      BOOST_CHECK_CLOSE(unit[k], mapped[k], 1e-13);  // overwritten with the mapped point
    }
    BOOST_CHECK_CLOSE(inPlace, weightedSum(mapped), 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(source_function_points_of_the_operators)

BOOST_AUTO_TEST_CASE(identical_points_unit_domain) {
  for (NodeGenFunc* nodes : {sgpp::combigrid::getEquidistantNodeGenFunc(),
                             sgpp::combigrid::getClenshawCurtisNodeGenFunc()}) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const size_t nDim : {1, 2, 3}) {
        checkOperatorsEvaluateIdenticalPoints(nodes, boundary, nDim, 4,
                                              HyperCubeArea(nDim, {0.0, 1.0}));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(identical_points_shifted_domain) {
  for (NodeGenFunc* nodes : {sgpp::combigrid::getEquidistantNodeGenFunc(),
                             sgpp::combigrid::getClenshawCurtisNodeGenFunc()}) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const size_t nDim : {1, 2, 3}) {
        checkOperatorsEvaluateIdenticalPoints(nodes, boundary, nDim, 4, shiftedDomain(nDim));
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
