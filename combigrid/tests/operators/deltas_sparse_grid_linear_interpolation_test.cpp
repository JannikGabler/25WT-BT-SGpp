// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_test.cpp
 * @brief Boost.Test cases for the linear interpolation operator on a deltas sparse grid
 * (@c interpolateLinear on a @c DeltasSparseGrid) and its index (@c misc::DSGLinInterpIndex).
 *
 * The operator computes the same combination-technique interpolant as @c interpolateLinear on the
 * @c SparseGrid of the same generation instruction, but evaluates every distinct corner once. The
 * tests compare both operators (up to round-off) over node families, boundary level offsets,
 * domains, grids, dimensions (incl. d > @c WEIGHT_TABLE_MAX_DIM) and query points, count the
 * evaluations (each point the SparseGrid operator evaluates exactly once, no other point) and
 * compare the vector-valued overloads with the scalar one.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Non-polynomial, not separable test function.
double testFunc(const DataVector& x) {
  double sqDist = 0.0;
  for (size_t i = 0; i < x.size(); i++) {
    const double c = 0.3 + 0.1 * static_cast<double>(i);
    sqDist += (x[i] - c) * (x[i] - c);
  }
  return std::exp(-sqDist) * (1.0 + 0.5 * x[0]) + 0.25 * x[x.size() - 1];
}

/// Three outputs: testFunc, a product of cosines, a constant.
void testFuncVec(const DataVector& x, const std::span<double> out) {
  out[0] = testFunc(x);
  double prod = 1.0;
  for (size_t i = 0; i < x.size(); i++) prod *= std::cos(x[i] + 0.1 * static_cast<double>(i));
  out[1] = prod;
  out[2] = 2.5;
}

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

/// One test configuration (complete grid of level maxLvl, or anisotropic MIs).
struct Config {
  std::string nodes;
  LvlType boundary;
  bool unitDomain;
  size_t nDim;
  bool mivec;
  LvlType maxLvl = 3;
};

/// Anisotropic MIs (closure computed by MIVecSGGenInstr): 3 e_0, (1, ..., 1), 2 e_{d-1}, and
/// (2, 1, 0, ...) for d >= 2.
LvlMIVec anisotropicMIs(const size_t nDim) {
  LvlMI first(nDim, 0), ones(nDim, 1), last(nDim, 0), mixed(nDim, 0);
  first[0] = 3;
  last[nDim - 1] = 2;
  std::vector<LvlMI> mis{first, ones, last};
  if (nDim >= 2) {
    mixed[0] = 2;
    mixed[1] = 1;
    mis.push_back(mixed);
  }
  return LvlMIVec(mis);
}

/// The generation instruction of @p config (@p miVec: storage of the MIs, must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const Config& config, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.mivec) {
    miVec = anisotropicMIs(config.nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(config.maxLvl, config.nDim);
  }
  genInstr->setNodeGenFunc(config.nodes == "cc" ? getClenshawCurtisNodeGenFunc()
                                                : getEquidistantNodeGenFunc());
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundary);
  genInstr->setDomain(config.unitDomain ? HyperCubeArea(config.nDim, {0.0, 1.0})
                                        : shiftedDomain(config.nDim));
  return genInstr;
}

/// Query points: two interior ones, one on nodes of every level (the domain's midpoint in every
/// coordinate), one on the domain's boundary (lower and upper bounds alternating), one mixing a
/// node, the boundary and interior coordinates.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  std::vector<DataVector> points(5, DataVector(nDim));
  for (size_t i = 0; i < nDim; i++) {
    const double lo = domain[i].first, width = domain[i].second - domain[i].first;
    const double di = static_cast<double>(i);
    points[0][i] = lo + width * std::fmod(0.137 + 0.311 * di, 1.0);
    points[1][i] = lo + width * std::fmod(0.871 + 0.577 * di, 1.0);
    points[2][i] = lo + 0.5 * width;
    points[3][i] = (i % 2 == 0) ? lo : domain[i].second;
    points[4][i] = (i % 3 == 0)   ? lo + 0.25 * width
                   : (i % 3 == 1) ? domain[i].second
                                  : lo + width * 0.6180339887;
  }
  return points;
}

