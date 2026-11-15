// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/// @name Constructors
/// @{

DeltasSparseGrid::DeltasSparseGrid(const SGGenInstr& genInstr)
    : SparseGridBase(genInstr.nDim()), genInstr(genInstr.clone()) {
  // Checked up front: the node lists are generated in an OpenMP parallel region, which the
  // exception thrown by the delta node generation for non-nested nodes cannot leave.
  if (!genInstr.areNodesOfAllDimsNested()) {
    throw std::invalid_argument(
        "DeltasSparseGrid: The nodes have to be nested in every dimension w.r.t. the "
        "level-to-grid-point-count function of that dimension.");
  }

  std::pair<LvlMIVec, std::vector<CTCoeffType>> mis = genInstr.genFullMIVecWithCoeffs();
  NodeTable unitNodeTable =
      tools::node_table_generation::genUnitNodeTable(genInstr, mis.first, true);
  NodeTable domainNodeTable = tools::node_table_generation::mapOntoDomain(genInstr, unitNodeTable);

  setStorage(std::move(mis.first), std::move(mis.second), std::move(domainNodeTable),
             std::move(unitNodeTable));
}

/// @}

/// @name Getters
/// @{

const SGGenInstr& DeltasSparseGrid::getGenInstr() const noexcept { return *genInstr; }

/// @}

}  // namespace combigrid
}  // namespace sgpp
