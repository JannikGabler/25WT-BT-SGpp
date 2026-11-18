// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file
 * @brief RP-6: evaluation balance of the DSG multilinear interpolation's evaluation pass. Runs the
 * operator's internal passes serially (index, context, d transforms) for the points of an e2e
 * case and counts the reached slots (= evaluations) per chunk of DSG_SUM_CHUNK_LEAVES leaves and
 * per leaf. The pass hands out chunks with schedule(dynamic, 1); the simulated makespan at T
 * threads (greedy, chunk order, cost = evaluations) is compared with max(total / T, largest
 * chunk) and with leaf granularity. Internal headers (analysis only, not a library change).
 */

#include <sgpp_combigrid.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>

#include <algorithm>
#include <cstdio>
#include <functional>
#include <queue>
#include <string>
#include <vector>

#include "perf_problems.hpp"

namespace perf = sgpp::combigrid::perf;
namespace dsg = sgpp::combigrid::tools::dsg_lin_interpolation;
using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::misc::DSGLinInterpContext;
using sgpp::combigrid::misc::DSGLinInterpSlotScratch;

namespace {

double greedy(const std::vector<size_t>& costs, size_t threads) {
  std::priority_queue<double, std::vector<double>, std::greater<>> h;
  for (size_t t = 0; t < threads; t++) h.push(0.0);
  for (const size_t c : costs) {
    const double t = h.top();
    h.pop();
    h.push(t + static_cast<double>(c));
  }
  double m = 0;
  while (!h.empty()) {
    m = h.top();
    h.pop();
  }
  return m;
}

void run(const std::string& name, const std::string& nodes, LvlType b, size_t d, LvlType L,
         size_t nPoints, bool shifted) {
  const size_t T = 16;
  const HyperCubeArea domain = shifted ? perf::shiftedDomain(d) : perf::unitDomain(d);
  CompleteSGGenInstr genInstr(L, d);
  perf::configure(genInstr, nodes, b, domain);
  const DeltasSparseGrid grid(genInstr);
  const auto indexPtr = dsg::getIndex(grid, false);
  const auto& index = *indexPtr;
  const size_t chunkLeaves = sgpp::combigrid::constants::lin_interpolation::DSG_SUM_CHUNK_LEAVES;
  double sumTotal = 0, sumBound = 0, sumChunk = 0, sumLeaf = 0;
  size_t maxChunk = 0;
  for (const DataVector& p : perf::randomPoints(domain, nPoints, 1000 + d)) {
    DSGLinInterpContext context(index);
    DataVector np(p);
    for (size_t j = 0; j < d; j++) {
      np[j] = (p[j] - domain[j].first) / (domain[j].second - domain[j].first);
    }
    dsg::prepareContext(index, np, domain, context);
    DSGLinInterpSlotScratch slots(index, 1);
    for (size_t dim = 0; dim < d; dim++) dsg::transformDimension(index, context, slots, dim, false);
    std::vector<size_t> perLeaf(index.nLeaves(), 0), perChunk;
    for (size_t leaf = 0; leaf < index.nLeaves(); leaf++) {
      for (size_t s = index.slotOffsets()[leaf]; s < index.slotOffsets()[leaf + 1]; s++) {
        perLeaf[leaf] += slots.reached[s] ? 1 : 0;
      }
      if (leaf % chunkLeaves == 0) perChunk.push_back(0);
      perChunk.back() += perLeaf[leaf];
    }
    size_t total = 0;
    for (const size_t c : perLeaf) total += c;
    const size_t big = *std::max_element(perChunk.begin(), perChunk.end());
    maxChunk = std::max(maxChunk, big);
    sumTotal += static_cast<double>(total);
    sumBound += static_cast<double>(total) / T;
    sumChunk += greedy(perChunk, T);
    sumLeaf += greedy(perLeaf, T);
  }
  std::printf("| %s | %zu | %zu | %.1f | %.1f | %zu | %.1f | %.1f | %.3f | %.3f |\n", name.c_str(),
              index.nLeaves(), (index.nLeaves() + chunkLeaves - 1) / chunkLeaves,
              sumTotal / nPoints, sumBound / nPoints, maxChunk, sumChunk / nPoints,
              sumLeaf / nPoints, sumBound / sumChunk, sumBound / sumLeaf);
}

}  // namespace

int main() {
  std::printf(
      "| case | leaves | chunks | evaluations / point | balanced / thread | largest chunk (max) "
      "| makespan chunks | makespan leaves | eff. chunks | eff. leaves |\n"
      "|---|---|---|---|---|---|---|---|---|---|\n");
  run("expensive/interplin/dsg/equi/b1/d5/L5/p16", "equi", 1, 5, 5, 16, false);
  run("expensive/interplin/dsg/equi/b1/d10/L4/p16", "equi", 1, 10, 4, 16, false);
  run("interplin/dsg/equi/b1/d5/L8/p1024 (64 pts)", "equi", 1, 5, 8, 64, true);
  run("interplin/dsg/equi/b1/d8/L5/p256 (64 pts)", "equi", 1, 8, 5, 64, true);
  run("interplin/dsg/equi/b1/d10/L5/p32", "equi", 1, 10, 5, 32, true);
  return 0;
}
