// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/// @name Constructors
/// @{

SparseGrid::SparseGrid(const SGGenInstr& genInstr)
    : SparseGridBase(genInstr.nDim()), genInstr(genInstr.clone()) {
  std::pair<LvlMIVec, std::vector<CTCoeffType>> mis = genInstr.genReducedMIVecWithCoeffs();
  NodeTable nodeTable = tools::node_table_generation::genNodeTable(genInstr, mis.first, false);

  setStorage(std::move(mis.first), std::move(mis.second), std::move(nodeTable));
}

/// @}

/// @name Getters
/// @{

const SGGenInstr& SparseGrid::getGenInstr() const noexcept { return *genInstr; }

/// @}

}  // namespace combigrid
}  // namespace sgpp
