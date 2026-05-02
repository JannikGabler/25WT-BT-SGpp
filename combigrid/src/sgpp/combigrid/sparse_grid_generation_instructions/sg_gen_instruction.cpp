#include <algorithm>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/***********
Constructors
***********/

SGGenInstr::SGGenInstr(const size_t nDim)
    : boundaryLevelOffset(0),
      domain(nDim, {0, 1}),
      nodeGenFuncs(nDim, getEquidistantNodeGenFunc()),
      lvl2GPCntFuncs(nDim, doublingLvl2GPCntFunction) {}

/******
Getters
******/

size_t SGGenInstr::nDim() const { return this->domain.size(); }

LvlType SGGenInstr::getBoundaryLevelOffset() const { return boundaryLevelOffset; }

const HyperCubeArea& SGGenInstr::getDomain() const { return domain; }

std::pair<double, double> SGGenInstr::getDomainForDim(const size_t dim) const {
  return domain[dim];
}

const std::vector<NodeGenFunc*>& SGGenInstr::getNodeGenFuncs() const { return nodeGenFuncs; }

NodeGenFunc* SGGenInstr::getNodeGenFuncForDim(const size_t dim) const { return nodeGenFuncs[dim]; }

const std::vector<Lvl2GPCntFunc>& SGGenInstr::getLvl2GPCntFuncs() const { return lvl2GPCntFuncs; }

Lvl2GPCntFunc SGGenInstr::getLvl2GPCntFuncForDim(const size_t dim) const {
  return lvl2GPCntFuncs[dim];
}

/******
Setters
******/

void SGGenInstr::setBoundaryLevelOffset(const LvlType boundaryLevelOffset) {
  this->boundaryLevelOffset = boundaryLevelOffset;
}

void SGGenInstr::setDomainForDim(const std::pair<double, double> interval, const size_t dim) {
  domain[dim] = interval;
}

void SGGenInstr::setDomain(const std::pair<double, double> interval) {
  std::fill(domain.begin(), domain.end(), interval);
}

void SGGenInstr::setDomain(const std::vector<std::pair<double, double>>& domain) {
  const size_t nElemsToCopy = std::min(this->domain.size(), domain.size());
  std::copy_n(domain.begin(), nElemsToCopy, this->domain.begin());
}

void SGGenInstr::setNodeGenFuncForDim(NodeGenFunc* const nodeGenFunc, const size_t dim) {
  nodeGenFuncs[dim] = nodeGenFunc;
}

void SGGenInstr::setNodeGenFunc(NodeGenFunc* const nodeGenFunc) {
  std::fill(nodeGenFuncs.begin(), nodeGenFuncs.end(), nodeGenFunc);
}

void SGGenInstr::setNodeGenFuncs(const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  const size_t nElemsToCopy = std::min(this->nodeGenFuncs.size(), nodeGenFuncs.size());
  std::copy_n(nodeGenFuncs.begin(), nElemsToCopy, this->nodeGenFuncs.begin());
}

void SGGenInstr::setLvl2GPCntFuncForDim(const Lvl2GPCntFunc lvl2GPCntFunc, const size_t dim) {
  this->lvl2GPCntFuncs.at(dim) = lvl2GPCntFunc;
}

void SGGenInstr::setLvl2GPCntFunc(const Lvl2GPCntFunc lvl2GPCntFunc) {
  std::fill(lvl2GPCntFuncs.begin(), lvl2GPCntFuncs.end(), lvl2GPCntFunc);
}

void SGGenInstr::setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs) {
  const size_t nElemsToCopy = std::min(this->lvl2GPCntFuncs.size(), lvl2GPCntFuncs.size());
  std::copy_n(lvl2GPCntFuncs.begin(), nElemsToCopy, this->lvl2GPCntFuncs.begin());
}

/****************
Helper operations
****************/
void SGGenInstr::resize(const size_t newDimCnt) {
  domain.resize(newDimCnt);
  nodeGenFuncs.resize(newDimCnt);
  lvl2GPCntFuncs.resize(newDimCnt);
}

double SGGenInstr::getVolumeOfDomain() const {
  if (nDim() == 0) {
    return 0;
  }

  double volume = 1;
  for (size_t dim = 0; dim < nDim(); dim++) {
    volume *= domain[dim].second - domain[dim].first;
  }

  return volume;
}

std::vector<NodeGenFunc*> SGGenInstr::getUniqueNodeGenFuncs() const {
  std::vector<NodeGenFunc*> uniqueNodeGenFuncs;

  for (NodeGenFunc* const candidate : nodeGenFuncs) {
    bool unique = true;

    for (NodeGenFunc* const nodeGenFunc : uniqueNodeGenFuncs) {
      // There should be only one object for each unique NodeGenFunc
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
