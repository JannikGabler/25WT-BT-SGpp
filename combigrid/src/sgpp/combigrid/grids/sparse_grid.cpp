// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/// @name Constructors
/// @{

SparseGrid::SparseGrid(const SGGenInstr& genInstr)
    : SparseGridBase(static_cast<const SparseGridSetup&>(genInstr)) {
  std::pair<LvlMIVec, std::vector<CTCoeffType>> mis = genInstr.genReducedMIVecWithCoeffs();
  std::pair<NodeTable, NodeTable> nodeTables =
      tools::node_table_generation::genNodeTables(genInstr, mis.first, false);

  setStorage(std::move(mis.first), std::move(mis.second), std::move(nodeTables.second),
             std::move(nodeTables.first));
}

/// @}

/// @name Getters
/// @{

/// @}

}  // namespace combigrid
}  // namespace sgpp
