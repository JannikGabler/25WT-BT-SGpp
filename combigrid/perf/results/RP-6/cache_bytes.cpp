// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file
 * @brief RP-6: cross-call operator data cache size (`getCrossCallOperatorDataCacheBytes()`,
 * entries) after one default-options operator call on the grids of the judged DSG e2e cases
 * (quad/dsg, interp/dsg, interplin/dsg; their domains and node families). Public API only.
 * Output: one table row per case.
 */

#include <sgpp_combigrid.hpp>

#include <cstdio>
#include <string>
#include <vector>

#include "perf_problems.hpp"

using sgpp::base::DataVector;
namespace perf = sgpp::combigrid::perf;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::SourceFunc;

namespace {

void report(const std::string& name, const DeltasSparseGrid& grid) {
  std::printf("| %s | %zu | %.2f MiB |", name.c_str(),
              grid.getCrossCallOperatorDataCacheEntries().size(),
              static_cast<double>(grid.getCrossCallOperatorDataCacheBytes()) / (1024.0 * 1024.0));
  for (const auto& e : grid.getCrossCallOperatorDataCacheEntries()) {
    std::printf(" %s %.2f MiB;", e.operatorName.c_str(),
                static_cast<double>(e.bytes) / (1024.0 * 1024.0));
  }
  std::printf("\n");
}

void quad(const std::string& nodes, LvlType b, size_t d, LvlType L) {
  CompleteSGGenInstr genInstr(L, d);
  perf::configure(genInstr, nodes, b, perf::unitDomain(d));
  const DeltasSparseGrid grid(genInstr);
  const SourceFunc f(perf::expSum);
  (void)quadrature(f, grid);
  report("quad/dsg/" + nodes + "/b" + std::to_string(b) + "/d" + std::to_string(d) + "/L" +
             std::to_string(L),
         grid);
}

void interp(bool linear, const std::string& nodes, LvlType b, size_t d, LvlType L) {
  const HyperCubeArea domain = perf::shiftedDomain(d);
  CompleteSGGenInstr genInstr(L, d);
  perf::configure(genInstr, nodes, b, domain);
  const DeltasSparseGrid grid(genInstr);
  const SourceFunc f(perf::cosProd);
  const std::vector<DataVector> points = perf::randomPoints(domain, 1, 1000 + d);
  (void)(linear ? interpolateLinear(f, points[0], grid) : interpolate(f, points[0], grid));
  report(std::string(linear ? "interplin" : "interp") + "/dsg/" + nodes + "/b" +
             std::to_string(b) + "/d" + std::to_string(d) + "/L" + std::to_string(L),
         grid);
}

}  // namespace

int main() {
  std::printf("| case | entries | bytes | per entry |\n|---|---|---|---|\n");
  quad("equi", 0, 3, 16);
  quad("equi", 0, 5, 12);
  quad("equi", 0, 7, 10);
  quad("equi", 0, 10, 7);
  quad("equi", 1, 15, 7);
  quad("equi", 1, 20, 6);
  quad("cc", 0, 5, 12);
  quad("cc", 0, 7, 10);
  quad("cc", 0, 10, 7);
  interp(false, "cc", 0, 2, 12);
  interp(false, "cc", 0, 3, 9);
  interp(false, "cc", 0, 5, 6);
  interp(false, "cc", 0, 7, 4);
  interp(false, "cc", 0, 10, 2);
  interp(false, "equi", 0, 3, 5);
  interp(true, "equi", 1, 2, 16);
  interp(true, "equi", 1, 5, 8);
  interp(true, "equi", 1, 8, 5);
  interp(true, "equi", 1, 10, 5);
  return 0;
}
