// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_e2e.cpp
 * @brief End-to-end and phase-level benchmarks of the combigrid performance suite.
 *
 * PROTECTED FILE (see combigrid/perf/README.md). Uses the public API of the module only, so that
 * it stays valid across internal optimizations.
 *
 * Every case runs a complete user pipeline and times its phases:
 * - @c setup:      source function and generation instruction (and inputs such as points/MIs),
 * - @c generation: construction of the (deltas) sparse grid,
 * - @c operator:   quadrature / interpolation at all query points,
 * - @c total:      wall time of the whole pipeline (recorded by the harness).
 *
 * Case names: <operator>/<grid>/<nodes>[-<integrand>]/b<boundary level offset>/d<dim>/L<level>[/...]
 * - operator: quad, interp (global), interplin (multilinear), mivec (MIVecSGGenInstr + quadrature)
 * - grid:     sg (SparseGrid), dsg (DeltasSparseGrid)
 * - nodes:    equi, cc (Clenshaw-Curtis), cheb1; integrand: exp (expSum), uq (thesis forward-UQ
 *             integrand), sin (sinProd)
 * All grids use the doubling level-to-grid-point-count function. Sizes are chosen such that one
 * pipeline takes about 0.1-4 s with 16 threads on the benchmarking machine (Ryzen 7 5825U).
 *
 * Usage: see perf_harness.hpp, normally driven by combigrid/perf/scripts/perf.py.
 */

#include <cstddef>
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

struct Integrand {
  const char* name;
  double (*func)(const DataVector&);
  double (*exact)(size_t nDim);  // over [0, 1]^d
};

double expSumExact(const size_t nDim) { return perf::expSumIntegral(perf::unitDomain(nDim)); }
double sinProdExact(const size_t nDim) { return perf::sinProdIntegral(perf::unitDomain(nDim)); }

const Integrand EXP{"exp", perf::expSum, expSumExact};
const Integrand UQ{"uq", perf::uqIntegrand, perf::uqIntegral};
const Integrand SIN{"sin", perf::sinProd, sinProdExact};

template <typename Grid>
size_t sumOfGridPoints(const Grid& grid) {
  size_t sum = 0;
  for (const auto& tgData : grid) sum += tgData.tensorGrid.nGP();
  return sum;
}

std::string caseName(const std::string& op, const std::string& grid, const std::string& nodes,
                     const LvlType boundary, const size_t nDim, const std::string& level) {
  return op + "/" + grid + "/" + nodes + "/b" + std::to_string(boundary) + "/d" +
         std::to_string(nDim) + "/" + level;
}

/// Quadrature of an integrand over [0, 1]^d on a complete (deltas) sparse grid.
template <typename Grid>
void addQuadrature(Suite& suite, const std::string& gridName, const std::string& nodes,
                   const Integrand& integrand, const LvlType boundary, const size_t nDim,
                   const LvlType maxLvl) {
  suite.addPipeline(
      caseName("quad", gridName, nodes + "-" + integrand.name, boundary, nDim,
               "L" + std::to_string(maxLvl)),
      [=](PipelineRun& run) {
        std::optional<SourceFunc> sourceFunc;
        std::optional<CompleteSGGenInstr> genInstr;
        std::optional<Grid> grid;
        double result = 0.0;

        run.phase("setup", [&]() {
          sourceFunc.emplace(integrand.func);
          genInstr.emplace(maxLvl, nDim);
          perf::configure(*genInstr, nodes, boundary, perf::unitDomain(nDim));
        });
        run.phase("generation", [&]() { grid.emplace(*genInstr); });
        run.phase("operator", [&]() { result = quadrature(*sourceFunc, *grid); });

        run.setResult(result, integrand.exact(nDim));
        run.setCounter("nTG", static_cast<double>(grid->nTG()));
        run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
      });
}

