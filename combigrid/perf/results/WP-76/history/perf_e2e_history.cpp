// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_e2e_history.cpp
 * @brief WP-76 (decision D4): the e2e group expensive/ of perf_e2e.cpp (the block copied verbatim
 * from the patched perf_e2e.cpp, guards added) compiled against earlier states of the module
 * (baseline, RP-1 ... RP-5, HEAD) to give the new cases a history. Operators that a state lacks are skipped:
 * DeltasSparseGrid global / multilinear interpolation (WP-45 / WP-47) and CachedSourceFunc (WP-26).
 * Grid iteration: domainTensorGrids() since WP-18, range-for over the grid before.
 */

#include <cstddef>
#include <memory>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

#if __has_include(<sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>)
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#define HISTORY_HAVE_DSG_INTERP
#endif
#if __has_include(<sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>)
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#define HISTORY_HAVE_DSG_INTERPLIN
#endif
#if __has_include(<sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>)
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#define HISTORY_HAVE_CACHE
using sgpp::combigrid::CachedSourceFunc;
#endif

#include "perf_harness.hpp"
#include "perf_problems.hpp"

using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::perf::DataVector;
using sgpp::combigrid::perf::PipelineRun;
using sgpp::combigrid::perf::Suite;

namespace perf = sgpp::combigrid::perf;

namespace {

template <typename Grid>
size_t sumOfGridPoints(const Grid& grid) {
  size_t sum = 0;
  if constexpr (requires { grid.domainTensorGrids(); }) {
    for (const auto& tgData : grid.domainTensorGrids()) sum += tgData.tensorGrid.nGP();
  } else {
    for (const auto& tgData : grid) sum += tgData.tensorGrid.nGP();
  }
  return sum;
}

std::string caseName(const std::string& op, const std::string& grid, const std::string& nodes,
                     const LvlType boundary, const size_t nDim, const std::string& level) {
  return op + "/" + grid + "/" + nodes + "/b" + std::to_string(boundary) + "/d" +
         std::to_string(nDim) + "/" + level;
}

/*******************************************************************************
Group expensive/: expensive source functions
*******************************************************************************/

/// An expensive integrand: the cheap function's value plus burned work (perf::expensive).
struct ExpensiveFunc {
  const char* name;                   // value function: "exp" (perf::expSum)
  const char* cost;                   // "c1us", "c100us", optionally with "-ramp"
  double (*func)(const DataVector&);  // perf::expensive<...>
  bool cached;                        // wrap in a CachedSourceFunc (name suffix "-cached")
};

std::string expensiveName(const std::string& op, const std::string& grid, const std::string& nodes,
                          const ExpensiveFunc& f, const LvlType boundary, const size_t nDim,
                          const std::string& level) {
  return "expensive/" + caseName(op, grid, nodes + "-" + f.name, boundary, nDim, level) + "/" +
         f.cost + (f.cached ? "-cached" : "");
}

std::unique_ptr<SourceFunc> makeSourceFunc(const ExpensiveFunc& f) {
#ifdef HISTORY_HAVE_CACHE
  if (f.cached) return std::make_unique<CachedSourceFunc>(f.func);
#endif
  return std::make_unique<SourceFunc>(f.func);
}

/// Quadrature of perf::expSum (expensive) over [0, 1]^d on a complete (deltas) sparse grid.
template <typename Grid>
void addExpensiveQuadrature(Suite& suite, const std::string& gridName, const std::string& nodes,
                            const ExpensiveFunc& f, const LvlType boundary, const size_t nDim,
                            const LvlType maxLvl) {
  suite.addPipeline(
      expensiveName("quad", gridName, nodes, f, boundary, nDim, "L" + std::to_string(maxLvl)),
      [=](PipelineRun& run) {
        std::unique_ptr<SourceFunc> sourceFunc;
        std::optional<CompleteSGGenInstr> genInstr;
        std::optional<Grid> grid;
        double result = 0.0;

        run.phase("setup", [&]() {
          sourceFunc = makeSourceFunc(f);
          genInstr.emplace(maxLvl, nDim);
          perf::configure(*genInstr, nodes, boundary, perf::unitDomain(nDim));
        });
        run.phase("generation", [&]() { grid.emplace(*genInstr); });
        run.phase("operator", [&]() { result = quadrature(*sourceFunc, *grid); });

        run.setResult(result, perf::expSumIntegral(perf::unitDomain(nDim)));
        run.setCounter("nTG", static_cast<double>(grid->nTG()));
        run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
      });
}

/// MIVecSGGenInstr from an anisotropic, not downwards-closed MI set, then a (deltas) sparse grid
/// and a quadrature of perf::expSum (expensive) over [0, 1]^d.
template <typename Grid>
void addExpensiveMIVec(Suite& suite, const std::string& gridName, const std::string& nodes,
                       const ExpensiveFunc& f, const LvlType boundary, const size_t nDim,
                       const LvlType maxSum, const size_t nMI) {
  suite.addPipeline(expensiveName("mivec", gridName, nodes, f, boundary, nDim,
                                  "S" + std::to_string(maxSum) + "/n" + std::to_string(nMI)),
                    [=](PipelineRun& run) {
                      std::unique_ptr<SourceFunc> sourceFunc;
                      std::optional<LvlMIVec> mis;
                      std::optional<MIVecSGGenInstr> genInstr;
                      std::optional<Grid> grid;
                      double result = 0.0;

                      run.phase("setup", [&]() {
                        sourceFunc = makeSourceFunc(f);
                        mis.emplace(perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim));
                        genInstr.emplace(*mis);
                        perf::configure(*genInstr, nodes, boundary, perf::unitDomain(nDim));
                      });
                      run.phase("generation", [&]() { grid.emplace(*genInstr); });
                      run.phase("operator", [&]() { result = quadrature(*sourceFunc, *grid); });

                      run.setResult(result, perf::expSumIntegral(perf::unitDomain(nDim)));
                      run.setCounter("nTG", static_cast<double>(grid->nTG()));
                      run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
                    });
}

