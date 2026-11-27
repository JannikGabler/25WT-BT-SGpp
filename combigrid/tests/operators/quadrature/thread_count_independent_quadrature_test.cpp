// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file thread_count_independent_quadrature_test.cpp
 * @brief Boost.Test cases checking that @c quadrature on a @c SparseGrid gives bitwise identical
 * results at every number of OpenMP threads and in repeated calls, and that it combines the
 * per-tensor-grid estimates in the order of the tensor grids.
 *
 * Configurations: scalar and vector-valued (m = 3) source functions, boundary level offsets 0 / 1,
 * Clenshaw-Curtis, equidistant and first-kind Chebyshev nodes, complete and anisotropic
 * multi-index grids (one with tensor grids of strongly different sizes, whose largest exceed a
 * thread's balanced share), unit and shifted domains, with and without the cross-call operator
 * data cache, and grids with fewer tensor grids than threads.
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
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

namespace {

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::CTCoeffType;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::QuadratureOptions;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;
using sgpp::combigrid::tools::numerics::compensatedDotProduct;
using sgpp::combigrid::tools::numerics::compensatedDotProducts;

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
    for (const LvlType boundary : {LvlType{0}, LvlType{1}}) {
      const std::string prefix = family + " b" + std::to_string(boundary);
      // Many tensor grids of different sizes (d4 L6: 140, d6 L4: 210), shifted domain
      result.push_back({prefix + " complete d4 L6", nodes, boundary, false, 4, 6, false});
      result.push_back({prefix + " complete d6 L4 shifted", nodes, boundary, false, 6, 4, true});
      result.push_back({prefix + " mivec d5", nodes, boundary, true, 5, 0, false});
      result.push_back({prefix + " mivec skewed d3", nodes, boundary, true, 3, 0, false, true});
      // Fewer tensor grids than threads: d1 (1 tensor grid), d2 L2 (5 tensor grids)
      result.push_back({prefix + " complete d1 L5", nodes, boundary, false, 1, 5, true});
      result.push_back({prefix + " complete d2 L2", nodes, boundary, false, 2, 2, false});
    }
  }
  return result;
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

/// Scalar and vector-valued quadrature of one grid with one option (m + 1 values).
std::vector<double> quadratures(const SparseGrid& grid, const QuadratureOptions& options) {
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);
  std::vector<double> values{sgpp::combigrid::quadrature(f, grid, options)};
  const DataVector vec = sgpp::combigrid::quadrature(fVec, grid, options);
  values.insert(values.end(), vec.begin(), vec.end());
  return values;
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

/// Per tensor grid (in their order): the scalar and the m vector-valued estimates of the
/// per-tensor-grid kernel on the unit hypercube (without coefficient).
std::vector<std::vector<double>> tensorGridEstimates(const SparseGrid& grid) {
  namespace quad_tools = sgpp::combigrid::tools::quadrature;
  const sgpp::combigrid::misc::QuadWeightTable table(grid);
  table.computeAllRows();
  const sgpp::combigrid::misc::QuadratureContext context{.sparseGrid = grid, .weightTable = table};
  sgpp::combigrid::misc::QuadratureScratch scratch(grid.nDim());
  sgpp::combigrid::misc::QuadratureScratch vecScratch(grid.nDim(), N_OUTPUTS);
  const SourceFunc f(testFunc);
  const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);

  std::vector<std::vector<double>> estimates;
  for (size_t tgIdx = 0; tgIdx < grid.nTG(); tgIdx++) {
    const auto tgData = grid.getDomainTensorGrid(tgIdx);
    std::vector<double> values(1 + N_OUTPUTS);
    values[0] = quad_tools::processTensorGrid(context.weightTable, f, tgData.tensorGrid, scratch);
    quad_tools::processTensorGrid(context.weightTable, fVec, tgData.tensorGrid, vecScratch,
                                  std::span<double>(values).subspan(1));
    estimates.push_back(values);
  }
  return estimates;
}

/// Volume times the compensated combination (tools::numerics::compensatedDotProduct(s)) of the
/// estimates in tensor grid order: the scalar value, then the m vector-valued ones.
std::vector<double> combineInOrder(const SparseGrid& grid,
                                   const std::vector<std::vector<double>>& estimates,
                                   const double volume) {
  std::vector<double> scalar, vec;
  for (const std::vector<double>& tgValues : estimates) {
    scalar.push_back(tgValues[0]);
    vec.insert(vec.end(), tgValues.begin() + 1, tgValues.end());
  }
  std::vector<double> result(1 + N_OUTPUTS);
  result[0] = compensatedDotProduct<double>(grid.getCoefficients(), scalar);
  compensatedDotProducts<double>(grid.getCoefficients(), vec, std::span<double>(result).subspan(1));
  for (double& value : result) value *= volume;
  return result;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(thread_count_independent_quadrature)

BOOST_AUTO_TEST_CASE(Results_Are_Bitwise_Equal_At_All_Thread_Counts_And_In_Repeated_Calls) {
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid grid(*genInstr);

    std::vector<double> reference;
    withThreads(1, [&]() { reference = quadratures(grid, QuadratureOptions{}); });

    for (const bool useCache : {false, true}) {
      const QuadratureOptions options{.useCrossCallOperatorDataCache = useCache};
      for (const int nThreads : THREAD_COUNTS) {
        BOOST_TEST_CONTEXT(config.name << " nTG " << grid.nTG() << " cache " << useCache
                                       << " threads " << nThreads) {
          withThreads(nThreads, [&]() {
            const int nRepeats = (nThreads == THREAD_COUNTS.back()) ? N_REPEATS : 1;
            for (int repeat = 0; repeat < nRepeats; repeat++) {
              checkBitwise(quadratures(grid, options), reference);
            }
          });
        }
      }
    }
    grid.clearCrossCallOperatorDataCache();
  }
}

BOOST_AUTO_TEST_CASE(Result_Is_The_Combination_In_Tensor_Grid_Order) {
  // The operator combines the per-tensor-grid estimates in the order of the tensor grids with the
  // compensated dot product and multiplies the sum by the domain volume: recomputed here serially
  // from the per-tensor-grid kernel (bitwise, also at 16 threads)
  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid grid(*genInstr);
    const std::vector<double> expected =
        combineInOrder(grid, tensorGridEstimates(grid), genInstr->getVolumeOfDomain());

    for (const int nThreads : {1, 16}) {
      BOOST_TEST_CONTEXT(config.name << " threads " << nThreads) {
        withThreads(nThreads, [&]() { checkBitwise(quadratures(grid, {}), expected); });
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