/// Global (or multilinear) interpolation of perf::cosProd on the shifted domain at fixed points.
void addInterpolation(Suite& suite, const bool linear, const std::string& nodes,
                      const LvlType boundary, const size_t nDim, const LvlType maxLvl,
                      const size_t nPoints) {
  suite.addPipeline(
      caseName(linear ? "interplin" : "interp", "sg", nodes, boundary, nDim,
               "L" + std::to_string(maxLvl) + "/p" + std::to_string(nPoints)),
      [=](PipelineRun& run) {
        const HyperCubeArea domain = perf::shiftedDomain(nDim);
        std::optional<SourceFunc> sourceFunc;
        std::optional<CompleteSGGenInstr> genInstr;
        std::optional<SparseGrid> grid;
        std::vector<DataVector> points;
        double sum = 0.0;

        run.phase("setup", [&]() {
          sourceFunc.emplace(perf::cosProd);
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
        for (const DataVector& point : points) exact += perf::cosProd(point);
        run.setResult(sum, exact);
        run.setCounter("nTG", static_cast<double>(grid->nTG()));
        run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
      });
}

/// MIVecSGGenInstr from an anisotropic, not downwards-closed MI set (closure, coefficients,
/// tensor grids), followed by a quadrature of perf::expSum.
void addMIVec(Suite& suite, const std::string& nodes, const LvlType boundary, const size_t nDim,
              const LvlType maxSum, const size_t nMI) {
  suite.addPipeline(caseName("mivec", "sg", nodes + "-exp", boundary, nDim,
                             "S" + std::to_string(maxSum) + "/n" + std::to_string(nMI)),
                    [=](PipelineRun& run) {
                      std::optional<SourceFunc> sourceFunc;
                      std::optional<LvlMIVec> mis;
                      std::optional<MIVecSGGenInstr> genInstr;
                      std::optional<SparseGrid> grid;
                      double result = 0.0;

                      run.phase("setup", [&]() {
                        sourceFunc.emplace(perf::expSum);
                        mis.emplace(perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim));
                        genInstr.emplace(*mis);
                        perf::configure(*genInstr, nodes, boundary, perf::unitDomain(nDim));
                      });
                      run.phase("generation", [&]() { grid.emplace(*genInstr); });
                      run.phase("operator",
                                [&]() { result = quadrature(*sourceFunc, *grid); });

                      run.setResult(result, perf::expSumIntegral(perf::unitDomain(nDim)));
                      run.setCounter("nTG", static_cast<double>(grid->nTG()));
                      run.setCounter("sumGP", static_cast<double>(sumOfGridPoints(*grid)));
                    });
}

}  // namespace

int main(int argc, char** argv) {
  Suite suite("e2e", 3, 1);

  // Complete sparse grid quadrature
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 0, 2, 20);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 0, 5, 10);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 0, 7, 7);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 0, 10, 4);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 1, 10, 7);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 1, 15, 6);
  addQuadrature<SparseGrid>(suite, "sg", "equi", EXP, 1, 20, 5);
  addQuadrature<SparseGrid>(suite, "sg", "equi", SIN, 0, 7, 7);
  addQuadrature<SparseGrid>(suite, "sg", "cc", UQ, 0, 2, 20);
  addQuadrature<SparseGrid>(suite, "sg", "cc", UQ, 0, 3, 14);
  addQuadrature<SparseGrid>(suite, "sg", "cc", UQ, 0, 5, 10);
  addQuadrature<SparseGrid>(suite, "sg", "cc", UQ, 0, 7, 7);
  addQuadrature<SparseGrid>(suite, "sg", "cc", UQ, 0, 10, 4);

  // Deltas sparse grid quadrature
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 0, 3, 16);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 0, 5, 12);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 0, 7, 10);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 0, 10, 7);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 1, 15, 7);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "equi", EXP, 1, 20, 6);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", UQ, 0, 5, 12);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", UQ, 0, 7, 10);
  addQuadrature<DeltasSparseGrid>(suite, "dsg", "cc", UQ, 0, 10, 7);

  // Global interpolation
  addInterpolation(suite, false, "cc", 0, 2, 12, 64);
  addInterpolation(suite, false, "cc", 0, 3, 9, 64);
  addInterpolation(suite, false, "cc", 0, 5, 6, 64);
  addInterpolation(suite, false, "cc", 0, 7, 4, 16);
  addInterpolation(suite, false, "cc", 0, 10, 2, 8);
  addInterpolation(suite, false, "cheb1", 0, 3, 6, 64);
  addInterpolation(suite, false, "cheb1", 0, 5, 4, 64);
  addInterpolation(suite, false, "equi", 0, 3, 5, 64);

  // Multilinear interpolation
  addInterpolation(suite, true, "equi", 1, 2, 16, 16384);
  addInterpolation(suite, true, "equi", 1, 5, 8, 1024);
  addInterpolation(suite, true, "equi", 1, 8, 5, 256);
  addInterpolation(suite, true, "equi", 1, 10, 5, 32);

  // Sparse grids from (adaptive-like) multi-index sets
  addMIVec(suite, "cc", 1, 5, 16, 1000);
  addMIVec(suite, "cc", 1, 10, 12, 1000);
  addMIVec(suite, "cc", 1, 15, 10, 300);
  addMIVec(suite, "cc", 1, 20, 7, 300);

  return suite.main(argc, argv);
}
