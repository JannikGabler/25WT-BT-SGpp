// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file bench_structures.cpp
 * @brief Costs of rebuilding the library's structures for the growing multi-index set of an
 * adaptive run (a dump of run_quad: one multi-index per line, in evaluation order).
 *
 * Usage: bench_structures <dump file> <boundary offset> [func]
 * Output: one line per prefix size with the median time of each step.
 */

#include <sgpp_combigrid.hpp>

#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

using namespace sgpp::combigrid;  // NOLINT(build/namespaces)
using sgpp::base::DataVector;

namespace {

double now() {
  return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

/// Median wall time of @p reps calls in seconds.
double timeIt(const int reps, const std::function<void()>& call) {
  std::vector<double> times;
  for (int r = 0; r < reps; r++) {
    const double t0 = now();
    call();
    times.push_back(now() - t0);
  }
  std::ranges::sort(times);
  return times[times.size() / 2];
}

void configure(SGGenInstr& genInstr, const LvlType boundaryOffset) {
  genInstr.setDomain({0.0, 1.0});
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(boundaryOffset);
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) return 2;
  const LvlType boundaryOffset = static_cast<LvlType>(std::stoul(argv[2]));
  std::vector<std::vector<LvlType>> all;
  std::ifstream in(argv[1]);
  for (std::string line; std::getline(in, line);) {
    std::istringstream stream(line);
    std::vector<LvlType> mi;
    for (LvlType l; stream >> l;) mi.push_back(l);
    if (!mi.empty()) all.push_back(mi);
  }
  const size_t nDim = all[0].size();
  const SourceFunc func([](const DataVector& x) {
    double s = 0;
    for (size_t k = 0; k < x.size(); k++) s += x[k] * x[k];
    return std::exp(-s);
  });

  std::printf("# d=%zu, %zu multi-indices, boundary offset %u; times in ms (median)\n", nDim,
              all.size(), boundaryOffset);
  std::printf("%8s %8s %10s %10s %10s %10s %10s %10s %10s %12s %12s\n", "nMI", "nTG", "coeffs",
              "instr+red", "SG", "DSG", "quadSG", "quadDSG", "quadDSG2", "pointsSG", "pointsDSG");
  std::vector<size_t> sizes;
  for (size_t n = 100; n < all.size(); n *= 3) sizes.push_back(n);
  sizes.push_back(all.size());
  for (const size_t n : sizes) {
    LvlMIVec miVec(nDim, n);
    for (size_t i = 0; i < n; i++) miVec.setMI(i, LvlMI(all[i]));
    const int reps = n <= 1000 ? 9 : 5;
    const double tCoeffs = timeIt(reps, [&] { (void)tools::computeCTCoeffs(miVec); });
    size_t nTG = 0;
    const double tInstr = timeIt(reps, [&] {
      MIVecSGGenInstr genInstr(miVec);
      configure(genInstr, boundaryOffset);
      nTG = genInstr.genReducedMIVecWithCoeffs().second.size();
    });
    MIVecSGGenInstr genInstr(miVec);
    configure(genInstr, boundaryOffset);
    const double tSG = timeIt(reps, [&] { const SparseGrid sg(genInstr); });
    const double tDSG = timeIt(reps, [&] { const DeltasSparseGrid dsg(genInstr); });
    const SparseGrid sg(genInstr);
    size_t pointsSG = 0;
    for (size_t t = 0; t < sg.nTG(); t++) pointsSG += sg.getUnitTensorGrid(t).tensorGrid.nGP();
    const double tQuadSG = timeIt(3, [&] { (void)quadrature(func, sg); });
    // First DSG quadrature of a new grid, and a repeated one on the same grid.
    const double tQuadDSG = timeIt(3, [&] {
      const DeltasSparseGrid dsg(genInstr);
      (void)quadrature(func, dsg);
    }) - tDSG;
    const DeltasSparseGrid dsg(genInstr);
    size_t pointsDSG = 0;
    for (size_t t = 0; t < dsg.nTG(); t++) pointsDSG += dsg.getUnitTensorGrid(t).tensorGrid.nGP();
    const double tQuadDSG2 = timeIt(3, [&] { (void)quadrature(func, dsg); });
    std::printf("%8zu %8zu %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %12zu %12zu\n", n, nTG,
                1e3 * tCoeffs, 1e3 * tInstr, 1e3 * tSG, 1e3 * tDSG, 1e3 * tQuadSG, 1e3 * tQuadDSG,
                1e3 * tQuadDSG2, pointsSG, pointsDSG);
    std::fflush(stdout);
  }

  // Today's public API per candidate: one single-tensor-grid SparseGrid per multi-index.
  const size_t nSingle = std::min<size_t>(all.size(), 2000);
  double sum = 0;
  size_t points = 0;
  const double t0 = now();
  for (size_t i = 0; i < nSingle; i++) {
    LvlMIVec single(nDim, 1);
    single.setMI(0, LvlMI(all[i]));
    MIVecSGGenInstr genInstr(single);
    configure(genInstr, boundaryOffset);
    const SparseGrid sg(genInstr);
    points += sg.getUnitTensorGrid(sg.nTG() - 1).tensorGrid.nGP();
    sum += quadrature(func, sg);
  }
  const double tSingle = now() - t0;
  std::printf("# per-candidate loop with the public API (single-MI instruction + SparseGrid + "
              "quadrature): %.2f us per multi-index over the first %zu (%.1f points per "
              "multi-index on average, checksum %.6e)\n",
              1e6 * tSingle / static_cast<double>(nSingle), nSingle,
              static_cast<double>(points) / static_cast<double>(nSingle), sum);
  return 0;
}
