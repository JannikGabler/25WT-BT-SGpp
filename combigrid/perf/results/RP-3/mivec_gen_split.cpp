// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// RP-3 scratch diagnostic (not part of the tree): split of the MIVecSGGenInstr generation of the
// e2e mivec cases into closure, CT coefficients, filtering/partitioning and tensor-grid
// materialization. Same MI sets and configuration as perf_e2e's addMIVec.

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp_combigrid.hpp>
#include <optional>
#include <vector>

#include "perf_problems.hpp"

using namespace sgpp::combigrid;  // NOLINT

namespace {

template <typename F>
double timeIt(F&& f) {
  const auto t0 = std::chrono::steady_clock::now();
  f();
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
}

double median(std::vector<double> v) {
  std::sort(v.begin(), v.end());
  return v[v.size() / 2];
}

}  // namespace

int main(int argc, char** argv) {
  const int reps = argc > 1 ? std::atoi(argv[1]) : 21;
  const struct {
    size_t d;
    LvlType S;
    size_t n;
  } cases[] = {{5, 16, 1000}, {10, 12, 1000}, {15, 10, 300}, {20, 7, 300}};

  std::printf("| case | nMI in | closure size | nTG | closure | CT coeffs | genReducedMIVecWithCoeffs |"
              " SparseGrid (generation) | materialization = SG - reduced |\n");
  std::printf("|---|---|---|---|---|---|---|---|---|\n");
  for (const auto& c : cases) {
    const LvlMIVec mis = perf::anisotropicMIs(c.d, c.n, c.S, 2000 + c.d);
    MIVecSGGenInstr genInstr(mis);
    perf::configure(genInstr, "cc", 1, perf::unitDomain(c.d));
    size_t closureSize = 0, nTG = 0;
    { SparseGrid warm(genInstr); nTG = warm.nTG(); }

    std::vector<double> tClosure, tCoeffs, tReduced, tSG;
    for (int r = 0; r < reps; r++) {
      std::optional<LvlMIVec> closure;
      tClosure.push_back(timeIt([&] { closure.emplace(mis.downwardsClosure()); }));
      closureSize = closure->nMI();
      std::vector<CTCoeffType> coeffs;
      tCoeffs.push_back(timeIt([&] { coeffs = tools::computeCTCoeffs(*closure); }));
      tReduced.push_back(timeIt([&] { auto p = genInstr.genReducedMIVecWithCoeffs(); }));
      std::optional<SparseGrid> sg;
      tSG.push_back(timeIt([&] { sg.emplace(genInstr); }));
    }
    const double mc = median(tClosure), mk = median(tCoeffs), mr = median(tReduced),
                 ms = median(tSG);
    std::printf("| mivec d%zu/S%u/n%zu | %zu | %zu | %zu | %.3f ms | %.3f ms | %.3f ms | %.3f ms |"
                " %.3f ms |\n",
                c.d, static_cast<unsigned>(c.S), c.n, mis.nMI(), closureSize, nTG, mc, mk, mr, ms,
                ms - mr);
  }
  return 0;
}
