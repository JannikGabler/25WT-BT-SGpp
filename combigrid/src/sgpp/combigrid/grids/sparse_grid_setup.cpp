// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

SparseGridSetup::SparseGridSetup(const size_t nDim)
    : boundaryLevelOffset(0),
      domain(nDim, {0, 1}),
      nodeFamilies(nDim, getEquidistantNodes()),
      lvl2GPCntFuncs(nDim, doublingLvl2GPCntFunction) {}

size_t SparseGridSetup::nDim() const { return domain.size(); }

LvlType SparseGridSetup::getBoundaryLevelOffset() const { return boundaryLevelOffset; }

const HyperCubeArea& SparseGridSetup::getDomain() const { return domain; }

std::pair<double, double> SparseGridSetup::getDomainForDim(const size_t dim) const {
  return domain[dim];
}

const std::vector<NodeFamily*>& SparseGridSetup::getNodeFamilies() const { return nodeFamilies; }

NodeFamily* SparseGridSetup::getNodeFamilyForDim(const size_t dim) const {
  return nodeFamilies[dim];
}

const std::vector<Lvl2GPCntFunc>& SparseGridSetup::getLvl2GPCntFuncs() const {
  return lvl2GPCntFuncs;
}

Lvl2GPCntFunc SparseGridSetup::getLvl2GPCntFuncForDim(const size_t dim) const {
  return lvl2GPCntFuncs[dim];
}

void SparseGridSetup::setBoundaryLevelOffset(const LvlType boundaryLevelOffset) {
  this->boundaryLevelOffset = boundaryLevelOffset;
}

void SparseGridSetup::setDomainForDim(const std::pair<double, double> interval, const size_t dim) {
  domain[dim] = interval;
}

void SparseGridSetup::setDomain(const std::pair<double, double> interval) {
  std::fill(domain.begin(), domain.end(), interval);
}

void SparseGridSetup::setDomain(const HyperCubeArea& domain) {
  const size_t nElemsToCopy = std::min(this->domain.size(), domain.size());
  std::copy_n(domain.begin(), nElemsToCopy, this->domain.begin());
}

void SparseGridSetup::setNodeFamilyForDim(NodeFamily* const nodeFamily, const size_t dim) {
  nodeFamilies[dim] = nodeFamily;
}

void SparseGridSetup::setNodeFamily(NodeFamily* const nodeFamily) {
  std::fill(nodeFamilies.begin(), nodeFamilies.end(), nodeFamily);
}

void SparseGridSetup::setNodeFamilies(const std::vector<NodeFamily*>& nodeFamilies) {
  const size_t nElemsToCopy = std::min(this->nodeFamilies.size(), nodeFamilies.size());
  std::copy_n(nodeFamilies.begin(), nElemsToCopy, this->nodeFamilies.begin());
}

void SparseGridSetup::setLvl2GPCntFuncForDim(const Lvl2GPCntFunc lvl2GPCntFunc, const size_t dim) {
  lvl2GPCntFuncs.at(dim) = lvl2GPCntFunc;
}

void SparseGridSetup::setLvl2GPCntFunc(const Lvl2GPCntFunc lvl2GPCntFunc) {
  std::fill(lvl2GPCntFuncs.begin(), lvl2GPCntFuncs.end(), lvl2GPCntFunc);
}

void SparseGridSetup::setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs) {
  const size_t nElemsToCopy = std::min(this->lvl2GPCntFuncs.size(), lvl2GPCntFuncs.size());
  std::copy_n(lvl2GPCntFuncs.begin(), nElemsToCopy, this->lvl2GPCntFuncs.begin());
}

void SparseGridSetup::resize(const size_t newDimCnt) {
  domain.resize(newDimCnt);
  nodeFamilies.resize(newDimCnt);
  lvl2GPCntFuncs.resize(newDimCnt);
}

double SparseGridSetup::getVolumeOfDomain() const {
  if (nDim() == 0) {
    return 0;
  }

  double volume = 1;
  for (const auto& [min, max] : domain) {
    volume *= max - min;
  }

  return volume;
}

std::vector<NodeFamily*> SparseGridSetup::getUniqueNodeFamilies() const {
  std::vector<NodeFamily*> uniqueNodeFamilies;

  // There is only one object per node family (singletons): pointer identity suffices
  for (NodeFamily* const candidate : nodeFamilies) {
    if (std::ranges::find(uniqueNodeFamilies, candidate) == uniqueNodeFamilies.end()) {
      uniqueNodeFamilies.push_back(candidate);
    }
  }

  return uniqueNodeFamilies;
}

bool SparseGridSetup::areNodesOfAllDimsNested() const {
  for (size_t dim = 0; dim < nDim(); dim++) {
    if (!nodeFamilies[dim]->isNested(lvl2GPCntFuncs[dim])) {
      return false;
    }
  }

  return true;
}

}  // namespace combigrid
}  // namespace sgpp
