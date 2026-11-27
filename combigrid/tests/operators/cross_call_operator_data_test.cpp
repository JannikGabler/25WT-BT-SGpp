// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_test.cpp
 * @brief Boost.Test cases for the operators' use of the cross-call operator data cache: the
 * defaults, the option @c useCrossCallOperatorDataCache (on: built once and reused, shared by
 * scalar and vector-valued calls; off: nothing stored and nothing read), bitwise equal results,
 * clearing and viewing the cache, concurrent calls with clears and views, and @c operator==.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/operators/cross_call/cross_call_operator_data_tools.hpp>
#include <span>
#include <string>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::CrossCallOperatorDataInfo;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::DeltasSparseGridQuadratureOptions;
using sgpp::combigrid::GlobalInterpolationOptions;
using sgpp::combigrid::LinearInterpolationOptions;
using sgpp::combigrid::QuadratureOptions;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;
using sgpp::combigrid::tools::cross_operator_call::nOperatorDataBuilds;

namespace {

CompleteSGGenInstr makeGenInstr(const bool clenshawCurtis, const size_t nDim,
                                const sgpp::combigrid::LvlType maxLvl) {
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setDomain({-0.5, 1.5});
  genInstr.setNodeFamily(clenshawCurtis ? sgpp::combigrid::getClenshawCurtisNodes()
                                        : sgpp::combigrid::getEquidistantNodes());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  return genInstr;
}

double scalarFunc(const DataVector& x) {
  double value = 1.0;
  for (size_t k = 0; k < x.size(); k++) value *= std::cos(0.7 * x[k]) + 0.1 * x[k];
  return value;
}

void vectorFunc(const DataVector& x, std::span<double> out) {
  out[0] = scalarFunc(x);
  out[1] = x.sum();
}

DataVector queryPoint(const size_t nDim) {
  DataVector point(nDim);
  for (size_t k = 0; k < nDim; k++) point[k] = -0.3 + 0.37 * static_cast<double>(k + 1);
  return point;
}

uint64_t bits(const double value) { return std::bit_cast<uint64_t>(value); }

/// The four operators with cached data, scalar and vector-valued, with one option value: bit
/// patterns of all results.
std::vector<uint64_t> cachedOperatorResults(const SparseGrid& sg, const DeltasSparseGrid& dsg,
                                            const bool useCache) {
  const SourceFunc func(scalarFunc);
  const VectorSourceFunc vecFunc(vectorFunc, 2);
  const DataVector point = queryPoint(sg.nDim());
  const QuadratureOptions quad{.useCrossCallOperatorDataCache = useCache};
  const DeltasSparseGridQuadratureOptions dsgQuad{.useCrossCallOperatorDataCache = useCache};
  const GlobalInterpolationOptions interp{.useCrossCallOperatorDataCache = useCache};
  const LinearInterpolationOptions lin{.useCrossCallOperatorDataCache = useCache};
  std::vector<uint64_t> results;
  const auto addAll = [&](const DataVector& values) {
    for (size_t k = 0; k < values.size(); k++) results.push_back(bits(values[k]));
  };

  results.push_back(bits(sgpp::combigrid::quadrature(func, sg, quad)));
  addAll(sgpp::combigrid::quadrature(vecFunc, sg, quad));
  results.push_back(bits(sgpp::combigrid::quadrature(func, dsg, dsgQuad)));
  addAll(sgpp::combigrid::quadrature(vecFunc, dsg, dsgQuad));
  results.push_back(bits(sgpp::combigrid::interpolate(func, point, dsg, interp)));
  addAll(sgpp::combigrid::interpolate(vecFunc, point, dsg, interp));
  results.push_back(bits(sgpp::combigrid::interpolateLinear(func, point, dsg, lin)));
  addAll(sgpp::combigrid::interpolateLinear(vecFunc, point, dsg, lin));
  return results;
}

std::vector<std::string> operatorNames(const sgpp::combigrid::SparseGridBase& grid) {
  std::vector<std::string> names;
  for (const CrossCallOperatorDataInfo& info : grid.getCrossCallOperatorDataCacheEntries()) {
    names.push_back(info.operatorName);
  }
  return names;
}

/// Sets the number of OpenMP threads for its lifetime.
class OneThread {
 public:
  OneThread() : previous(omp_get_max_threads()) { omp_set_num_threads(1); }
  ~OneThread() { omp_set_num_threads(previous); }
  OneThread(const OneThread&) = delete;
  OneThread& operator=(const OneThread&) = delete;

