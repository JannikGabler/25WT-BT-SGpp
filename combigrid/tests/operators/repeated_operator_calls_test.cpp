// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file repeated_operator_calls_test.cpp
 * @brief Boost.Test cases pinning that repeated operator calls on one grid give bitwise the
 * results of the first call on a fresh grid of the same generation instruction (all operators on
 * @c SparseGrid and @c DeltasSparseGrid, scalar and vector-valued; d = 1-4, boundary offsets 0 / 1,
 * unit and shifted domains), and that operator calls do not change the grid. Bitwise at one
 * thread (the reductions of some operators depend on the work distribution), within round-off
 * with all threads.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <bit>
#include <cmath>
#include <cstddef>
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
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <span>
#include <string>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;

namespace {

/// A grid configuration of the tests.
struct RepeatConfig {
  std::string nodes;  ///< "equi", "cc" or "cheb1" (cheb1: not nested, no deltas sparse grid).
  size_t nDim;
  LvlType maxLvl;
  LvlType boundaryOffset;
  bool shiftedDomain;
};

const std::vector<RepeatConfig>& repeatConfigs() {
  static const std::vector<RepeatConfig> configs{
      {"cc", 1, 4, 0, false},    {"equi", 1, 3, 1, true}, {"cc", 2, 5, 0, true},
      {"equi", 3, 4, 1, false},  {"cc", 4, 3, 1, true},   {"equi", 2, 4, 0, true},
      {"cheb1", 2, 4, 0, false}, {"cheb1", 3, 3, 1, true}};
  return configs;
}

NodeGenFunc* nodeGenFunc(const std::string& nodes) {
  if (nodes == "equi") return sgpp::combigrid::getEquidistantNodeGenFunc();
  if (nodes == "cc") return sgpp::combigrid::getClenshawCurtisNodeGenFunc();
  return sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc();
}

CompleteSGGenInstr makeGenInstr(const RepeatConfig& config) {
  CompleteSGGenInstr genInstr(config.maxLvl, config.nDim);
  if (config.shiftedDomain) {
    genInstr.setDomain({-0.5, 1.75});
  } else {
    genInstr.setDomain({0.0, 1.0});
  }
  genInstr.setNodeGenFunc(nodeGenFunc(config.nodes));
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(config.boundaryOffset);
  return genInstr;
}

double scalarFunc(const DataVector& x) {
  double value = 1.0;
  for (size_t k = 0; k < x.size(); k++) value *= std::exp(0.3 * x[k]) + 0.2 * x[k] * x[k];
  return value;
}

void vectorFunc(const DataVector& x, std::span<double> out) {
  out[0] = scalarFunc(x);
  out[1] = std::sin(x[0]) + 2.0;
  out[2] = x.sum();
}

DataVector queryPoint(const RepeatConfig& config) {
  DataVector point(config.nDim);
  for (size_t k = 0; k < config.nDim; k++) {
    const double unit = 0.23 + 0.17 * static_cast<double>(k);
    point[k] = config.shiftedDomain ? -0.5 + 2.25 * unit : unit;
  }
  return point;
}

/// The bit patterns of all results of one grid: per operator scalar, then the m vector outputs.
template <typename Grid>
std::vector<uint64_t> allResults(const Grid& grid, const DataVector& point) {
  const SourceFunc func(scalarFunc);
  const VectorSourceFunc vecFunc(vectorFunc, 3);
  std::vector<uint64_t> bits;
  const auto add = [&](const double value) { bits.push_back(std::bit_cast<uint64_t>(value)); };
  const auto addAll = [&](const DataVector& values) {
    for (size_t k = 0; k < values.size(); k++) add(values[k]);
  };

  add(sgpp::combigrid::quadrature(func, grid));
  addAll(sgpp::combigrid::quadrature(vecFunc, grid));
  add(sgpp::combigrid::interpolate(func, point, grid));
  addAll(sgpp::combigrid::interpolate(vecFunc, point, grid));
  add(sgpp::combigrid::interpolateLinear(func, point, grid));
  addAll(sgpp::combigrid::interpolateLinear(vecFunc, point, grid));
  return bits;
}

/// Sets the number of OpenMP threads for its lifetime.
class ThreadCount {
 public:
  explicit ThreadCount(const int nThreads) : previous(omp_get_max_threads()) {
    omp_set_num_threads(nThreads);
  }
  ~ThreadCount() { omp_set_num_threads(previous); }
  ThreadCount(const ThreadCount&) = delete;
  ThreadCount& operator=(const ThreadCount&) = delete;

 private:
  int previous;
};

void checkClose(const std::vector<uint64_t>& actual, const std::vector<uint64_t>& expected) {
  BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
  for (size_t i = 0; i < actual.size(); i++) {
    const double a = std::bit_cast<double>(actual[i]);
    const double e = std::bit_cast<double>(expected[i]);
    BOOST_CHECK_SMALL(a - e, 1e-12 * (1.0 + std::abs(e)));
  }
}

/// Calls every operator three times on one grid and compares with a fresh grid's first call:
/// bitwise at one thread, within round-off with all threads.
template <typename Grid>
void checkRepeatedCalls(const RepeatConfig& config) {
  const CompleteSGGenInstr genInstr = makeGenInstr(config);
  const DataVector point = queryPoint(config);
  const Grid grid(genInstr);
  const size_t nTG = grid.nTG();
  std::vector<uint64_t> first;
  {
    const ThreadCount oneThread(1);
    first = allResults(grid, point);
    for (int repeat = 0; repeat < 2; repeat++) {
      const std::vector<uint64_t> again = allResults(grid, point);
      BOOST_CHECK_EQUAL_COLLECTIONS(again.begin(), again.end(), first.begin(), first.end());
    }
    const Grid fresh(genInstr);
    const std::vector<uint64_t> freshResults = allResults(fresh, point);
    BOOST_CHECK_EQUAL_COLLECTIONS(freshResults.begin(), freshResults.end(), first.begin(),
                                  first.end());
    BOOST_CHECK(grid == fresh);
  }
  checkClose(allResults(grid, point), first);
  BOOST_CHECK_EQUAL(grid.nTG(), nTG);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(repeated_operator_calls)

BOOST_AUTO_TEST_CASE(SparseGrid_Repeated_Calls_Equal_First_Call_Bitwise) {
  for (const RepeatConfig& config : repeatConfigs()) {
    BOOST_TEST_CONTEXT(config.nodes << " d" << config.nDim << " L" << config.maxLvl << " b"
                                    << config.boundaryOffset) {
      checkRepeatedCalls<SparseGrid>(config);
    }
  }
}

BOOST_AUTO_TEST_CASE(DeltasSparseGrid_Repeated_Calls_Equal_First_Call_Bitwise) {
  for (const RepeatConfig& config : repeatConfigs()) {
    if (config.nodes == "cheb1") continue;  // Not nested
    BOOST_TEST_CONTEXT(config.nodes << " d" << config.nDim << " L" << config.maxLvl << " b"
                                    << config.boundaryOffset) {
      checkRepeatedCalls<DeltasSparseGrid>(config);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
