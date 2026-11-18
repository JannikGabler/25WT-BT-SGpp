// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file
 * @brief RP-6: tensor-grid sizes (grid points per TG, in TG order) of the e2e grids whose TG loop
 * balance is examined (mivec / quad, SG and DSG; DSG = points of each delta TG = evaluations).
 * Output: one JSON object per line {"case", "nGP": [...], "coef": [...]}. (SG multilinear
 * interpolation evaluates 2^d corners per TG and point: uniform cost, not modelled by nGP.) Public API only, built
 * against a snapshot's libraries (build_tg_sizes.sh).
 */

#include <sgpp_combigrid.hpp>

#include <cstdio>
#include <optional>
#include <string>

#include "perf_problems.hpp"

namespace perf = sgpp::combigrid::perf;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::SparseGrid;

template <typename Grid>
void dump(const std::string& name, const Grid& grid) {
  std::printf("{\"case\": \"%s\", \"nGP\": [", name.c_str());
  for (size_t i = 0; i < grid.nTG(); i++) {
    std::printf("%s%zu", i ? ", " : "", grid.getDomainTensorGrid(i).tensorGrid.nGP());
  }
  std::printf("], \"coef\": [");
  for (size_t i = 0; i < grid.nTG(); i++) {
    std::printf("%s%lld", i ? ", " : "",
                static_cast<long long>(grid.getDomainTensorGrid(i).coefficient));
  }
  std::printf("]}\n");
}

template <typename Grid>
void mivec(const std::string& name, size_t nDim, LvlType maxSum, size_t nMI) {
  const LvlMIVec mis = perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim);
  MIVecSGGenInstr genInstr(mis);
  perf::configure(genInstr, "cc", 1, perf::unitDomain(nDim));
  const Grid grid(genInstr);
  dump(name, grid);
}

template <typename Grid>
void complete(const std::string& name, const std::string& nodes, LvlType boundary, size_t nDim,
              LvlType maxLvl) {
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
  const Grid grid(genInstr);
  dump(name, grid);
}

int main() {
  mivec<SparseGrid>("expensive/mivec/sg/d15/S6/n300", 15, 6, 300);
  mivec<DeltasSparseGrid>("expensive/mivec/dsg/d15/S6/n300", 15, 6, 300);
  mivec<SparseGrid>("expensive/mivec/sg/d20/S10/n300", 20, 10, 300);
  mivec<DeltasSparseGrid>("expensive/mivec/dsg/d20/S10/n300", 20, 10, 300);
  mivec<SparseGrid>("mivec/sg/d5/S16/n1000", 5, 16, 1000);
  mivec<SparseGrid>("mivec/sg/d10/S12/n1000", 10, 12, 1000);
  mivec<SparseGrid>("mivec/sg/d15/S10/n300", 15, 10, 300);
  mivec<SparseGrid>("mivec/sg/d20/S7/n300", 20, 7, 300);
  complete<SparseGrid>("expensive/quad/sg/cc/b1/d10/L4", "cc", 1, 10, 4);
  complete<DeltasSparseGrid>("expensive/quad/dsg/cc/b1/d10/L4", "cc", 1, 10, 4);
  complete<SparseGrid>("expensive/quad/sg/cc/b1/d20/L3", "cc", 1, 20, 3);
  complete<SparseGrid>("expensive/quad/sg/cc/b1/d15/L5", "cc", 1, 15, 5);
  complete<DeltasSparseGrid>("expensive/quad/dsg/cc/b1/d15/L5", "cc", 1, 15, 5);
  complete<SparseGrid>("quad/sg/equi/b1/d10/L7", "equi", 1, 10, 7);
  complete<SparseGrid>("quad/sg/equi/b1/d15/L6", "equi", 1, 15, 6);
  complete<SparseGrid>("quad/sg/equi/b1/d20/L5", "equi", 1, 20, 5);
  complete<SparseGrid>("quad/sg/equi/b0/d10/L4", "equi", 0, 10, 4);
  complete<SparseGrid>("quad/sg/equi/b0/d7/L7", "equi", 0, 7, 7);
  complete<SparseGrid>("quad/sg/equi/b0/d5/L10", "equi", 0, 5, 10);
  complete<SparseGrid>("quad/sg/cc/b0/d10/L4", "cc", 0, 10, 4);
  complete<SparseGrid>("interplin/sg/equi/b1/d5/L5", "equi", 1, 5, 5);
  complete<SparseGrid>("interplin/sg/equi/b1/d10/L5", "equi", 1, 10, 5);
  complete<SparseGrid>("interp/sg/cc/b1/d10/L3", "cc", 1, 10, 3);
  complete<SparseGrid>("interp/sg/cc/b0/d10/L2", "cc", 0, 10, 2);
  complete<SparseGrid>("interp/sg/cc/b0/d7/L4", "cc", 0, 7, 4);
  complete<SparseGrid>("interp/sg/cc/b0/d5/L6", "cc", 0, 5, 6);
  complete<SparseGrid>("interp/sg/cc/b0/d3/L9", "cc", 0, 3, 9);
  complete<SparseGrid>("interp/sg/cc/b0/d2/L12", "cc", 0, 2, 12);
  complete<SparseGrid>("interp/sg/cheb1/b0/d5/L4", "cheb1", 0, 5, 4);
  complete<SparseGrid>("interp/sg/cc/b1/d7/L3", "cc", 1, 7, 3);
  complete<SparseGrid>("expensive/interp/sg/cc/b1/d10/L4", "cc", 1, 10, 4);
  complete<SparseGrid>("quad/sg/cc/b0/d2/L20", "cc", 0, 2, 20);
  complete<SparseGrid>("quad/sg/equi/b0/d2/L20", "equi", 0, 2, 20);
  complete<DeltasSparseGrid>("quad/dsg/equi/b0/d3/L16", "equi", 0, 3, 16);
  complete<DeltasSparseGrid>("quad/dsg/equi/b0/d5/L12", "equi", 0, 5, 12);
  complete<DeltasSparseGrid>("quad/dsg/equi/b0/d7/L10", "equi", 0, 7, 10);
  complete<DeltasSparseGrid>("quad/dsg/equi/b0/d10/L7", "equi", 0, 10, 7);
  complete<DeltasSparseGrid>("quad/dsg/equi/b1/d15/L7", "equi", 1, 15, 7);
  complete<DeltasSparseGrid>("quad/dsg/equi/b1/d20/L6", "equi", 1, 20, 6);
  complete<DeltasSparseGrid>("quad/dsg/cc/b0/d10/L7", "cc", 0, 10, 7);
  return 0;
}
