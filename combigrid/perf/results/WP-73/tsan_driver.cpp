// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tsan_driver.cpp
 * @brief ThreadSanitizer driver for the cross-call operator data cache: std::threads (no OpenMP
 * regions: libgomp is not instrumented) call the DSG interpolation operators (small grids: no
 * parallel region of the operator) and the quadratures with the option on, while other threads
 * clear and view the grids' caches; plus the cache class alone (get / clear / entries).
 */

#include <omp.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <memory>
#include <thread>
#include <vector>

#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_cache.hpp>
#include <sgpp_combigrid.hpp>

namespace cg = sgpp::combigrid;
using sgpp::base::DataVector;

double func(const DataVector& x) {
  double v = 1.0;
  for (size_t k = 0; k < x.size(); k++) v *= std::cos(0.7 * x[k]) + 0.1 * x[k];
  return v;
}

int main() {
  omp_set_num_threads(1);
  cg::CompleteSGGenInstr instr(4, 3);
  instr.setDomain({-0.5, 1.5});
  instr.setNodeGenFunc(cg::getEquidistantNodeGenFunc());
  instr.setLvl2GPCntFunc(cg::doublingLvl2GPCntFunction);
  instr.setBoundaryLevelOffset(1);
  const cg::DeltasSparseGrid dsg(instr);
  const cg::SparseGrid sg(instr);
  const cg::SourceFunc f(func);
  DataVector point(3);
  for (size_t k = 0; k < 3; k++) point[k] = -0.3 + 0.37 * static_cast<double>(k + 1);
  const double lin = cg::interpolateLinear(f, point, dsg);
  const double glob = cg::interpolate(f, point, dsg);
  const cg::QuadratureOptions quadOn{.useCrossCallOperatorDataCache = true};
  const double quad = cg::quadrature(f, dsg, quadOn);
  std::atomic<int> nWrong{0};

  std::vector<std::thread> threads;
  for (int t = 0; t < 8; t++) {
    threads.emplace_back([&, t]() {
      omp_set_num_threads(1);
      for (int i = 0; i < 300; i++) {
        switch ((t + i) % 6) {
          case 0:
            dsg.clearCrossCallOperatorDataCache();
            sg.clearCrossCallOperatorDataCache();
            break;
          case 1:
            if (dsg.getCrossCallOperatorDataCacheEntries().size() > 3) nWrong++;
            if (sg.getCrossCallOperatorDataCacheBytes() > (1u << 30)) nWrong++;
            break;
          case 2:
            if (cg::interpolateLinear(f, point, dsg) != lin) nWrong++;
            break;
          case 3:
            if (std::abs(cg::interpolate(f, point, dsg) - glob) > 1e-13) nWrong++;
            break;
          case 4:
            if (std::abs(cg::quadrature(f, dsg, quadOn) - quad) > 1e-13) nWrong++;
            break;
          default:
            cg::quadrature(f, sg, quadOn);
        }
      }
    });
  }
  for (auto& thread : threads) thread.join();

  // The cache class alone
  cg::misc::CrossCallOperatorDataCache cache;
  constexpr cg::misc::CrossCallOperatorDataDescriptor D{.operatorName = "op", .description = "d",
                                                        .key = ""};
  threads.clear();
  for (int t = 0; t < 8; t++) {
    threads.emplace_back([&, t]() {
      for (int i = 0; i < 20000; i++) {
        if ((t + i) % 50 == 0) {
          cache.clear();
        } else if ((t + i) % 13 == 0) {
          for (const auto& info : cache.entries()) {
            if (info.bytes != sizeof(std::vector<int>)) nWrong++;
          }
        } else {
          const auto v = cache.getOrBuild<std::vector<int>>(
              D, []() { return std::make_unique<std::vector<int>>(64, 5); });
          if ((*v)[63] != 5) nWrong++;
        }
      }
    });
  }
  for (auto& thread : threads) thread.join();

  std::printf("tsan driver: %s (%d wrong)\n", nWrong.load() == 0 ? "OK" : "FAILED", nWrong.load());
  return nWrong.load() == 0 ? 0 : 1;
}
