// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file thread_count_independent_operators_test.cpp
 * @brief Boost.Test cases checking that the quadrature on a @c DeltasSparseGrid and the global and
 * linear interpolation on a @c SparseGrid (and on a @c DeltasSparseGrid) give bitwise identical
 * results at every number of OpenMP threads and in repeated calls, and that they combine the
 * per-tensor-grid values in the order of the tensor grids with compensated summation.
 *
 * Configurations: scalar and vector-valued (m = 3) source functions, boundary level offsets 0 / 1,
 * Clenshaw-Curtis, equidistant and first-kind Chebyshev nodes (deltas sparse grids only for the
 * nested families), complete and anisotropic multi-index grids (one with tensor grids of strongly
 * different sizes), unit and shifted domains, with and
 * without the cross-call operator data cache (deltas sparse grid quadrature), and grids with fewer
 * tensor grids than threads. The quadrature on a @c SparseGrid is checked in
 * thread_count_independent_quadrature_test.cpp.
 */

#define BOOST_TEST_DYN_LINK

#include <omp.h>

#include <boost/test/unit_test.hpp>

#include <bit>
#include <cmath>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_quadrature_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature_options.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/deltas_sparse_grid_weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace {

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::DeltasSparseGridQuadratureOptions;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::SparseGridSetup;
using sgpp::combigrid::VectorSourceFunc;
using sgpp::combigrid::tools::numerics::CompensatedSum;

constexpr size_t N_OUTPUTS = 3;
const std::vector<int> THREAD_COUNTS{1, 2, 3, 16};
constexpr int N_REPEATS = 5;

/// Source function with a sign change and values of different magnitudes (round-off in the
/// combination depends on the summation order).
double testFunc(const DataVector& x) {
  double sum = 0.0, prod = 1.0;
  for (size_t i = 0; i < x.size(); i++) {
    sum += (0.7 + 0.3 * static_cast<double>(i)) * x[i];
    prod *= std::cos(1.3 * x[i] + 0.2 * static_cast<double>(i));
  }
  return std::exp(sum) - 3.0 * prod;
}

void testFuncVec(const DataVector& x, const std::span<double> out) {
  const double base = testFunc(x);
  out[0] = base;
  out[1] = std::sin(3.0 * x[0]) * base;
  out[2] = 1e3 * x[x.size() - 1] * x[x.size() - 1] - base;
}

struct Config {
  std::string name;
  NodeGenFunc* nodes;
  bool nested;
  LvlType boundary;
  bool mivec;
  size_t nDim;
  LvlType level;  // Complete grids only
  bool shifted;
  bool skewed = false;  // Multi-index grids only: see anisotropicMIs
};

/// Anisotropic multi-index set (downwards closure taken by MIVecSGGenInstr). @p skewed (d = 3):
/// tensor grids from 1 to about 4000 points (a level-11 axis, a level-4 axis and a mixed
/// multi-index), so that the largest tensor grids exceed a thread's balanced share of the points.
LvlMIVec anisotropicMIs(const size_t nDim, const bool skewed) {
  if (skewed) {
    BOOST_REQUIRE_EQUAL(nDim, 3);
    return LvlMIVec(std::vector<LvlMI>{LvlMI{11, 0, 0}, LvlMI{0, 0, 4}, LvlMI{2, 3, 1}});
  }
  std::vector<LvlMI> mis;
  for (size_t dim = 0; dim < nDim; dim++) {
    LvlMI heavy(nDim, 1);
    heavy[dim] = static_cast<LvlType>(5 - dim % 3);
    mis.push_back(heavy);
  }
  return LvlMIVec(mis);
}

std::unique_ptr<SGGenInstr> makeGenInstr(const Config& config, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.mivec) {
    miVec = anisotropicMIs(config.nDim, config.skewed);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(config.level, config.nDim);
  }
  genInstr->setNodeGenFunc(config.nodes);
  genInstr->setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundary);
  HyperCubeArea domain(config.nDim, {0.0, 1.0});
  if (config.shifted) {
    for (size_t i = 0; i < config.nDim; i++) {
      domain[i] = {-0.4 + 0.1 * static_cast<double>(i), 0.9 + 0.3 * static_cast<double>(i)};
    }
  }
  genInstr->setDomain(domain);
  return genInstr;
}