/// Combinations of nodes, boundary offsets, domains, dimensions 1-4 and grids.
std::vector<Config> lowDimConfigs() {
  std::vector<Config> result;
  for (const std::string nodes : {"equi", "cc"}) {
    for (const LvlType boundary : {0u, 1u, 2u}) {
      for (const bool unitDomain : {true, false}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (const bool mivec : {false, true}) {
            result.push_back({nodes, boundary, unitDomain, nDim, mivec});
          }
        }
      }
    }
  }
  return result;
}

/// Larger grids: parallel passes (d5 L5), the weight table exceeded (d11, d12), a deep level.
std::vector<Config> largeConfigs() {
  return {{"equi", 1, false, 5, false, 5},  {"cc", 0, true, 5, false, 4},
          {"equi", 1, false, 11, false, 2}, {"cc", 2, false, 12, false, 2},
          {"equi", 0, false, 2, false, 9},  {"equi", 1, true, 1, false, 7}};
}

std::string describe(const Config& c, const size_t pointIdx) {
  return c.nodes + " b" + std::to_string(c.boundary) + (c.unitDomain ? " unit" : " shifted") +
         " d" + std::to_string(c.nDim) + (c.mivec ? " mivec" : " L" + std::to_string(c.maxLvl)) +
         " point " + std::to_string(pointIdx);
}

/// Sum of |c_l| (scale of the round-off of a combination).
double sumAbsCoeffs(const SparseGrid& sg) {
  double sum = 0.0;
  for (const TensorGridCTData& tgData : sg) sum += std::abs(tgData.coefficient);
  return sum;
}

void checkEqualsSparseGridOperator(const std::vector<Config>& configs) {
  for (const Config& config : configs) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);
    const DeltasSparseGrid dsg(*genInstr);
    const double tol = 1e-14 * (1.0 + sumAbsCoeffs(sg)) * 4.0;
    const std::vector<DataVector> points = testPoints(genInstr->getDomain());

    for (size_t p = 0; p < points.size(); p++) {
      const double expected = interpolateLinear(SourceFunc(testFunc), points[p], sg);
      const double result = interpolateLinear(SourceFunc(testFunc), points[p], dsg);
      BOOST_TEST_CONTEXT(describe(config, p)) {
        BOOST_CHECK_SMALL(result - expected, tol * (1.0 + std::abs(expected)));
      }
    }
  }
}

/// Bit pattern of a point (key of the evaluation counts).
std::vector<uint64_t> keyOf(const DataVector& x) {
  std::vector<uint64_t> key(x.size());
  for (size_t i = 0; i < x.size(); i++) key[i] = std::bit_cast<uint64_t>(x[i]);
  return key;
}

/// Source function recording its evaluation points (per OpenMP thread, no locking); counts()
/// returns the number of evaluations per point.
struct CountingFunc {
  std::vector<std::vector<std::vector<uint64_t>>> keysPerThread =
      std::vector<std::vector<std::vector<uint64_t>>>(omp_get_max_threads());

  SourceFunc sourceFunc() {
    return SourceFunc([this](const DataVector& x) {
      keysPerThread[omp_get_thread_num()].push_back(keyOf(x));
      return testFunc(x);
    });
  }

  std::map<std::vector<uint64_t>, size_t> counts() const {
    std::map<std::vector<uint64_t>, size_t> result;
    for (const auto& keys : keysPerThread) {
      for (const auto& key : keys) result[key]++;
    }
    return result;
  }
};

}  // namespace

BOOST_AUTO_TEST_SUITE(dsg_linear_interpolation)

BOOST_AUTO_TEST_CASE(Equals_SparseGrid_Operator_Low_Dimensions) {
  checkEqualsSparseGridOperator(lowDimConfigs());
}

