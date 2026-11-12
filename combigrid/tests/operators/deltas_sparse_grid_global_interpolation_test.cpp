// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_global_interpolation_test.cpp
 * @brief Boost.Test cases for the global interpolation operator on a deltas sparse grid
 * (@c interpolate on a @c DeltasSparseGrid) and its index (@c misc::DSGGlobalInterpIndex).
 *
 * The operator computes the same combination-technique interpolant as @c interpolate on the
 * @c SparseGrid of the same generation instruction, but evaluates every grid point once and
 * weights it with its global weight. The tests compare both operators (up to round-off, scaled
 * with the Lebesgue constants of the 1D rules) over node families, boundary level offsets,
 * domains, grids, dimensions and query points, count the evaluations (each point the SparseGrid
 * operator evaluates exactly once, no other point) and compare the vector-valued overloads with
 * the scalar one.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <numeric>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/deltas_sparse_grid_global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <span>
#include <stdexcept>
#include <string>
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

/// Larger grids: parallel path (d5 L5, d11, d12), deep levels (cc; equidistant interpolation of
/// high degree is dominated by round-off).
std::vector<Config> largeConfigs() {
  return {{"equi", 1, false, 5, false, 5},  {"cc", 0, true, 5, false, 4},
          {"equi", 1, false, 11, false, 2}, {"cc", 2, false, 12, false, 2},
          {"cc", 0, false, 2, false, 9},    {"cc", 1, true, 1, false, 7},
          {"equi", 0, false, 2, false, 5}};
}

std::string describe(const Config& c, const size_t pointIdx) {
  return c.nodes + " b" + std::to_string(c.boundary) + (c.unitDomain ? " unit" : " shifted") +
         " d" + std::to_string(c.nDim) + (c.mivec ? " mivec" : " L" + std::to_string(c.maxLvl)) +
         " point " + std::to_string(pointIdx);
}

/**
 * Scale of the round-off of both operators at @p point: sum over the tensor grids of |c_l| times
 * the product of the 1D Lebesgue sums sum_i |L_i(x_j)| (bounds the sum of |W_p| over the grid
 * points), times max |f| (bounded by 2 for testFunc on the test domains).
 */
double roundOffScale(const SparseGrid& sg, const DataVector& point) {
  const SGGenInstr& genInstr = sg.getGenInstr();
  const DataVector normPoint = tools::normalizeDataVector(point, genInstr.getDomain());
  double scale = 0.0;
  for (const TensorGridCTData& tgData : sg) {
    double product = 1.0;
    for (size_t j = 0; j < sg.nDim(); j++) {
      const GPCntType nNodes = tgData.tensorGrid.getGPCntPerDim()[j];
      const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(j);
      const DataVector nodesVec =
          nodeGenFunc->genNodes(nNodes, tgData.mi[j] >= genInstr.getBoundaryLevelOffset());
      const std::vector<double> nodes(nodesVec.begin(), nodesVec.end());
      std::vector<double> weights(nNodes);
      nodeGenFunc->getInterpolationMethod(nNodes)->basisValues(normPoint[j], nodes, weights);
      double lebesgue = 0.0;
      for (const double w : weights) lebesgue += std::abs(w);
      product *= lebesgue;
    }
    scale += std::abs(tgData.coefficient) * product;
  }
  return 2.0 * scale;
}

/// Tolerance of a comparison with the SparseGrid operator.
double tolerance(const SparseGrid& sg, const DataVector& point) {
  return 64.0 * 0x1p-52 * (1.0 + roundOffScale(sg, point));
}

