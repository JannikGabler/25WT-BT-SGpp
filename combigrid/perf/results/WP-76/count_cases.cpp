// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-76: source-function evaluations of the e2e group expensive/ (same configurations, domains,
// MI-set seeds and points as perf_e2e.cpp, with a counting perf::expSum; cached cases: cache
// inserts = distinct points). Built with build_proto.sh; output: counts.md.
#include <atomic>
#include <cstdio>
#include <functional>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <string>

#include "../../../benchmarks/perf/perf_problems.hpp"

using namespace sgpp::combigrid;  // NOLINT (measurement driver)
namespace perf = sgpp::combigrid::perf;
using DataVector = sgpp::base::DataVector;

namespace {

std::atomic<size_t> evals{0};
double countingExp(const DataVector& x) {
  evals.fetch_add(1, std::memory_order_relaxed);
  return perf::expSum(x);
}

template <typename Grid>
Grid completeGrid(const std::string& nodes, size_t d, LvlType L) {
  CompleteSGGenInstr gi(L, d);
  perf::configure(gi, nodes, 1, perf::unitDomain(d));
  return Grid(gi);
}

template <typename Grid>
void quad(const char* name, size_t d, LvlType L) {
  evals = 0;
  quadrature(SourceFunc(countingExp), completeGrid<Grid>("cc", d, L));
  std::printf("| %s | %zu |\n", name, evals.load());
}

template <typename Grid>
void mivec(const char* name, size_t d, LvlType S, size_t n) {
  const LvlMIVec mis = perf::anisotropicMIs(d, n, S, 2000 + d);
  MIVecSGGenInstr gi(mis);
  perf::configure(gi, "cc", 1, perf::unitDomain(d));
  evals = 0;
  quadrature(SourceFunc(countingExp), Grid(gi));
  std::printf("| %s | %zu |\n", name, evals.load());
}

template <typename Grid>
void interp(const char* name, bool linear, const std::string& nodes, size_t d, LvlType L,
            size_t p) {
  const Grid grid = completeGrid<Grid>(nodes, d, L);
  const SourceFunc f(countingExp);
  evals = 0;
  for (const DataVector& x : perf::randomPoints(perf::unitDomain(d), p, 1000 + d)) {
    if (linear) {
      interpolateLinear(f, x, grid);
    } else {
      interpolate(f, x, grid);
    }
  }
  std::printf("| %s | %zu |\n", name, evals.load());
}

void cachedQuad(const char* name, size_t d, LvlType L) {
  const CachedSourceFunc f(perf::expSum);
  quadrature(f, completeGrid<SparseGrid>("cc", d, L));
  std::printf("| %s | %zu (cache inserts; %zu lookups) |\n", name, f.statistics().inserts,
              f.statistics().lookups);
}

void cachedInterp(const char* name, bool linear, const std::string& nodes, size_t d, LvlType L,
                  size_t p) {
  const SparseGrid grid = completeGrid<SparseGrid>(nodes, d, L);
  const CachedSourceFunc f(perf::expSum);
  for (const DataVector& x : perf::randomPoints(perf::unitDomain(d), p, 1000 + d)) {
    if (linear) {
      interpolateLinear(f, x, grid);
    } else {
      interpolate(f, x, grid);
    }
  }
  std::printf("| %s | %zu (cache inserts; %zu lookups) |\n", name, f.statistics().inserts,
              f.statistics().lookups);
}

}  // namespace

int main() {
  std::printf("| case (expensive/...) | evaluations |\n|---|---|\n");
  quad<SparseGrid>("quad/sg/cc-exp/b1/d10/L4", 10, 4);
  quad<DeltasSparseGrid>("quad/dsg/cc-exp/b1/d10/L4", 10, 4);
  quad<SparseGrid>("quad/sg/cc-exp/b1/d20/L3", 20, 3);
  quad<DeltasSparseGrid>("quad/dsg/cc-exp/b1/d20/L3", 20, 3);
  quad<SparseGrid>("quad/sg/cc-exp/b1/d15/L5", 15, 5);
  quad<DeltasSparseGrid>("quad/dsg/cc-exp/b1/d15/L5", 15, 5);
  mivec<SparseGrid>("mivec/sg/cc-exp/b1/d15/S6/n300", 15, 6, 300);
  mivec<DeltasSparseGrid>("mivec/dsg/cc-exp/b1/d15/S6/n300", 15, 6, 300);
  mivec<SparseGrid>("mivec/sg/cc-exp/b1/d20/S10/n300", 20, 10, 300);
  mivec<DeltasSparseGrid>("mivec/dsg/cc-exp/b1/d20/S10/n300", 20, 10, 300);
  interp<SparseGrid>("interp/sg/cc-exp/b1/d10/L3/p8", false, "cc", 10, 3, 8);
  interp<DeltasSparseGrid>("interp/dsg/cc-exp/b1/d10/L3/p8", false, "cc", 10, 3, 8);
  interp<SparseGrid>("interp/sg/cc-exp/b1/d7/L3/p8", false, "cc", 7, 3, 8);
  interp<SparseGrid>("interp/sg/cc-exp/b1/d10/L4/p16", false, "cc", 10, 4, 16);
  interp<DeltasSparseGrid>("interp/dsg/cc-exp/b1/d10/L4/p16", false, "cc", 10, 4, 16);
  interp<SparseGrid>("interplin/sg/equi-exp/b1/d5/L5/p16", true, "equi", 5, 5, 16);
  interp<DeltasSparseGrid>("interplin/dsg/equi-exp/b1/d5/L5/p16", true, "equi", 5, 5, 16);
  interp<SparseGrid>("interplin/sg/equi-exp/b1/d10/L4/p16", true, "equi", 10, 4, 16);
  interp<DeltasSparseGrid>("interplin/dsg/equi-exp/b1/d10/L4/p16", true, "equi", 10, 4, 16);
  interp<SparseGrid>("interplin/sg/equi-exp/b1/d8/L4/p32", true, "equi", 8, 4, 32);
  cachedQuad("quad/sg/cc-exp/b1/d10/L4 -cached", 10, 4);
  cachedInterp("interp/sg/cc-exp/b1/d10/L3/p8 -cached", false, "cc", 10, 3, 8);
  cachedInterp("interplin/sg/equi-exp/b1/d5/L5/p16 -cached", true, "equi", 5, 5, 16);
  cachedInterp("interplin/sg/equi-exp/b1/d8/L4/p32 -cached", true, "equi", 8, 4, 32);
  return 0;
}