/// Global (or multilinear) interpolation of perf::expSum (expensive) on [0, 1]^d at fixed points.
template <typename Grid>
void addExpensiveInterpolation(Suite& suite, const bool linear, const std::string& gridName,
                               const std::string& nodes, const ExpensiveFunc& f,
                               const LvlType boundary, const size_t nDim, const LvlType maxLvl,
                               const size_t nPoints) {
  suite.addPipeline(
      expensiveName(linear ? "interplin" : "interp", gridName, nodes, f, boundary, nDim,
                    "L" + std::to_string(maxLvl) + "/p" + std::to_string(nPoints)),
      [=](PipelineRun& run) {
        const HyperCubeArea domain = perf::unitDomain(nDim);
        std::unique_ptr<SourceFunc> sourceFunc;
        std::optional<CompleteSGGenInstr> genInstr;
        std::optional<Grid> grid;
        std::vector<DataVector> points;
        double sum = 0.0;

        run.phase("setup", [&]() {
          sourceFunc = makeSourceFunc(f);
          genInstr.emplace(maxLvl, nDim);
          perf::configure(*genInstr, nodes, boundary, domain);
          points = perf::randomPoints(domain, nPoints, 1000 + nDim);
        });
        run.phase("generation", [&]() { grid.emplace(*genInstr); });
        run.phase("operator", [&]() {
          for (const DataVector& point : points) {
            sum += linear ? interpolateLinear(*sourceFunc, point, *grid)
                          : interpolate(*sourceFunc, point, *grid);
          }
        });

        double exact = 0.0;
        for (const DataVector& point : points) exact += perf::expSum(point);
        run.setResult(sum, exact);
        run.setCounter("nTG", static_cast<double>(grid->nTG()));
        run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
      });
}

using perf::CostShape;
constexpr uint64_t C1 = perf::COST_1US_ITERATIONS;
constexpr uint64_t C100 = perf::COST_100US_ITERATIONS;

const ExpensiveFunc EXP_1US{"exp", "c1us", perf::expensive<perf::expSum, C1, CostShape::uniform>,
                            false};
const ExpensiveFunc EXP_100US{"exp", "c100us",
                              perf::expensive<perf::expSum, C100, CostShape::uniform>, false};
const ExpensiveFunc EXP_100US_RAMP{"exp", "c100us-ramp",
                                   perf::expensive<perf::expSum, C100, CostShape::ramp>, false};

ExpensiveFunc cached(ExpensiveFunc f) {
  f.cached = true;
  return f;
}