std::vector<Config> configs() {
  std::vector<Config> result;
  const std::vector<std::pair<std::string, NodeGenFunc*>> families{
      {"cc", sgpp::combigrid::getClenshawCurtisNodeGenFunc()},
      {"equi", sgpp::combigrid::getEquidistantNodeGenFunc()},
      {"cheb1", sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc()}};
  for (const auto& [family, nodes] : families) {
    const bool nested = family != "cheb1";
    for (const LvlType boundary : {LvlType{0}, LvlType{1}}) {
      const std::string prefix = family + " b" + std::to_string(boundary);
      // Many tensor grids of different sizes (d4 L6: 140, d6 L4: 210), shifted domain
      result.push_back({prefix + " complete d4 L6", nodes, nested, boundary, false, 4, 6, false});
      result.push_back(
          {prefix + " complete d6 L4 shifted", nodes, nested, boundary, false, 6, 4, true});
      result.push_back({prefix + " mivec d5", nodes, nested, boundary, true, 5, 0, false});
      result.push_back(
          {prefix + " mivec skewed d3", nodes, nested, boundary, true, 3, 0, false, true});
      // Fewer tensor grids than threads: d1 (1 tensor grid), d2 L2 (5 tensor grids)
      result.push_back({prefix + " complete d1 L5", nodes, nested, boundary, false, 1, 5, true});
      result.push_back({prefix + " complete d2 L2", nodes, nested, boundary, false, 2, 2, false});
    }
  }
  return result;
}

/// Query points in the domain: one in the interior, one with coordinates on the boundary and the
/// midpoint.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  std::vector<DataVector> points(2, DataVector(nDim));
  for (size_t i = 0; i < nDim; i++) {
    const double lo = domain[i].first, width = domain[i].second - domain[i].first;
    points[0][i] = lo + width * std::fmod(0.137 + 0.311 * static_cast<double>(i), 1.0);
    points[1][i] = (i % 3 == 0) ? lo : (i % 3 == 1) ? lo + 0.5 * width : domain[i].second;
  }
  return points;
}

uint64_t bitsOf(const double x) { return std::bit_cast<uint64_t>(x); }

/// Runs @p body with @p nThreads OpenMP threads (restores the previous number afterwards).
template <typename Body>
void withThreads(const int nThreads, const Body& body) {
  const int before = omp_get_max_threads();
  omp_set_num_threads(nThreads);
  body();
  omp_set_num_threads(before);
}

/// Checks that @p actual equals @p expected bitwise.
void checkBitwise(const std::vector<double>& actual, const std::vector<double>& expected) {
  BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
  for (size_t k = 0; k < actual.size(); k++) {
    BOOST_TEST_CONTEXT("value " << k << ": " << actual[k] << " vs. " << expected[k]) {
      BOOST_CHECK_EQUAL(bitsOf(actual[k]), bitsOf(expected[k]));
    }
  }
}

/// Appends the scalar value @p scalar and the m values @p vec to @p values.
void append(std::vector<double>& values, const double scalar, const DataVector& vec) {
  values.push_back(scalar);
  values.insert(values.end(), vec.begin(), vec.end());
}

/// SparseGrid: the global and the linear interpolation (scalar, then vector-valued) at every test
/// point.
std::vector<double> sgInterpolations(const SparseGrid& grid) {
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);
  std::vector<double> values;
  for (const DataVector& point : testPoints(grid.getSetup().getDomain())) {
    append(values, sgpp::combigrid::interpolate(f, point, grid),
           sgpp::combigrid::interpolate(fVec, point, grid));
    append(values, sgpp::combigrid::interpolateLinear(f, point, grid),
           sgpp::combigrid::interpolateLinear(fVec, point, grid));
  }
  return values;
}

/// DeltasSparseGrid: the quadrature with @p options, then the global and the linear interpolation
/// at every test point (scalar, then vector-valued each).
std::vector<double> dsgOperators(const DeltasSparseGrid& grid,
                                 const DeltasSparseGridQuadratureOptions& options) {
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);
  std::vector<double> values;
  append(values, sgpp::combigrid::quadrature(f, grid, options),
         sgpp::combigrid::quadrature(fVec, grid, options));
  for (const DataVector& point : testPoints(grid.getSetup().getDomain())) {
    append(values, sgpp::combigrid::interpolate(f, point, grid),
           sgpp::combigrid::interpolate(fVec, point, grid));
    append(values, sgpp::combigrid::interpolateLinear(f, point, grid),
           sgpp::combigrid::interpolateLinear(fVec, point, grid));
  }
  return values;
}