 private:
  int previous;
};

}  // namespace

BOOST_AUTO_TEST_SUITE(cross_call_operator_data)

BOOST_AUTO_TEST_CASE(Defaults_Keep_Only_The_DSG_Operator_Data) {
  // The option types carry the defaults: SparseGrid quadrature off, DSG operators on
  BOOST_CHECK(!QuadratureOptions().useCrossCallOperatorDataCache);
  BOOST_CHECK(DeltasSparseGridQuadratureOptions().useCrossCallOperatorDataCache);
  BOOST_CHECK(GlobalInterpolationOptions().useCrossCallOperatorDataCache);
  BOOST_CHECK(LinearInterpolationOptions().useCrossCallOperatorDataCache);

  const CompleteSGGenInstr genInstr = makeGenInstr(true, 3, 4);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  const SourceFunc func(scalarFunc);
  const VectorSourceFunc vecFunc(vectorFunc, 2);
  const DataVector point = queryPoint(3);

  sgpp::combigrid::quadrature(func, sg);
  sgpp::combigrid::quadrature(vecFunc, sg);
  sgpp::combigrid::interpolate(func, point, sg);
  sgpp::combigrid::interpolateLinear(func, point, sg);
  BOOST_CHECK(sg.getCrossCallOperatorDataCacheEntries().empty());
  BOOST_CHECK_EQUAL(sg.getCrossCallOperatorDataCacheBytes(), 0u);

  // DSG quadrature: one build shared by repeated scalar and vector-valued calls
  const size_t before = nOperatorDataBuilds();
  sgpp::combigrid::quadrature(func, dsg);
  sgpp::combigrid::quadrature(func, dsg);
  sgpp::combigrid::quadrature(vecFunc, dsg);
  std::vector<double> span(2);
  sgpp::combigrid::quadrature(vecFunc, dsg, std::span<double>(span));
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 1u);
  const std::vector<std::string> quadOnly{"quadrature (DeltasSparseGrid)"};
  const std::vector<std::string> afterQuad = operatorNames(dsg);
  BOOST_CHECK_EQUAL_COLLECTIONS(afterQuad.begin(), afterQuad.end(), quadOnly.begin(),
                                quadOnly.end());

  sgpp::combigrid::interpolate(func, point, dsg);
  sgpp::combigrid::interpolateLinear(func, point, dsg);
  const std::vector<std::string> expected{"quadrature (DeltasSparseGrid)",
                                          "interpolate (DeltasSparseGrid)",
                                          "interpolateLinear (DeltasSparseGrid)"};
  const std::vector<std::string> names = operatorNames(dsg);
  BOOST_CHECK_EQUAL_COLLECTIONS(names.begin(), names.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Option_On_Builds_Once_Shared_By_Scalar_And_Vector_Calls) {
  const CompleteSGGenInstr genInstr = makeGenInstr(true, 2, 5);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);

  const size_t before = nOperatorDataBuilds();
  cachedOperatorResults(sg, dsg, true);
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 4u);  // One per operator and grid
  cachedOperatorResults(sg, dsg, true);
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 4u);

  const std::vector<std::string> sgNames = operatorNames(sg);
  BOOST_REQUIRE_EQUAL(sgNames.size(), 1u);
  BOOST_CHECK_EQUAL(sgNames[0], "quadrature (SparseGrid)");
  const std::vector<std::string> dsgNames = operatorNames(dsg);
  const std::vector<std::string> expected{"quadrature (DeltasSparseGrid)",
                                          "interpolate (DeltasSparseGrid)",
                                          "interpolateLinear (DeltasSparseGrid)"};
  BOOST_CHECK_EQUAL_COLLECTIONS(dsgNames.begin(), dsgNames.end(), expected.begin(), expected.end());

  size_t total = 0;
  for (const CrossCallOperatorDataInfo& info : dsg.getCrossCallOperatorDataCacheEntries()) {
    BOOST_CHECK_GT(info.bytes, 0u);
    BOOST_CHECK(!info.description.empty());
    BOOST_CHECK(info.key.empty());
    total += info.bytes;
  }
  BOOST_CHECK_EQUAL(dsg.getCrossCallOperatorDataCacheBytes(), total);
}