void addExpensiveCases(Suite& suite) {
  // 100 us per evaluation: evaluation-bound (counts, load balance), SG / DSG pairs
  addExpensiveQuadrature<SparseGrid>(suite, "sg", "cc", EXP_100US, 1, 10, 4);
  addExpensiveQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", EXP_100US, 1, 10, 4);
  addExpensiveQuadrature<SparseGrid>(suite, "sg", "cc", EXP_100US, 1, 20, 3);
  addExpensiveQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", EXP_100US, 1, 20, 3);
  addExpensiveMIVec<SparseGrid>(suite, "sg", "cc", EXP_100US, 1, 15, 6, 300);
  addExpensiveMIVec<DeltasSparseGrid>(suite, "dsg", "cc", EXP_100US, 1, 15, 6, 300);
  addExpensiveInterpolation<SparseGrid>(suite, false, "sg", "cc", EXP_100US, 1, 10, 3, 8);
#ifdef HISTORY_HAVE_DSG_INTERP
  addExpensiveInterpolation<DeltasSparseGrid>(suite, false, "dsg", "cc", EXP_100US, 1, 10, 3, 8);
#endif
  addExpensiveInterpolation<SparseGrid>(suite, true, "sg", "equi", EXP_100US, 1, 5, 5, 16);
#ifdef HISTORY_HAVE_DSG_INTERPLIN
  addExpensiveInterpolation<DeltasSparseGrid>(suite, true, "dsg", "equi", EXP_100US, 1, 5, 5, 16);
#endif
  // 100 us with a cost rising along x_0 (load balance of the scheduling)
  addExpensiveMIVec<SparseGrid>(suite, "sg", "cc", EXP_100US_RAMP, 1, 15, 6, 300);
  addExpensiveInterpolation<SparseGrid>(suite, false, "sg", "cc", EXP_100US_RAMP, 1, 7, 3, 8);
  addExpensiveInterpolation<SparseGrid>(suite, true, "sg", "equi", EXP_100US_RAMP, 1, 5, 5, 16);
  // 1 us per evaluation: evaluations and operator overhead both matter
  addExpensiveQuadrature<SparseGrid>(suite, "sg", "cc", EXP_1US, 1, 15, 5);
  addExpensiveQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", EXP_1US, 1, 15, 5);
  addExpensiveMIVec<SparseGrid>(suite, "sg", "cc", EXP_1US, 1, 20, 10, 300);
  addExpensiveMIVec<DeltasSparseGrid>(suite, "dsg", "cc", EXP_1US, 1, 20, 10, 300);
  addExpensiveInterpolation<SparseGrid>(suite, false, "sg", "cc", EXP_1US, 1, 10, 4, 16);
#ifdef HISTORY_HAVE_DSG_INTERP
  addExpensiveInterpolation<DeltasSparseGrid>(suite, false, "dsg", "cc", EXP_1US, 1, 10, 4, 16);
#endif
  addExpensiveInterpolation<SparseGrid>(suite, true, "sg", "equi", EXP_1US, 1, 10, 4, 16);
#ifdef HISTORY_HAVE_DSG_INTERPLIN
  addExpensiveInterpolation<DeltasSparseGrid>(suite, true, "dsg", "equi", EXP_1US, 1, 10, 4, 16);
#endif
  // Cached source functions (CachedSourceFunc, default configuration; reported separately)
#ifdef HISTORY_HAVE_CACHE
  addExpensiveQuadrature<SparseGrid>(suite, "sg", "cc", cached(EXP_100US), 1, 10, 4);
#endif
#ifdef HISTORY_HAVE_CACHE
  addExpensiveInterpolation<SparseGrid>(suite, false, "sg", "cc", cached(EXP_100US), 1, 10, 3, 8);
#endif
#ifdef HISTORY_HAVE_CACHE
  addExpensiveInterpolation<SparseGrid>(suite, true, "sg", "equi", cached(EXP_100US), 1, 5, 5, 16);
#endif
#ifdef HISTORY_HAVE_CACHE
  addExpensiveInterpolation<SparseGrid>(suite, true, "sg", "equi", cached(EXP_1US), 1, 8, 4, 32);
#endif
}

}  // namespace

int main(int argc, char** argv) {
  Suite suite("e2e", 3, 1);
  addExpensiveCases(suite);
  return suite.main(argc, argv);
}
