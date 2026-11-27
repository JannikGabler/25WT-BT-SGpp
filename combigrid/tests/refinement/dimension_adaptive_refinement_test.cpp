// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dimension_adaptive_refinement_test.cpp
 * @brief Boost.Test cases of @c DimensionAdaptiveRefinement: the refined grid equals the grid
 * that @c MIVecSGGenInstr builds from its index set (levels, coefficients, node tables and operator
 * results bitwise within one build), the estimates equal the operators on the refined grid, the
 * synchronized schedule is independent of the thread count, budgets and limits hold, later calls
 * reuse every value, edge cases (1D, empty grid, boundary offsets, domains, node families),
 * exceptions of the source function, the asynchronous schedule, and the refinement's behaviour on
 * an anisotropic and a smooth function.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>
#include <sgpp/combigrid/refinement/dimension_adaptive_refinement.hpp>
#include <sgpp/combigrid/refinement/refinement_options.hpp>
#include <sgpp/combigrid/refinement/targets/global_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/linear_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/quadrature_refinement_target.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using sgpp::base::DataMatrix;
using sgpp::base::DataVector;
using namespace sgpp::combigrid;  // NOLINT(build/namespaces)

namespace {

/// Grid configuration of a test: dimensionality, node families, growth, offset, domain.
struct Config {
  std::string name;
  size_t nDim;
  std::vector<NodeFamily*> nodes;
  Lvl2GPCntFunc growth;
  LvlType offset;
  HyperCubeArea domain;
};

std::vector<Config> configs() {
  NodeFamily* const cc = getClenshawCurtisNodes();
  NodeFamily* const equi = getEquidistantNodes();
  NodeFamily* const cheb1 = getFirstTypeChebyshevNodes();
  return {
      {"d1 cc b0", 1, {cc}, doublingLvl2GPCntFunction, 0, {{0.0, 1.0}}},
      {"d2 equi b1 domain",
       2,
       {equi, equi},
       doublingLvl2GPCntFunction,
       1,
       {{-1.0, 2.0}, {0.5, 1.5}}},
      {"d3 cc b1",
       3,
       {cc, cc, cc},
       doublingLvl2GPCntFunction,
       1,
       {{0.0, 1.0}, {0.0, 2.0}, {-1.0, 0.0}}},
      {"d3 equi linear b0", 3, {equi, equi, equi}, linearLvl2GPCntFunction, 0, {}},
      {"d2 cheb1 linear b0", 2, {cheb1, cheb1}, linearLvl2GPCntFunction, 0, {}},
      {"d4 mixed b2", 4, {cc, equi, cc, equi}, doublingLvl2GPCntFunction, 2, {}},
  };
}

/// Exponential of a weighted sum: strongly anisotropic, exact integral known.
double anisoExp(const DataVector& x) {
  double sum = 0.0;
  for (size_t k = 0; k < x.size(); k++) sum += x[k] / static_cast<double>((k + 1) * (k + 1));
  return std::exp(sum);
}

/// Two outputs of a vector-valued function: anisoExp and a product of cosines.
void vectorFunc(const DataVector& x, std::span<double> out) {
  out[0] = anisoExp(x);
  double product = 1.0;
  for (size_t k = 0; k < x.size(); k++) product *= std::cos(x[k] / static_cast<double>(k + 1));
  out[1] = product;
}

/// A grid of maximum level @p maxLvl with the configuration's setup.
CompleteSGGenInstr makeInstr(const Config& config, const LvlType maxLvl) {
  CompleteSGGenInstr instr(maxLvl, config.nDim);
  instr.setNodeFamilies(config.nodes);
  instr.setLvl2GPCntFunc(config.growth);
  instr.setBoundaryLevelOffset(config.offset);
  if (!config.domain.empty()) instr.setDomain(config.domain);
  return instr;
}

/// Points inside the domain of @p setup (rows).
DataMatrix pointsIn(const SparseGridSetup& setup, const size_t nPoints) {
  DataMatrix points(nPoints, setup.nDim());
  for (size_t p = 0; p < nPoints; p++) {
    for (size_t k = 0; k < setup.nDim(); k++) {
      const auto [lo, hi] = setup.getDomainForDim(k);
      points.set(p, k, lo + (hi - lo) * std::fmod(0.137 + 0.311 * static_cast<double>(p + k), 1.0));
    }
  }
  return points;
}

uint64_t bitsOf(const double x) { return std::bit_cast<uint64_t>(x); }

/// Row @p row of @p matrix.
DataVector rowOf(const DataMatrix& matrix, const size_t row) {
  DataVector result(matrix.getNcols());
  matrix.getRow(row, result);
  return result;
}

/// Whether @p a and @p b hold the same multi-indices in the same order.
bool sameLevels(const LvlMIVec& a, const LvlMIVec& b) {
  if (a.nDim() != b.nDim() || a.nMI() != b.nMI()) return false;
  for (size_t mi = 0; mi < a.nMI(); mi++) {
    if (!(a[mi] == b[mi])) return false;
  }
  return true;
}

/**
 * Checks that @p refined equals the grid MIVecSGGenInstr builds from its levels with its setup:
 * levels (order), coefficients, node tables and the operators' results bitwise.
 */
void checkEqualsFreshGrid(const SparseGrid& refined) {
  const LvlMIVec levels = refined.getLevels();
  MIVecSGGenInstr instr(levels);
  static_cast<SparseGridSetup&>(instr) = refined.getSetup();
  const SparseGrid fresh(instr);

  BOOST_REQUIRE_EQUAL(refined.nTG(), fresh.nTG());
  for (size_t tg = 0; tg < fresh.nTG(); tg++) {
    BOOST_REQUIRE(refined.getLevels()[tg] == fresh.getLevels()[tg]);
    BOOST_REQUIRE_EQUAL(refined.getCoefficients()[tg], fresh.getCoefficients()[tg]);
  }
  for (size_t dim = 0; dim < fresh.nDim(); dim++) {
    BOOST_REQUIRE_EQUAL(refined.getUnitNodeTable().maxLvl(dim),
                        fresh.getUnitNodeTable().maxLvl(dim));
    for (LvlType lvl = 0; lvl <= fresh.getUnitNodeTable().maxLvl(dim); lvl++) {
      for (const auto& [a, b] :
           {std::pair(&refined.getUnitNodeTable(), &fresh.getUnitNodeTable()),
            std::pair(&refined.getDomainNodeTable(), &fresh.getDomainNodeTable())}) {
        BOOST_REQUIRE_EQUAL(a->nNodes(dim, lvl), b->nNodes(dim, lvl));
        for (size_t i = 0; i < b->nNodes(dim, lvl); i++) {
          BOOST_REQUIRE_EQUAL(bitsOf(a->nodes(dim, lvl)[i]), bitsOf(b->nodes(dim, lvl)[i]));
        }
      }
    }
  }
  BOOST_CHECK_EQUAL(refined.getMaxTGGPCnt(), fresh.getMaxTGGPCnt());
  const SourceFunc f(anisoExp);
  BOOST_CHECK_EQUAL(bitsOf(quadrature(f, refined)), bitsOf(quadrature(f, fresh)));
  const DataVector point = rowOf(pointsIn(refined.getSetup(), 1), 0);
  BOOST_CHECK_EQUAL(bitsOf(interpolate(f, point, refined)), bitsOf(interpolate(f, point, fresh)));
}

/// Runs @p body with @p nThreads OpenMP threads (restores the previous number afterwards).
template <typename Body>
void withThreads(const int nThreads, const Body& body) {
  const int before = omp_get_max_threads();
  omp_set_num_threads(nThreads);
  body();
  omp_set_num_threads(before);
}

RefinementOptions budget(const size_t maxGridPoints) {
  RefinementOptions options;
  options.maxGridPoints = maxGridPoints;
  return options;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(Refinement_DimensionAdaptiveRefinement)

BOOST_AUTO_TEST_CASE(Refined_Grid_Equals_Grid_Built_From_Its_Index_Set) {
  for (const Config& config : configs()) {
    for (const LvlType startLvl : {0u, 1u}) {
      BOOST_TEST_CONTEXT(config.name << ", start level " << startLvl) {
        SparseGrid sg(makeInstr(config, startLvl));
        const SourceFunc f(anisoExp);
        QuadratureRefinementTarget target(f);
        const RefinementStatistics statistics = refine(sg, target, budget(2000));

        BOOST_CHECK(statistics.nDecisions > 0);
        BOOST_CHECK(statistics.nEvaluatedGridPoints <= 2000);
        checkEqualsFreshGrid(sg);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Quadrature_Estimate_Equals_Quadrature_On_The_Refined_Grid) {
  for (const Config& config : configs()) {
    BOOST_TEST_CONTEXT(config.name) {
      SparseGrid sg(makeInstr(config, 0));
      const SourceFunc f(anisoExp);
      QuadratureRefinementTarget target(f);
      DimensionAdaptiveRefinement refinement(sg, target, budget(1500));
      refinement.refine();

      const double q = quadrature(f, sg);
      BOOST_CHECK_EQUAL(bitsOf(refinement.getEstimate()[0]), bitsOf(q));
      BOOST_CHECK_CLOSE_FRACTION(textbookQuadrature(f, sg), q, 1e-12);

      const VectorSourceFunc vf(vectorFunc, 2);
      SparseGrid vsg(makeInstr(config, 0));
      QuadratureRefinementTarget vtarget(vf);
      DimensionAdaptiveRefinement vrefinement(vsg, vtarget, budget(1500));
      vrefinement.refine();
      const DataVector vq = quadrature(vf, vsg);
      for (size_t i = 0; i < 2; i++) {
        BOOST_CHECK_EQUAL(bitsOf(vrefinement.getEstimate()[i]), bitsOf(vq[i]));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Interpolation_Estimates_Equal_The_Operators_On_The_Refined_Grid) {
  for (const Config& config : configs()) {
    BOOST_TEST_CONTEXT(config.name) {
      const SourceFunc f(anisoExp);
      const VectorSourceFunc vf(vectorFunc, 2);
      SparseGrid sg(makeInstr(config, 0));
      const DataMatrix points = pointsIn(sg.getSetup(), 3);

      GlobalInterpolationRefinementTarget global(f, points);
      DimensionAdaptiveRefinement globalRefinement(sg, global, budget(800));
      globalRefinement.refine();
      for (size_t p = 0; p < 3; p++) {
        BOOST_CHECK_EQUAL(bitsOf(globalRefinement.getEstimate()[p]),
                          bitsOf(interpolate(f, rowOf(points, p), sg)));
      }

      LinearInterpolationRefinementTarget linear(vf, points);
      DimensionAdaptiveRefinement linearRefinement(sg, linear, budget(800));  // continues sg
      linearRefinement.refine();
      for (size_t p = 0; p < 3; p++) {
        const DataVector expected = interpolateLinear(vf, rowOf(points, p), sg);
        for (size_t i = 0; i < 2; i++) {
          BOOST_CHECK_EQUAL(bitsOf(linearRefinement.getEstimate()[2 * p + i]), bitsOf(expected[i]));
        }
      }
      checkEqualsFreshGrid(sg);
    }
  }
}

BOOST_AUTO_TEST_CASE(Synchronized_Schedule_Is_Independent_Of_The_Thread_Count) {
  const Config config = configs()[2];
  const SourceFunc f(anisoExp);
  std::vector<double> referenceBits;
  LvlMIVec referenceLevels(config.nDim, 0);

  for (const int nThreads : {1, 2, 3, 16}) {
    BOOST_TEST_CONTEXT("threads " << nThreads) {
      withThreads(nThreads, [&]() {
        SparseGrid sg(makeInstr(config, 0));
        QuadratureRefinementTarget target(f);
        RefinementOptions options = budget(5000);
        options.maxBatchSize = 4;
        DimensionAdaptiveRefinement refinement(sg, target, options);
        refinement.refine();
        const std::vector<double> bits{refinement.getEstimate()[0], refinement.getErrorEstimate()};
        if (nThreads == 1) {
          referenceBits = bits;
          referenceLevels = sg.getLevels();
          return;
        }
        BOOST_CHECK(sameLevels(sg.getLevels(), referenceLevels));
        for (size_t i = 0; i < bits.size(); i++) {
          BOOST_CHECK_EQUAL(bitsOf(bits[i]), bitsOf(referenceBits[i]));
        }
      });
    }
  }
}

BOOST_AUTO_TEST_CASE(Budget_Is_Never_Exceeded_And_Counts_The_Evaluations) {
  for (const size_t maxGridPoints : {size_t{1}, size_t{30}, size_t{333}, size_t{4000}}) {
    BOOST_TEST_CONTEXT("budget " << maxGridPoints) {
      std::atomic<size_t> nEvaluations{0};
      const SourceFunc f([&](const DataVector& x) {
        nEvaluations++;
        return anisoExp(x);
      });
      SparseGrid sg(makeInstr(configs()[2], 0));
      QuadratureRefinementTarget target(f);
      const RefinementStatistics statistics = refine(sg, target, budget(maxGridPoints));

      BOOST_CHECK(statistics.nEvaluatedGridPoints <= maxGridPoints);
      BOOST_CHECK_EQUAL(statistics.nEvaluatedGridPoints, nEvaluations.load());
      BOOST_CHECK(statistics.stopReason == RefinementStopReason::GridPointBudget);
      checkEqualsFreshGrid(sg);
    }
  }
}

BOOST_AUTO_TEST_CASE(Later_Calls_Reuse_Every_Value_A_New_Object_Restarts) {
  const SourceFunc f(anisoExp);
  SparseGrid sg(makeInstr(configs()[2], 0));
  QuadratureRefinementTarget target(f);
  DimensionAdaptiveRefinement refinement(sg, target, budget(1000));
  refinement.refine();
  const LvlMIVec levels = sg.getLevels();

  // Nothing new fits a budget of 0: the call evaluates nothing and changes nothing
  refinement.setOptions(budget(0));
  RefinementStatistics statistics = refinement.refine();
  BOOST_CHECK_EQUAL(statistics.nEvaluatedTensorGrids, 0u);
  BOOST_CHECK(sameLevels(sg.getLevels(), levels));

  // Another indicator continues where the last call stopped
  RefinementOptions options = budget(500);
  options.indicator = RefinementIndicator::Gain;
  refinement.setOptions(options);
  statistics = refinement.refine();
  BOOST_CHECK(statistics.nEvaluatedGridPoints <= 500);
  BOOST_CHECK(statistics.nDecisions > 0);

  // A new object knows nothing about the grid: it must evaluate the grid first
  QuadratureRefinementTarget otherTarget(f);
  DimensionAdaptiveRefinement other(sg, otherTarget, budget(0));
  const LvlMIVec levelsBefore = sg.getLevels();
  statistics = other.refine();
  BOOST_CHECK_EQUAL(statistics.nEvaluatedTensorGrids, 0u);
  BOOST_CHECK(statistics.stopReason == RefinementStopReason::GridPointBudget);
  BOOST_CHECK(std::isnan(other.getEstimate()[0]));  // the grid's tensor grids have no values
  BOOST_CHECK(std::isnan(other.getErrorEstimate()));
  BOOST_CHECK(sameLevels(sg.getLevels(), levelsBefore));
  checkEqualsFreshGrid(sg);

  // ... and the first object notices that the other one handed the grid back (a new storage)
  refinement.setOptions(budget(0));
  statistics = refinement.refine();
  BOOST_CHECK(statistics.stopReason == RefinementStopReason::GridPointBudget);
}

BOOST_AUTO_TEST_CASE(Steps_Make_One_Decision_Each) {
  const SourceFunc f(anisoExp);
  SparseGrid sg(makeInstr(configs()[1], 0));
  QuadratureRefinementTarget target(f);
  RefinementOptions options;
  options.maxLevel = 3;
  DimensionAdaptiveRefinement refinement(sg, target, options);

  size_t nSteps = 0;
  RefinementStatistics statistics;
  do {
    statistics = refinement.step();
    checkEqualsFreshGrid(sg);
    BOOST_CHECK(statistics.nDecisions <= 1);
    nSteps++;
  } while (statistics.stopReason == RefinementStopReason::StepDone && nSteps < 100);
  BOOST_CHECK(statistics.stopReason == RefinementStopReason::NoCandidates);

  // Exhausted under maxLevel 3: the full tensor product of levels 0 ... 3, as refine() reaches it
  SparseGrid full(makeInstr(configs()[1], 0));
  QuadratureRefinementTarget fullTarget(f);
  options.maxGridPoints = 1000000;
  DimensionAdaptiveRefinement fullRefinement(full, fullTarget, options);
  BOOST_CHECK(fullRefinement.refine().stopReason == RefinementStopReason::NoCandidates);
  BOOST_CHECK(sameLevels(full.getLevels(), sg.getLevels()));
  BOOST_CHECK_EQUAL(bitsOf(fullRefinement.getEstimate()[0]), bitsOf(refinement.getEstimate()[0]));
}

BOOST_AUTO_TEST_CASE(Stopping_Criteria) {
  const SourceFunc f(anisoExp);
  {
    SparseGrid sg(makeInstr(configs()[2], 0));
    QuadratureRefinementTarget target(f);
    RefinementOptions options;
    options.absoluteTolerance = 1e-6;
    DimensionAdaptiveRefinement refinement(sg, target, options);
    BOOST_CHECK(refinement.refine().stopReason == RefinementStopReason::Tolerance);
    BOOST_CHECK(refinement.getErrorEstimate() <= 1e-6);
  }
  {
    SparseGrid sg(makeInstr(configs()[2], 0));
    QuadratureRefinementTarget target(f);
    RefinementOptions options;
    options.relativeTolerance = 1e-5;
    DimensionAdaptiveRefinement refinement(sg, target, options);
    BOOST_CHECK(refinement.refine().stopReason == RefinementStopReason::RelativeTolerance);
    BOOST_CHECK(refinement.getErrorEstimate() <= 1e-5 * std::abs(refinement.getEstimate()[0]));
  }
  {
    SparseGrid sg(makeInstr(configs()[2], 1));
    const LvlMIVec levels = sg.getLevels();
    const double before = quadrature(f, sg);
    QuadratureRefinementTarget target(f);
    RefinementOptions options;
    options.maxSeconds = 0.0;
    BOOST_CHECK(refine(sg, target, options).stopReason == RefinementStopReason::TimeLimit);
    BOOST_CHECK(sameLevels(sg.getLevels(), levels));
    BOOST_CHECK_EQUAL(bitsOf(quadrature(f, sg)), bitsOf(before));
  }
  {
    SparseGrid sg(makeInstr(configs()[2], 0));
    QuadratureRefinementTarget target(f);
    RefinementOptions options;
    options.maxTensorGrids = 10;
    const RefinementStatistics statistics = refine(sg, target, options);
    BOOST_CHECK(statistics.stopReason == RefinementStopReason::TensorGridLimit);
    BOOST_CHECK(statistics.nTensorGrids <= 10);
  }
  {
    SparseGrid sg(makeInstr(configs()[2], 0));
    QuadratureRefinementTarget target(f);
    RefinementOptions options = budget(std::numeric_limits<size_t>::max() - 1);
    options.maxLevel = 2;
    BOOST_CHECK(refine(sg, target, options).stopReason == RefinementStopReason::NoCandidates);
    for (size_t tg = 0; tg < sg.nTG(); tg++) {
      for (size_t dim = 0; dim < sg.nDim(); dim++) BOOST_CHECK(sg.getLevels()[tg][dim] <= 2);
    }
  }
  {
    SparseGrid sg(makeInstr(configs()[2], 0));
    QuadratureRefinementTarget target(f);
    BOOST_CHECK_THROW(refine(sg, target, RefinementOptions()), std::invalid_argument);
  }
}

BOOST_AUTO_TEST_CASE(Empty_And_One_Dimensional_Grids) {
  const SourceFunc f(anisoExp);
  {
    const LvlMIVec none(3, 0);
    MIVecSGGenInstr instr(none);
    instr.setNodeFamily(getClenshawCurtisNodes());
    instr.setBoundaryLevelOffset(1);
    SparseGrid sg(instr);
    BOOST_REQUIRE_EQUAL(sg.nTG(), 0u);
    QuadratureRefinementTarget target(f);
    refine(sg, target, budget(300));
    BOOST_CHECK(sg.nTG() > 0);
    checkEqualsFreshGrid(sg);
  }
  {
    SparseGrid sg(makeInstr(configs()[0], 0));
    QuadratureRefinementTarget target(f);
    DimensionAdaptiveRefinement refinement(sg, target, budget(600));
    refinement.refine();
    // A chain of levels in one dimension: one tensor grid of coefficient 1
    BOOST_CHECK_EQUAL(sg.nTG(), 1u);
    BOOST_CHECK_CLOSE_FRACTION(refinement.getEstimate()[0], std::exp(1.0) - 1.0, 1e-13);
  }
}

BOOST_AUTO_TEST_CASE(Exceptions_Of_The_Source_Function_Leave_A_Valid_Grid) {
  std::atomic<size_t> nEvaluations{0};
  const SourceFunc f([&](const DataVector& x) {
    if (++nEvaluations == 120) throw std::runtime_error("source function failed");
    return anisoExp(x);
  });
  SparseGrid sg(makeInstr(configs()[2], 0));
  QuadratureRefinementTarget target(f);
  DimensionAdaptiveRefinement refinement(sg, target, budget(5000));
  BOOST_CHECK_THROW(refinement.refine(), std::runtime_error);
  checkEqualsFreshGrid(sg);
}

BOOST_AUTO_TEST_CASE(Asynchronous_Schedule_Gives_A_Valid_Grid) {
  const SourceFunc f(anisoExp);
  RefinementOptions options = budget(4000);
  options.schedule = RefinementSchedule::Asynchronous;
  for (const int nThreads : {1, 16}) {
    BOOST_TEST_CONTEXT("threads " << nThreads) {
      withThreads(nThreads, [&]() {
        SparseGrid sg(makeInstr(configs()[2], 0));
        QuadratureRefinementTarget target(f);
        DimensionAdaptiveRefinement refinement(sg, target, options);
        refinement.refine();
        checkEqualsFreshGrid(sg);
        BOOST_CHECK_EQUAL(bitsOf(refinement.getEstimate()[0]), bitsOf(quadrature(f, sg)));
      });
    }
  }
}

BOOST_AUTO_TEST_CASE(Evaluation_Ahead_Keeps_The_Result_Independent_Of_The_Thread_Count) {
  const SourceFunc f(anisoExp);
  RefinementOptions options = budget(5000);
  options.maxBatchSize = 2;
  options.speculationWindow = 6;
  std::vector<uint64_t> referenceBits;
  LvlMIVec referenceLevels(4, 0);

  for (const int nThreads : {1, 2, 16}) {
    BOOST_TEST_CONTEXT("threads " << nThreads) {
      withThreads(nThreads, [&]() {
        SparseGrid sg(makeInstr(configs()[5], 0));
        QuadratureRefinementTarget target(f);
        DimensionAdaptiveRefinement refinement(sg, target, options);
        const RefinementStatistics statistics = refinement.refine();
        checkEqualsFreshGrid(sg);
        BOOST_CHECK(statistics.nUnusedSpeculativeTensorGrids <= statistics.nSpeculativeTensorGrids);
        BOOST_CHECK_EQUAL(bitsOf(refinement.getEstimate()[0]), bitsOf(quadrature(f, sg)));
        const std::vector<uint64_t> bits{bitsOf(refinement.getEstimate()[0]),
                                         bitsOf(refinement.getErrorEstimate())};
        if (nThreads == 1) {
          referenceBits = bits;
          referenceLevels = sg.getLevels();
          return;
        }
        BOOST_CHECK(sameLevels(sg.getLevels(), referenceLevels));
        BOOST_CHECK(bits == referenceBits);
      });
    }
  }
}

BOOST_AUTO_TEST_CASE(Anisotropic_Function_Refines_Its_Important_Dimension) {
  // Algebraic convergence in x0 (a kink of the derivative): the gains stay far above round-off
  const SourceFunc f([](const DataVector& x) { return std::pow(std::abs(x[0] - 0.3), 1.5); });
  SparseGrid sg(makeInstr(configs()[2], 0));
  QuadratureRefinementTarget target(f);
  refine(sg, target, budget(3000));

  LvlType maxLvl0 = 0, maxOther = 0;
  for (size_t tg = 0; tg < sg.nTG(); tg++) {
    maxLvl0 = std::max(maxLvl0, sg.getLevels()[tg][0]);
    for (size_t dim = 1; dim < sg.nDim(); dim++)
      maxOther = std::max(maxOther, sg.getLevels()[tg][dim]);
  }
  BOOST_CHECK(maxLvl0 >= 4);
  BOOST_CHECK(maxOther <= 1);  // only the first look at the other dimensions
}

BOOST_AUTO_TEST_CASE(Smooth_Function_Is_Integrated_Accurately) {
  // d = 5, unit cube: the exact integral of anisoExp is the product of (e^{a_k} - 1) / a_k
  Config config = configs()[2];
  config.nDim = 5;
  config.nodes.assign(5, getClenshawCurtisNodes());
  config.domain.clear();
  double exact = 1.0;
  for (size_t k = 0; k < 5; k++) {
    const double a = 1.0 / static_cast<double>((k + 1) * (k + 1));
    exact *= (std::exp(a) - 1.0) / a;
  }

  const SourceFunc f(anisoExp);
  SparseGrid sg(makeInstr(config, 0));
  QuadratureRefinementTarget target(f);
  DimensionAdaptiveRefinement refinement(sg, target, budget(3000));
  refinement.refine();

  const double error = std::abs(refinement.getEstimate()[0] - exact);
  BOOST_TEST_MESSAGE("error " << error << ", estimate " << refinement.getErrorEstimate());
  BOOST_CHECK(error < 1e-9);
  BOOST_CHECK(refinement.getErrorEstimate() < 1e-7);
}

BOOST_AUTO_TEST_SUITE_END()