void checkEqualsSparseGridOperator(const std::vector<Config>& configs) {
  for (const Config& config : configs) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);
    const DeltasSparseGrid dsg(*genInstr);
    const std::vector<DataVector> points = testPoints(genInstr->getDomain());

    for (size_t p = 0; p < points.size(); p++) {
      const double expected = interpolate(SourceFunc(testFunc), points[p], sg);
      const double result = interpolate(SourceFunc(testFunc), points[p], dsg);
      BOOST_TEST_CONTEXT(describe(config, p)) {
        BOOST_CHECK_SMALL(result - expected, tolerance(sg, points[p]));
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

BOOST_AUTO_TEST_SUITE(dsg_global_interpolation)

BOOST_AUTO_TEST_CASE(Equals_SparseGrid_Operator_Low_Dimensions) {
  checkEqualsSparseGridOperator(lowDimConfigs());
}

BOOST_AUTO_TEST_CASE(Equals_SparseGrid_Operator_Large_Grids) {
  checkEqualsSparseGridOperator(largeConfigs());
}

BOOST_AUTO_TEST_CASE(Each_Grid_Point_Evaluated_Once) {
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

    for (const size_t p : {size_t{0}, size_t{2}}) {
      CountingFunc sgCounts, dsgCounts;
      interpolate(sgCounts.sourceFunc(), points[p], sg);
      interpolate(dsgCounts.sourceFunc(), points[p], dsg);

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

    for (const DataVector& point : testPoints(genInstr->getDomain())) {
      const double tol = tolerance(sg, point) * 1.25;  // max |f| of the outputs <= 2.5
      const DataVector result = interpolate(vecFunc, point, dsg);
      const DataVector sgResult = interpolate(vecFunc, point, sg);
      for (size_t k = 0; k < 3; k++) {
        const SourceFunc component([&vecFunc, k](const DataVector& x) {
          std::vector<double> out(3);
          vecFunc.evaluate(x, out);
          return out[k];
        });
        const double scalar = interpolate(component, point, dsg);
        BOOST_TEST_CONTEXT(describe(config, 0) << " output " << k) {
          BOOST_CHECK_SMALL(result[k] - scalar, tol);
          BOOST_CHECK_SMALL(result[k] - sgResult[k], tol);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Serial_Path_Equals_Parallel_Path) {
  // Bitwise: the sums of the delta tensor grids are added in the grid's order in both paths
  for (const Config& config :
       {Config{"cc", 0, false, 3, false, 4}, Config{"equi", 1, true, 2, true}}) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const DeltasSparseGrid dsg(*genInstr);
    const misc::DSGGlobalInterpIndex& index = tools::dsg_global_interpolation::getIndex(dsg);

    for (const DataVector& point : testPoints(genInstr->getDomain())) {
      const DataVector normPoint = tools::normalizeDataVector(point, genInstr->getDomain());
      const SourceFunc f(testFunc);
      const double serial =
          tools::dsg_global_interpolation::evaluateInterpolant(index, dsg, normPoint, f, false);
      const double parallel =
          tools::dsg_global_interpolation::evaluateInterpolant(index, dsg, normPoint, f, true);
      BOOST_CHECK_EQUAL(serial, parallel);

      const VectorSourceFunc vecFunc(testFuncVec, 3);
      std::vector<double> serialVec(3), parallelVec(3);
      tools::dsg_global_interpolation::evaluateInterpolant(index, dsg, normPoint, vecFunc, false,
                                                           serialVec);
      tools::dsg_global_interpolation::evaluateInterpolant(index, dsg, normPoint, vecFunc, true,
                                                           parallelVec);
      for (size_t k = 0; k < 3; k++) BOOST_CHECK_EQUAL(serialVec[k], parallelVec[k]);
      BOOST_CHECK_SMALL(serialVec[0] - serial, 1e-13 * (1.0 + std::abs(serial)));
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
  BOOST_CHECK_THROW(interpolate(vecFunc, point, dsg, std::span<double>(tooSmall)),
                    std::invalid_argument);
  BOOST_CHECK_THROW(interpolate(vecFunc, point, dsg, std::span<double>(tooLarge)),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Concurrent_First_Calls_Share_One_Index) {
  Config config{"cc", 1, false, 4, false, 4};
  LvlMIVec miVec(config.nDim, 0);
  const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
  const SparseGrid sg(*genInstr);
  const DeltasSparseGrid dsg(*genInstr);
  const std::vector<DataVector> points = testPoints(genInstr->getDomain());
  std::vector<double> results(4 * points.size());

#pragma omp parallel for num_threads(4) schedule(static, 1)
  for (size_t i = 0; i < results.size(); i++) {
    results[i] = interpolate(SourceFunc(testFunc), points[i % points.size()], dsg);
  }

  for (size_t i = 0; i < results.size(); i++) {
    const DataVector& point = points[i % points.size()];
    const double expected = interpolate(SourceFunc(testFunc), point, sg);
    BOOST_CHECK_SMALL(results[i] - expected, tolerance(sg, point));
  }
  BOOST_CHECK_EQUAL(&tools::dsg_global_interpolation::getIndex(dsg),
                    &tools::dsg_global_interpolation::getIndex(dsg));
}

BOOST_AUTO_TEST_CASE(Index_Rows_Match_Quadrature_Layout) {
  // The delta order of every row is a permutation of the regular nodes; the layout of the basis
  // values is the one of the quadrature weights; the basis values of a row sum to 1
  for (const LvlType boundary : {0u, 1u, 2u}) {
    for (const std::string nodes : {"equi", "cc"}) {
      Config config{nodes, boundary, false, 3, true};
      LvlMIVec miVec(config.nDim, 0);
      const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
      const DeltasSparseGrid dsg(*genInstr);
      const misc::DSGGlobalInterpIndex& index = tools::dsg_global_interpolation::getIndex(dsg);
      const misc::FlatNestedVec<double, 3> weights = tools::dsg_quadrature::comp1DWeights(dsg);
      const misc::FlatNestedVec<double, 3> basisValues =
          tools::dsg_global_interpolation::compBasisValues(index, DataVector(3, 0.37));

      BOOST_TEST_CONTEXT(nodes << " b" << boundary) {
        size_t nGridPoints = 0;
        for (const TensorGridCTData& tgData : dsg) nGridPoints += tgData.tensorGrid.nGP();
        BOOST_CHECK_EQUAL(index.nGridPoints(), nGridPoints);
        BOOST_CHECK_EQUAL(basisValues.size(), weights.size());
        for (size_t dim = 0; dim < 3; dim++) {
          BOOST_REQUIRE_EQUAL(weights[dim].size(), size_t{index.maxLvl(dim)} + 1);
          for (LvlType lvl = 0; lvl <= index.maxLvl(dim); lvl++) {
            const size_t r = index.row(dim, lvl);
            BOOST_CHECK_EQUAL(weights[dim][lvl].size(), index.nodes(r).size());
            std::vector<GPCntType> order(index.deltaOrder(r).begin(), index.deltaOrder(r).end());
            std::ranges::sort(order);
            std::vector<GPCntType> identity(order.size());
            std::iota(identity.begin(), identity.end(), GPCntType{0});
            BOOST_CHECK(order == identity);
            const std::span<const double> row = basisValues[dim][lvl];
            BOOST_CHECK_CLOSE(std::accumulate(row.begin(), row.end(), 0.0), 1.0, 1e-10);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Single_Point_Grid) {
  // Level 0, boundary level offset 1: one grid point; the interpolant is constant
  CompleteSGGenInstr genInstr(0, 2);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  const DeltasSparseGrid dsg(genInstr);
  const DataVector point(2, 0.9);
  BOOST_CHECK_CLOSE(interpolate(SourceFunc(testFunc), point, dsg), testFunc(DataVector(2, 0.5)),
                    1e-12);
}

BOOST_AUTO_TEST_SUITE_END()