/// Compensated combination (CompensatedSum, in tensor grid order) of the per-tensor-grid values
/// @p tgValues (scalar value, then the m vector-valued ones per tensor grid) with the factors
/// @p factors (one per tensor grid): the scalar value, then the m vector-valued ones.
std::vector<double> combineInOrder(const std::vector<double>& factors,
                                   const std::vector<std::vector<double>>& tgValues) {
  std::vector<CompensatedSum<double>> sums(1 + N_OUTPUTS);
  for (size_t tgIdx = 0; tgIdx < tgValues.size(); tgIdx++) {
    for (size_t k = 0; k < sums.size(); k++) {
      sums[k].addProduct(factors[tgIdx], tgValues[tgIdx][k]);
    }
  }
  std::vector<double> result;
  for (const CompensatedSum<double>& sum : sums) result.push_back(sum.value());
  return result;
}

/// The combination coefficients of @p grid as doubles.
std::vector<double> coefficientsOf(const SparseGrid& grid) {
  std::vector<double> coefficients;
  for (const auto coefficient : grid.getCoefficients()) {
    coefficients.push_back(static_cast<double>(coefficient));
  }
  return coefficients;
}

/// SparseGrid: the global and the linear interpolation at every test point, recomputed serially
/// from the per-tensor-grid kernels (layout of sgInterpolations).
std::vector<double> sgInterpolationsInOrder(const SparseGrid& grid) {
  namespace global = sgpp::combigrid::tools::global_interpolation;
  namespace linear = sgpp::combigrid::tools::lin_interpolation;
  const SparseGridSetup& setup = grid.getSetup();
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);
  sgpp::combigrid::misc::LinInterpScratch linScratch(grid.nDim());
  sgpp::combigrid::misc::LinInterpScratch linVecScratch(grid.nDim(), N_OUTPUTS);
  const std::vector<double> coefficients = coefficientsOf(grid);

  std::vector<double> values;
  for (const DataVector& point : testPoints(setup.getDomain())) {
    const DataVector normPoint =
        sgpp::combigrid::tools::normalizeDataVector(point, setup.getDomain());
    std::vector<std::vector<double>> globalValues, linearValues;
    for (size_t tgIdx = 0; tgIdx < grid.nTG(); tgIdx++) {
      const auto tg = grid.getDomainTensorGrid(tgIdx).tensorGrid;
      const auto unitTG = grid.getUnitTensorGrid(tgIdx).tensorGrid;
      std::vector<double> global(1 + N_OUTPUTS), linear(1 + N_OUTPUTS);
      global[0] = global::processTensorGrid(f, normPoint, tg, unitTG, setup);
      global::processTensorGrid(fVec, normPoint, tg, unitTG, setup,
                                std::span<double>(global).subspan(1));
      linear[0] = linear::processTensorGrid(f, normPoint, unitTG, setup, linScratch);
      linear::processTensorGrid(fVec, normPoint, unitTG, setup, linVecScratch,
                                std::span<double>(linear).subspan(1));
      globalValues.push_back(global);
      linearValues.push_back(linear);
    }
    for (const auto* tgValues : {&globalValues, &linearValues}) {
      const std::vector<double> combined = combineInOrder(coefficients, *tgValues);
      values.insert(values.end(), combined.begin(), combined.end());
    }
  }
  return values;
}

/// DeltasSparseGrid: the quadrature recomputed serially from the per-delta-tensor-grid kernel
/// (compensated sum in delta tensor grid order times the domain volume; scalar, then m values).
std::vector<double> dsgQuadratureInOrder(const DeltasSparseGrid& grid) {
  namespace dsg_quad = sgpp::combigrid::tools::dsg_quadrature;
  namespace dsg_sum = sgpp::combigrid::tools::dsg_weighted_sum;
  const auto index = dsg_quad::getIndex(grid, false);
  const sgpp::combigrid::misc::GenericDSGOperatorContext context{
      .deltasSparseGrid = grid, .weights1D = index->weights1D(), .coeffTrie = index->coeffTrie()};
  sgpp::combigrid::misc::DeltaWeightedSumScratch scratch(grid.nDim(),
                                                         grid.getMaxTGSumOverGPCntsPerDim());
  sgpp::combigrid::misc::DeltaWeightedSumScratch vecScratch(
      grid.nDim(), grid.getMaxTGSumOverGPCntsPerDim(), N_OUTPUTS);
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);

  std::vector<std::vector<double>> tgValues;
  for (size_t tgIdx = 0; tgIdx < grid.nTG(); tgIdx++) {
    const auto tgData = grid.getDomainTensorGrid(tgIdx);
    std::vector<double> values(1 + N_OUTPUTS);
    values[0] = dsg_sum::processDeltaTensorGrid(context, f, tgData, scratch);
    dsg_sum::processDeltaTensorGrid(context, fVec, tgData, vecScratch,
                                    std::span<double>(values).subspan(1));
    tgValues.push_back(values);
  }
  std::vector<double> result = combineInOrder(std::vector<double>(grid.nTG(), 1.0), tgValues);
  for (double& value : result) value *= grid.getSetup().getVolumeOfDomain();
  return result;
}

