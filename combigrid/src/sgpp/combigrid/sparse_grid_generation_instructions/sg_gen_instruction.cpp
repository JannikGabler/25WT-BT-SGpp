#include <algorithm>
#include <sgpp/combigrid/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/***********
Constructors
***********/

// TODO: Set default of lvl2GPCntFuncs
SGGenInstr::SGGenInstr(const size_t nDim)
    : boundaryIndexOffset(0),
      bounds(nDim, {0, 1}),
      nodeGenFuncs(nDim, genEquidistantNodes),
      lvl2GPCntFuncs(nDim, doublingLvl2GPCntFunction) {}

/******
Getters
******/

size_t SGGenInstr::nDim() const { return this->bounds.size(); }

LvlType SGGenInstr::getBoundaryIndexOffset() const { return boundaryIndexOffset; }

const std::vector<NodeGenFunc>& SGGenInstr::getNodeGenFuncs() const { return nodeGenFuncs; }

NodeGenFunc SGGenInstr::getNodeGenFuncForDim(const size_t dim) const { return nodeGenFuncs[dim]; }

const std::vector<Lvl2GPCntFunc>& SGGenInstr::getLvl2GPCntFuncs() const { return lvl2GPCntFuncs; }

Lvl2GPCntFunc SGGenInstr::getLvl2GPCntFuncForDim(const size_t dim) const {
  return lvl2GPCntFuncs[dim];
}

/******
Setters
******/

void SGGenInstr::setBoundaryIndexOffset(const LvlType boundaryIndexOffset) {
  this->boundaryIndexOffset = boundaryIndexOffset;
}

void SGGenInstr::setBoundForDim(const std::pair<double, double> bound, const size_t dim) {
  bounds[dim] = bound;
}

void SGGenInstr::setBounds(const std::vector<std::pair<double, double>>& bounds) {
  const size_t nElemsToCopy = std::min(this->bounds.size(), bounds.size());
  std::copy_n(bounds.begin(), nElemsToCopy, this->bounds.begin());
}

void SGGenInstr::setNodeGenFuncForDim(const NodeGenFunc nodeGenFunc, const size_t dim) {
  this->nodeGenFuncs.at(dim) = nodeGenFunc;
}

void SGGenInstr::setNodeGenFuncs(const std::vector<NodeGenFunc>& nodeGenFuncs) {
  const size_t nElemsToCopy = std::min(this->nodeGenFuncs.size(), nodeGenFuncs.size());
  std::copy_n(nodeGenFuncs.begin(), nElemsToCopy, this->nodeGenFuncs.begin());
}

void SGGenInstr::setLvl2GPCntFuncForDim(const Lvl2GPCntFunc lvl2GPCntFunc, const size_t dim) {
  this->lvl2GPCntFuncs.at(dim) = lvl2GPCntFunc;
}

void SGGenInstr::setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs) {
  const size_t nElemsToCopy = std::min(this->lvl2GPCntFuncs.size(), lvl2GPCntFuncs.size());
  std::copy_n(lvl2GPCntFuncs.begin(), nElemsToCopy, this->lvl2GPCntFuncs.begin());
}

/****************
Helper operations
****************/
void SGGenInstr::resize(const size_t newDimCnt) {
  bounds.resize(newDimCnt);
  nodeGenFuncs.resize(newDimCnt);
  lvl2GPCntFuncs.resize(newDimCnt);
}

std::vector<NodeGenFunc> SGGenInstr::getUniqueNodeGenFuncs() const {
  std::vector<NodeGenFunc> uniqueNodeGenFuncs;

  for (const NodeGenFunc candidate : nodeGenFuncs) {
    bool unique = true;

    for (const NodeGenFunc nodeGenFunc : uniqueNodeGenFuncs) {
      if (candidate == nodeGenFunc) {
        unique = false;
        break;
      }
    }

    if (unique) {
      uniqueNodeGenFuncs.push_back(candidate);
    }
  }

  return uniqueNodeGenFuncs;
}

}  // namespace combigrid
}  // namespace sgpp