BOOST_AUTO_TEST_CASE(Option_Off_Neither_Stores_Nor_Reads) {
  const CompleteSGGenInstr genInstr = makeGenInstr(false, 3, 3);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);

  // Nothing stored
  size_t before = nOperatorDataBuilds();
  cachedOperatorResults(sg, dsg, false);
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 8u);  // Every scalar and vector call
  BOOST_CHECK(sg.getCrossCallOperatorDataCacheEntries().empty());
  BOOST_CHECK(dsg.getCrossCallOperatorDataCacheEntries().empty());

  // Nothing read: with the data cached, every call builds its own
  cachedOperatorResults(sg, dsg, true);
  before = nOperatorDataBuilds();
  cachedOperatorResults(sg, dsg, false);
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 8u);
  BOOST_CHECK_EQUAL(operatorNames(dsg).size(), 3u);
}

BOOST_AUTO_TEST_CASE(Results_Bitwise_Equal_With_And_Without_Reuse) {
  for (const bool clenshawCurtis : {false, true}) {
    for (const size_t nDim : {1, 2, 4}) {
      const CompleteSGGenInstr genInstr = makeGenInstr(clenshawCurtis, nDim, 4);
      const SparseGrid sg(genInstr);
      const DeltasSparseGrid dsg(genInstr);
      const OneThread oneThread;  // The reductions of some operators depend on the work split

      const std::vector<uint64_t> off = cachedOperatorResults(sg, dsg, false);
      const std::vector<uint64_t> first = cachedOperatorResults(sg, dsg, true);
      const std::vector<uint64_t> later = cachedOperatorResults(sg, dsg, true);
      BOOST_CHECK_EQUAL_COLLECTIONS(first.begin(), first.end(), off.begin(), off.end());
      BOOST_CHECK_EQUAL_COLLECTIONS(later.begin(), later.end(), off.begin(), off.end());
    }
  }
}