BOOST_AUTO_TEST_CASE(Equals_SparseGrid_Operator_Large_Grids) {
  checkEqualsSparseGridOperator(largeConfigs());
}

BOOST_AUTO_TEST_CASE(Each_Distinct_Corner_Evaluated_Once) {
  // The large configurations without d12 (d11 already exceeds the weight table; counting the
  // points is slow in debug builds)
  std::vector<Config> configs = lowDimConfigs();
  for (const Config& config : largeConfigs()) {
    if (config.nDim < 12) configs.push_back(config);
  }

  for (const Config& config : configs) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);
    const DeltasSparseGrid dsg(*genInstr);
    const std::vector<DataVector> points = testPoints(genInstr->getDomain());

    for (size_t p = 0; p < points.size(); p++) {
      CountingFunc sgCounts, dsgCounts;
      interpolateLinear(sgCounts.sourceFunc(), points[p], sg);
      interpolateLinear(dsgCounts.sourceFunc(), points[p], dsg);

      const std::map<std::vector<uint64_t>, size_t> sgPoints = sgCounts.counts();
      const std::map<std::vector<uint64_t>, size_t> dsgPoints = dsgCounts.counts();

      BOOST_TEST_CONTEXT(describe(config, p)) {
        BOOST_CHECK_EQUAL(dsgPoints.size(), sgPoints.size());
        size_t nRepeated = 0, nForeign = 0;
        for (const auto& [key, count] : dsgPoints) {
          nRepeated += (count != 1);
          nForeign += (sgPoints.count(key) == 0);
        }
        BOOST_CHECK_EQUAL(nRepeated, 0u);
        BOOST_CHECK_EQUAL(nForeign, 0u);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Vector_Outputs_Equal_Scalar_Interpolation) {
  const std::vector<Config> configs = {{"equi", 1, false, 3, false},
                                       {"cc", 0, true, 2, true},
                                       {"equi", 2, false, 4, true},
                                       {"equi", 1, false, 5, false, 5},
                                       {"cc", 1, false, 11, false, 2}};
  const VectorSourceFunc vecFunc(testFuncVec, 3);

  for (const Config& config : configs) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);
    const DeltasSparseGrid dsg(*genInstr);
    const double tol = 1e-14 * (1.0 + sumAbsCoeffs(sg)) * 4.0;

    for (const DataVector& point : testPoints(genInstr->getDomain())) {
      const DataVector result = interpolateLinear(vecFunc, point, dsg);
      const DataVector sgResult = interpolateLinear(vecFunc, point, sg);
      for (size_t k = 0; k < 3; k++) {
        const SourceFunc component([&vecFunc, k](const DataVector& x) {
          std::vector<double> out(3);
          vecFunc.evaluate(x, out);
          return out[k];
        });
        const double scalar = interpolateLinear(component, point, dsg);
        BOOST_TEST_CONTEXT(describe(config, 0) << " output " << k) {
          BOOST_CHECK_SMALL(result[k] - scalar, tol * (1.0 + std::abs(scalar)));
          BOOST_CHECK_SMALL(result[k] - sgResult[k], tol * (1.0 + std::abs(scalar)));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Wrong_Result_Size_Is_Rejected) {
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  const DeltasSparseGrid dsg(genInstr);
  const VectorSourceFunc vecFunc(testFuncVec, 3);
  const DataVector point(2, 0.3);
  std::vector<double> tooSmall(2), tooLarge(4);
  BOOST_CHECK_THROW(interpolateLinear(vecFunc, point, dsg, std::span<double>(tooSmall)),
                    std::invalid_argument);
  BOOST_CHECK_THROW(interpolateLinear(vecFunc, point, dsg, std::span<double>(tooLarge)),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Concurrent_First_Calls_Share_One_Index) {
  Config config{"equi", 1, false, 4, false, 4};
  LvlMIVec miVec(config.nDim, 0);
  const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
  const SparseGrid sg(*genInstr);
  const DeltasSparseGrid dsg(*genInstr);
  const std::vector<DataVector> points = testPoints(genInstr->getDomain());
  std::vector<double> results(4 * points.size());

#pragma omp parallel for num_threads(4) schedule(static, 1)
  for (size_t i = 0; i < results.size(); i++) {
    results[i] = interpolateLinear(SourceFunc(testFunc), points[i % points.size()], dsg);
  }

  for (size_t i = 0; i < results.size(); i++) {
    const double expected = interpolateLinear(SourceFunc(testFunc), points[i % points.size()], sg);
    BOOST_CHECK_SMALL(results[i] - expected, 1e-12 * (1.0 + std::abs(expected)));
  }
  BOOST_CHECK_EQUAL(&tools::dsg_lin_interpolation::getIndex(dsg),
                    &tools::dsg_lin_interpolation::getIndex(dsg));
}

BOOST_AUTO_TEST_CASE(Index_Slots_Of_Doubling_Growth) {
  // Doubling growth: two new corner coordinates at level 0, one at every other level, so the
  // block of a multi-index has 2^(number of zero levels) slots
  for (const LvlType boundary : {0u, 1u, 2u}) {
    CompleteSGGenInstr genInstr(4, 3);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
    genInstr.setBoundaryLevelOffset(boundary);
    const DeltasSparseGrid dsg(genInstr);
    const misc::DSGLinInterpIndex& index = tools::dsg_lin_interpolation::getIndex(dsg);

    BOOST_CHECK_EQUAL(index.nLeaves(), dsg.nTG());
    size_t expectedSlots = 0;
    for (const TensorGridCTData& tgData : dsg) {
      expectedSlots += size_t{1} << std::count(tgData.mi.begin(), tgData.mi.end(), 0u);
    }
    BOOST_CHECK_EQUAL(index.nSlots(), expectedSlots);
    for (size_t dim = 0; dim < 3; dim++) {
      BOOST_CHECK_EQUAL(index.maxLvl(dim), 4u);
      for (LvlType lvl = 0; lvl <= 4; lvl++) {
        BOOST_CHECK_EQUAL(index.maxNewCoords(index.row(dim, lvl)), lvl == 0 ? 2 : 1);
        const std::span<const double> nodes = index.deltaNodes(index.row(dim, lvl));
        BOOST_CHECK(std::is_sorted(nodes.begin(), nodes.end()));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Lower_Bound_From_Any_Hint) {
  const std::vector<double> nodes = {0.0, 0.125, 0.25, 0.25, 0.5, 0.625, 0.75, 0.875, 1.0};
  const std::vector<double> xs = {-1.0, 0.0,  0.1, 0.125,
                                  0.2,  0.25, 0.3, 0.875,
                                  0.9,  1.0,  2.0, std::numeric_limits<double>::quiet_NaN()};
  for (size_t n = 0; n <= nodes.size(); n++) {
    const std::span<const double> sub(nodes.data(), n);
    for (const double x : xs) {
      const auto expected =
          static_cast<size_t>(std::lower_bound(sub.begin(), sub.end(), x) - sub.begin());
      for (size_t hint = 0; hint <= n + 2; hint++) {
        BOOST_CHECK_EQUAL(tools::dsg_lin_interpolation::lowerBoundFrom(sub, x, hint), expected);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Bracket_Rules) {
  using tools::dsg_lin_interpolation::bracketOf;
  const double inf = std::numeric_limits<double>::infinity();
  BOOST_CHECK(bracketOf(0.75, inf, 0.8) == std::make_pair(0.75, 1.0));
  BOOST_CHECK(bracketOf(-inf, 0.25, 0.1) == std::make_pair(0.0, 0.25));
  BOOST_CHECK(bracketOf(0.25, 0.5, 0.5 - 1e-13) == std::make_pair(0.5, 0.5));
  BOOST_CHECK(bracketOf(0.25, 0.5, 0.3) == std::make_pair(0.25, 0.5));
}

BOOST_AUTO_TEST_SUITE_END()