/// Checks @p compute() against its 1-thread result at every thread count, repeated at the largest.
template <typename Compute>
void checkAllThreadCounts(const std::string& name, const std::vector<double>& reference,
                          const Compute& compute) {
  for (const int nThreads : THREAD_COUNTS) {
    BOOST_TEST_CONTEXT(name << " threads " << nThreads) {
      withThreads(nThreads, [&]() {
        const int nRepeats = (nThreads == THREAD_COUNTS.back()) ? N_REPEATS : 1;
        for (int repeat = 0; repeat < nRepeats; repeat++) {
          checkBitwise(compute(), reference);
        }
      });
    }
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(thread_count_independent_operators)

BOOST_AUTO_TEST_CASE(SparseGrid_Interpolations_Are_Bitwise_Equal_At_All_Thread_Counts) {
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid grid(*genInstr);

    std::vector<double> reference;
    withThreads(1, [&]() { reference = sgInterpolations(grid); });
    checkAllThreadCounts(config.name + " nTG " + std::to_string(grid.nTG()), reference,
                         [&]() { return sgInterpolations(grid); });
  }
}

BOOST_AUTO_TEST_CASE(DeltasSparseGrid_Operators_Are_Bitwise_Equal_At_All_Thread_Counts) {
  for (const Config& config : configs()) {
    if (!config.nested) continue;
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const DeltasSparseGrid grid(*genInstr);

    std::vector<double> reference;
    withThreads(1, [&]() { reference = dsgOperators(grid, {}); });
    for (const bool useCache : {false, true}) {
      const DeltasSparseGridQuadratureOptions options{.useCrossCallOperatorDataCache = useCache};
      checkAllThreadCounts(
          config.name + " nTG " + std::to_string(grid.nTG()) + " cache " + std::to_string(useCache),
          reference, [&]() { return dsgOperators(grid, options); });
    }
    grid.clearCrossCallOperatorDataCache();
  }
}

BOOST_AUTO_TEST_CASE(SparseGrid_Interpolations_Are_The_Combination_In_Tensor_Grid_Order) {
  // The operators combine the per-tensor-grid values in the order of the tensor grids with the
  // compensated dot product: recomputed here serially from the per-tensor-grid kernels (bitwise,
  // also at 16 threads)
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid grid(*genInstr);
    const std::vector<double> expected = sgInterpolationsInOrder(grid);

    for (const int nThreads : {1, 16}) {
      BOOST_TEST_CONTEXT(config.name << " threads " << nThreads) {
        withThreads(nThreads, [&]() { checkBitwise(sgInterpolations(grid), expected); });
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(DeltasSparseGrid_Quadrature_Is_The_Sum_In_Tensor_Grid_Order) {
  // The quadrature sums the per-delta-tensor-grid values (weighted with the global weights) in the
  // order of the delta tensor grids with compensated summation and multiplies the sum by the
  // domain volume
  for (const Config& config : configs()) {
    if (!config.nested) continue;
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const DeltasSparseGrid grid(*genInstr);
    const std::vector<double> expected = dsgQuadratureInOrder(grid);
    const SourceFunc f(testFunc);
    const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);

    for (const bool useCache : {false, true}) {
      const DeltasSparseGridQuadratureOptions options{.useCrossCallOperatorDataCache = useCache};
      for (const int nThreads : {1, 16}) {
        BOOST_TEST_CONTEXT(config.name << " cache " << useCache << " threads " << nThreads) {
          withThreads(nThreads, [&]() {
            std::vector<double> actual;
            append(actual, sgpp::combigrid::quadrature(f, grid, options),
                   sgpp::combigrid::quadrature(fVec, grid, options));
            checkBitwise(actual, expected);
          });
        }
      }
    }
    grid.clearCrossCallOperatorDataCache();
  }
}

BOOST_AUTO_TEST_SUITE_END()
