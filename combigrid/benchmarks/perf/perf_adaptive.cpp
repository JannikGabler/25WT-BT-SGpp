// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_adaptive.cpp
 * @brief Feature benchmarks of the dimension-adaptive refinement (not part of the judged suite).
 *
 * Every case builds a level-0 grid and refines it with @c DimensionAdaptiveRefinement for the
 * quadrature of an anisotropic Gaussian (Genz family, coefficients a_k = 4 k^-2 of the
 * dimension-adaptive literature) until a grid-point budget is used up. Phases: @c generation
 * (the level-0 grid) and @c refine (the call). Counters: evaluated grid points, tensor grids,
 * decisions, the share of worker time spent waiting (idle), tensor grids evaluated ahead (used /
 * unused), the relative error of the estimate.
 *
 * Case names: adapt/quad/cc/b1/d<dim>/gauss/<cost>/budget<points>/<schedule>, with the cost of an
 * evaluation @c c0 (the cheap function alone), @c c1us or @c c100us (@c perf::expensive, fixed
 * FMA-chain work per evaluation) and the schedules @c sync (synchronized batches), @c sync-w32
 * (synchronized, 32 tensor grids evaluated ahead) and @c async (asynchronous).
 *
 * Usage: perf_adaptive [--filter RE] [--threads N] [--samples N] [--warmups N] [--json]
 * (see perf_harness.hpp).
 */

#include <omp.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/refinement/dimension_adaptive_refinement.hpp>
#include <sgpp/combigrid/refinement/refinement_options.hpp>
#include <sgpp/combigrid/refinement/targets/quadrature_refinement_target.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

#include "perf_harness.hpp"
#include "perf_problems.hpp"

namespace cg = sgpp::combigrid;
using sgpp::base::DataVector;
using sgpp::combigrid::perf::Suite;

namespace {

/// Coefficient a_k = 4 / k^2 (k = 1, 2, ...) of the anisotropic Gaussian.
double coefficient(const size_t k) { return 4.0 / static_cast<double>((k + 1) * (k + 1)); }

/// Genz Gaussian exp(-sum_k a_k^2 (x_k - 1/2)^2) on the unit cube.
double gauss(const DataVector& x) {
  double sum = 0.0;
  for (size_t k = 0; k < x.size(); k++) {
    const double t = coefficient(k) * (x[k] - 0.5);
    sum += t * t;
  }
  return std::exp(-sum);
}

/// Its exact integral: prod_k sqrt(pi) / a_k erf(a_k / 2).
double gaussIntegral(const size_t nDim) {
  long double result = 1.0L;
  for (size_t k = 0; k < nDim; k++) {
    const long double a = coefficient(k);
    result *= std::sqrt(static_cast<long double>(M_PI)) / a * std::erf(a / 2.0L);
  }
  return static_cast<double>(result);
}

/// A schedule variant of the refinement.
cg::RefinementOptions optionsOf(const std::string& schedule, const size_t budget) {
  cg::RefinementOptions options;
  options.maxGridPoints = budget;
  if (schedule == "sync-w32") {
    options.speculationWindow = 32;
  } else if (schedule == "async") {
    options.schedule = cg::RefinementSchedule::Asynchronous;
  }
  return options;
}

/// Registers one case: refine a level-0 CC grid (offset 1) for the quadrature of @p func.
void addCase(Suite& suite, const size_t nDim, const std::string& cost,
             double (*func)(const DataVector&), const size_t budget, const std::string& schedule) {
  const std::string name = "adapt/quad/cc/b1/d" + std::to_string(nDim) + "/gauss/" + cost +
                           "/budget" + std::to_string(budget) + "/" + schedule;
  suite.addPipeline(name, [=](cg::perf::PipelineRun& run) {
    std::optional<cg::SparseGrid> sg;
    run.phase("generation", [&]() {
      cg::CompleteSGGenInstr instr(0, nDim);
      instr.setNodeFamily(cg::getClenshawCurtisNodes());
      instr.setLvl2GPCntFunc(cg::doublingLvl2GPCntFunction);
      instr.setBoundaryLevelOffset(1);
      sg.emplace(instr);
    });

    const cg::SourceFunc f(func);
    cg::QuadratureRefinementTarget target(f);
    cg::DimensionAdaptiveRefinement refinement(*sg, target, optionsOf(schedule, budget));
    cg::RefinementStatistics statistics;
    run.phase("refine", [&]() { statistics = refinement.refine(); });

    const double exact = gaussIntegral(nDim);
    const double nThreads = static_cast<double>(omp_get_max_threads());
    run.setResult(refinement.getEstimate()[0], exact);
    run.setCounter("evaluations", static_cast<double>(statistics.nEvaluatedGridPoints));
    run.setCounter("tensorGrids", static_cast<double>(statistics.nTensorGrids));
    run.setCounter("decisions", static_cast<double>(statistics.nDecisions));
    run.setCounter("idleShare", statistics.workerIdleSeconds / (nThreads * statistics.seconds));
    run.setCounter("ahead", static_cast<double>(statistics.nSpeculativeTensorGrids));
    run.setCounter("aheadUnused", static_cast<double>(statistics.nUnusedSpeculativeTensorGrids));
    run.setCounter("relError", std::abs(refinement.getEstimate()[0] - exact) / exact);
  });
}

template <uint64_t ITERATIONS>
double expensiveGauss(const DataVector& x) {
  return cg::perf::expensive<gauss, ITERATIONS, cg::perf::CostShape::uniform>(x);
}

}  // namespace

int main(int argc, char** argv) {
  Suite suite("adaptive", 3, 1);
  for (const std::string schedule : {"sync", "sync-w32", "async"}) {
    // Evaluation-bound, many candidates per decision
    addCase(suite, 10, "c1us", expensiveGauss<cg::perf::COST_1US_ITERATIONS>, 300000, schedule);
    addCase(suite, 50, "c1us", expensiveGauss<cg::perf::COST_1US_ITERATIONS>, 300000, schedule);
    // Bookkeeping-bound: high dimension, cheap function
    addCase(suite, 100, "c0", gauss, 1000000, schedule);
    // Low dimension: few, large tensor grids per decision
    addCase(suite, 3, "c100us", expensiveGauss<cg::perf::COST_100US_ITERATIONS>, 20000, schedule);
    addCase(suite, 5, "c100us", expensiveGauss<cg::perf::COST_100US_ITERATIONS>, 20000, schedule);
  }
  return suite.main(argc, argv);
}