BOOST_AUTO_TEST_CASE(DSG_Quadrature_Default_Equals_Both_Option_Values) {
  // Repeated DSG quadrature calls with the default options (scalar and vector-valued) give
  // bitwise the results of explicit calls with reuse off and on, on boundary offsets 0 and 1.
  const OneThread oneThread;
  const SourceFunc func(scalarFunc);
  const VectorSourceFunc vecFunc(vectorFunc, 2);
  for (const sgpp::combigrid::LvlType boundary : {0, 1}) {
    for (const size_t nDim : {1, 3}) {
      CompleteSGGenInstr genInstr = makeGenInstr(true, nDim, 4);
      genInstr.setBoundaryLevelOffset(boundary);
      const DeltasSparseGrid dsg(genInstr);
      const DeltasSparseGrid offGrid(genInstr);
      const uint64_t off = bits(
          sgpp::combigrid::quadrature(func, offGrid, {.useCrossCallOperatorDataCache = false}));
      const DataVector offVec =
          sgpp::combigrid::quadrature(vecFunc, offGrid, {.useCrossCallOperatorDataCache = false});
      for (int call = 0; call < 3; call++) {
        BOOST_CHECK_EQUAL(bits(sgpp::combigrid::quadrature(func, dsg)), off);
        BOOST_CHECK_EQUAL(
            bits(sgpp::combigrid::quadrature(func, dsg, {.useCrossCallOperatorDataCache = true})),
            off);
        const DataVector vec = sgpp::combigrid::quadrature(vecFunc, dsg);
        BOOST_REQUIRE_EQUAL(vec.size(), 2u);
        for (size_t k = 0; k < 2; k++) BOOST_CHECK_EQUAL(bits(vec[k]), bits(offVec[k]));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Clear_Frees_And_Next_Call_Rebuilds) {
  const CompleteSGGenInstr genInstr = makeGenInstr(true, 3, 3);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  const OneThread oneThread;

  const std::vector<uint64_t> first = cachedOperatorResults(sg, dsg, true);
  BOOST_CHECK_GT(dsg.getCrossCallOperatorDataCacheBytes(), 0u);
  sg.clearCrossCallOperatorDataCache();
  dsg.clearCrossCallOperatorDataCache();
  BOOST_CHECK(sg.getCrossCallOperatorDataCacheEntries().empty());
  BOOST_CHECK(dsg.getCrossCallOperatorDataCacheEntries().empty());
  BOOST_CHECK_EQUAL(dsg.getCrossCallOperatorDataCacheBytes(), 0u);

  const size_t before = nOperatorDataBuilds();
  const std::vector<uint64_t> again = cachedOperatorResults(sg, dsg, true);
  BOOST_CHECK_EQUAL(nOperatorDataBuilds() - before, 4u);
  BOOST_CHECK_EQUAL_COLLECTIONS(again.begin(), again.end(), first.begin(), first.end());
  BOOST_CHECK_EQUAL(operatorNames(dsg).size(), 3u);
}

BOOST_AUTO_TEST_CASE(Equality_Ignores_The_Cache) {
  const CompleteSGGenInstr genInstr = makeGenInstr(true, 2, 4);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  const DeltasSparseGrid freshDsg(genInstr);
  cachedOperatorResults(sg, dsg, true);
  BOOST_CHECK(dsg == freshDsg);
  BOOST_CHECK(freshDsg.getCrossCallOperatorDataCacheEntries().empty());
}

BOOST_AUTO_TEST_CASE(Concurrent_Calls_Clears_And_Views) {
  // Operator calls (DSG interpolations and quadrature, cached index) while other threads clear and
  // view the cache: every result equals the serial one (the DSG interpolations do not depend on the
  // number of threads, the quadrature up to round-off); the sanitizers check that no index is
  // freed while a call uses it.
  const CompleteSGGenInstr genInstr = makeGenInstr(false, 3, 4);
  const DeltasSparseGrid dsg(genInstr);
  const SourceFunc func(scalarFunc);
  const DataVector point = queryPoint(3);
  const double expectedLin = sgpp::combigrid::interpolateLinear(func, point, dsg);
  const double expectedGlobal = sgpp::combigrid::interpolate(func, point, dsg);
  const double expectedQuad = sgpp::combigrid::quadrature(func, dsg);
  const auto close = [](const double value, const double expected) {
    return std::abs(value - expected) <= 1e-13 * (1.0 + std::abs(expected));
  };
  std::atomic<int> nWrong{0};
  std::atomic<int> nWrongLin{0};
  std::atomic<int> nWrongGlobal{0};
  std::atomic<int> nWrongQuad{0};

#pragma omp parallel num_threads(8)
  for (int i = 0; i < 200; i++) {
    const int task = (omp_get_thread_num() + i) % 8;
    if (task == 0) {
      dsg.clearCrossCallOperatorDataCache();
    } else if (task == 1) {
      size_t total = 0;
      for (const CrossCallOperatorDataInfo& info : dsg.getCrossCallOperatorDataCacheEntries()) {
        total += info.bytes;
      }
      if (total > (size_t{1} << 30)) nWrong++;
    } else if (task >= 6) {
      if (!close(sgpp::combigrid::quadrature(func, dsg), expectedQuad)) nWrongQuad++;
    } else if (task % 2 == 0) {
      if (bits(sgpp::combigrid::interpolateLinear(func, point, dsg)) != bits(expectedLin))
        nWrongLin++;
    } else {
      if (!close(sgpp::combigrid::interpolate(func, point, dsg), expectedGlobal)) nWrongGlobal++;
    }
  }

  BOOST_CHECK_EQUAL(nWrong.load(), 0);
  BOOST_CHECK_EQUAL(nWrongLin.load(), 0);
  BOOST_CHECK_EQUAL(nWrongGlobal.load(), 0);
  BOOST_CHECK_EQUAL(nWrongQuad.load(), 0);
  BOOST_CHECK_LE(operatorNames(dsg).size(), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
