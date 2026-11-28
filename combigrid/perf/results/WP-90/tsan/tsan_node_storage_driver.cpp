// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tsan_node_storage_driver.cpp
 * @brief ThreadSanitizer driver of the shared node storage (WP-90 P3): 8 std::threads build and
 * destroy sparse grids with overlapping node lists on one storage (rows requested, generated,
 * waited for and freed concurrently), read their nodes and query the storage's statistics; one
 * thread builds a grid large enough for the parallel (OpenMP) row generation.
 */

#include <omp.h>

#include <cstddef>
#include <cstdio>
#include <memory>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_storage.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <string>
#include <thread>
#include <vector>

namespace cg = sgpp::combigrid;

int main(int argc, char** argv) {
  // Argument "serial" (with OMP_NUM_THREADS=1, see tsan.sh): no libgomp workers
  omp_set_num_threads(argc > 1 && std::string(argv[1]) == "serial" ? 1 : 4);
  const auto storage = std::make_shared<cg::NodeStorage>();
  double checksum = 0.0;
  std::vector<double> sums(8, 0.0);
  std::vector<std::thread> threads;
  for (size_t t = 0; t < 8; t++) {
    threads.emplace_back([&, t]() {
      for (size_t it = 0; it < 40; it++) {
        const size_t variant = (t + it) % 4;
        cg::CompleteSGGenInstr genInstr(static_cast<cg::LvlType>(2 + (it % 5)), 3);
        genInstr.setNodeFamily(variant % 2 == 0 ? cg::getClenshawCurtisNodes()
                                                : cg::getEquidistantNodes());
        genInstr.setLvl2GPCntFunc(cg::doublingLvl2GPCntFunction);
        genInstr.setBoundaryLevelOffset(variant / 2);
        if (it % 3 == 0) genInstr.setDomain({-1.0, 2.0});
        genInstr.setNodeStorage(storage);
        const cg::SparseGrid sg(genInstr);
        const cg::DeltasSparseGrid dsg(genInstr);
        sums[t] += sg.getDomainNodeTable().nodes(1, 1)[0] + dsg.getUnitNodeTable().nodes(0, 2)[0];
        sums[t] += static_cast<double>(storage->nRows() % 2);
      }
    });
  }
  threads.emplace_back([&]() {  // Large rows: parallel generation
    for (size_t it = 0; it < 2; it++) {
      cg::CompleteSGGenInstr genInstr(16, 2);
      genInstr.setNodeFamily(cg::getClenshawCurtisNodes());
      genInstr.setNodeStorage(storage);
      const cg::SparseGrid sg(genInstr);
      checksum += sg.getUnitNodeTable().nodes(0, 16)[3];
    }
  });
  for (std::thread& thread : threads) thread.join();
  for (const double sum : sums) checksum += sum;
  std::printf("checksum %.17g, rows left %zu\n", checksum, storage->nRows());
  return storage->nRows() == 0 ? 0 : 1;
}
