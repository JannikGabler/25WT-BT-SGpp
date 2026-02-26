#include <algorithm>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {

namespace combigrid {

// TODO: Set default of lvl2GPCntFuncs
SGGenInstr::SGGenInstr(const size_t nDim)
    : boundaryIndexOffset(0),
      bounds(nDim, {0, 1}),
      nodeGenFuncs(nDim, nullptr),
      lvl2GPCntFuncs(nDim, nullptr) {}

size_t sgpp::combigrid::SGGenInstr::nDim() const { return this->bounds.size(); }

MIType SGGenInstr::getBoundaryIndexOffset() const { return boundaryIndexOffset; }

void SGGenInstr::setBoundaryIndexOffset(const MIType boundaryIndexOffset) {
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

void SGGenInstr::setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs, size_t dim) {
  const size_t nElemsToCopy = std::min(this->lvl2GPCntFuncs.size(), lvl2GPCntFuncs.size());
  std::copy_n(lvl2GPCntFuncs.begin(), nElemsToCopy, this->lvl2GPCntFuncs.begin());
}

void SGGenInstr::resize(const size_t newDimCnt) {
  bounds.resize(newDimCnt);
  nodeGenFuncs.resize(newDimCnt);
  lvl2GPCntFuncs.resize(newDimCnt);
}

std::vector<NodeGenFunc> SGGenInstr::getUniqueNodeGenFuncs() const {
  std::vector<NodeGenFunc> uniqueNodeGenFuncs;

  for (const NodeGenFunc candidate : uniqueNodeGenFuncs) {
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
