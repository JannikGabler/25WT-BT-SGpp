// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/projected_trie_node.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/numerics/double_double.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/projected_sub_trie_builder.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::CTCoeffTrie;
using sgpp::combigrid::misc::DeltaWeightedSumScratch;
using sgpp::combigrid::misc::DoubleDouble;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::misc::ProjectedTrieNode;

namespace sgpp {
namespace combigrid {

namespace tools {

ProjectedSubTrieBuilder::ProjectedSubTrieBuilder(const GenericDSGOperatorContext& context,
                                                 const LvlMI& deltaTGMI,
                                                 DeltaWeightedSumScratch& scratch)
    : context(context), deltaTGMI(deltaTGMI), scratch(scratch) {}

void ProjectedSubTrieBuilder::build() {
  for (std::vector<ProjectedTrieNode>& nodes : scratch.trieNodes) {
    nodes.clear();
  }

  if (!setProjDims()) {
    return;
  }

  LvlType maxLvlSum = 0;  // Of the tensor grids
  for (const CTCoeffTrie::Node& node : context.coeffTrie.nodesOfDepth(0)) {
    maxLvlSum = std::max(maxLvlSum, static_cast<LvlType>(node.lvl + node.maxSuffixLvlSum));
  }

  const LvlType lvlSum = deltaTGMI.sumOfElems();
  if (maxLvlSum < lvlSum) {
    return;  // No tensor grid >= deltaTGMI
  }
  slack = maxLvlSum - lvlSum;

  for (size_t projDepth = 0; projDepth + 1 < scratch.projDims.size(); projDepth++) {
    scratch.projChildBlocks[projDepth].clear();
  }
  scratch.projLeafSlots.clear();

  allocBlock(0, 0);  // The children of the root
  addChildren(0, 0, context.coeffTrie.nodesOfDepth(0).size(), lvlSum - deltaTGMI[0],
              Path{0, 0, 0, DoubleDouble{1.0, 0.0}});
  appendProjectedChildren(0, 0, 0);
}

size_t ProjectedSubTrieBuilder::deltaNodesStart(const Weights1DOfDim& weights1DOfDim,
                                                const LvlType deltaLvl) {
  return (deltaLvl == 0) ? 0 : weights1DOfDim[deltaLvl - 1].size();
}

bool ProjectedSubTrieBuilder::setProjDims() {
  const size_t nDim = context.coeffTrie.nDim();
  scratch.projDims.clear();

  for (size_t dim = 0; dim < nDim; dim++) {
    const Weights1DOfDim weights1DOfDim = context.weights1D[dim];
    const LvlType deltaLvl = deltaTGMI[dim];

    if (deltaLvl >= weights1DOfDim.size()) {  // Above the maximum level of the dimension
      scratch.projDims.clear();
      return false;
    }

    if (weights1DOfDim[deltaLvl].size() - deltaNodesStart(weights1DOfDim, deltaLvl) > 1) {
      scratch.projDims.push_back(dim);
    }
  }

  if (nDim > 0 && scratch.projDims.empty()) {  // A single grid point: rows along dimension 0
    scratch.projDims.push_back(0);
  }

  return nDim > 0;
}

// TODO: Functional decomposition (?) if feasible without performance loss
void ProjectedSubTrieBuilder::addChildren(const size_t dim, const size_t childBegin,
                                          const size_t childEnd, const LvlType lvlSumAfterDim,
                                          const Path& path) {
  const CTCoeffTrie& coeffTrie = context.coeffTrie;
  const std::span<const CTCoeffTrie::Node> nodesOfDim = coeffTrie.nodesOfDepth(dim);
  const std::span<const CTCoeffTrie::Node> children =
      nodesOfDim.subspan(childBegin, childEnd - childBegin);
  const bool isLeafDepth = (dim + 1 == coeffTrie.nDim());
  const double* const leafCoeffs = coeffTrie.leafCoeffs().data();

  // The children are sorted by level, so the ones with a level >= deltaTGMI[dim] are a suffix
  const LvlType deltaLvl = deltaTGMI[dim];
  const auto firstChild = std::ranges::lower_bound(children, deltaLvl, {}, &CTCoeffTrie::Node::lvl);

  const bool isProjDim =
      (path.nextProjDepth < scratch.projDims.size() && scratch.projDims[path.nextProjDepth] == dim);
  const size_t block = isProjDim ? childBlock(path) : 0;
  const Weights1DOfDim weights1DOfDim = context.weights1D[dim];
  const size_t weightIdx = deltaNodesStart(weights1DOfDim, deltaLvl);  // The single delta node

  for (auto child = firstChild; child != children.end(); ++child) {
    if (child->maxSuffixLvlSum < lvlSumAfterDim) {  // No tensor grid >= deltaTGMI below the child
      continue;
    }

    const size_t childIdx = childBegin + static_cast<size_t>(child - children.begin());
    const LvlType excess = child->lvl - deltaLvl;
    const Path childPath =
        isProjDim ? Path{path.nextProjDepth + 1, block + excess,
                         static_cast<LvlType>(path.projExcess + excess), path.weightProduct}
                  : Path{path.nextProjDepth, path.parentSlot, path.projExcess,
                         path.weightProduct * weights1DOfDim[child->lvl][weightIdx]};
    assert(!isProjDim || excess < blockWidth(path.nextProjDepth, path.projExcess));

    if (isLeafDepth) {  // childPath.parentSlot is a slot of the last projected depth
      assert(childPath.nextProjDepth == scratch.projDims.size());
      scratch.projLeafSlots[childPath.parentSlot] += childPath.weightProduct * leafCoeffs[childIdx];
    } else {
      // = coeffTrie.childBegin(dim, childIdx), from the local span
      const size_t grandChildBegin = (childIdx == 0) ? 0 : nodesOfDim[childIdx - 1].childEnd;
      addChildren(dim + 1, grandChildBegin, child->childEnd, lvlSumAfterDim - deltaTGMI[dim + 1],
                  childPath);
    }
  }
}

size_t ProjectedSubTrieBuilder::childBlock(const Path& path) {
  if (path.nextProjDepth == 0) {
    return 0;  // Allocated by build
  }

  // Not invalidated by allocBlock, which appends to the slots of the next projected depth
  size_t& block = scratch.projChildBlocks[path.nextProjDepth - 1][path.parentSlot];

  if (block == DeltaWeightedSumScratch::NO_BLOCK) {
    block = allocBlock(path.nextProjDepth, path.projExcess);
  }

  return block;
}

size_t ProjectedSubTrieBuilder::allocBlock(const size_t projDepth, const LvlType projExcess) {
  const size_t width = blockWidth(projDepth, projExcess);

  if (projDepth + 1 == scratch.projDims.size()) {
    const size_t start = scratch.projLeafSlots.size();
    scratch.projLeafSlots.resize(start + width, DoubleDouble{0.0, 0.0});
    return start;
  }

  std::vector<size_t>& slots = scratch.projChildBlocks[projDepth];
  const size_t start = slots.size();
  slots.resize(start + width, DeltaWeightedSumScratch::NO_BLOCK);
  return start;
}

size_t ProjectedSubTrieBuilder::blockWidth(const size_t projDepth, const LvlType projExcess) const {
  const size_t dim = scratch.projDims[projDepth];
  assert(projExcess <= slack);
  return std::min<size_t>(context.weights1D[dim].size() - 1 - deltaTGMI[dim], slack - projExcess) +
         1;
}

bool ProjectedSubTrieBuilder::appendProjectedChildren(const size_t projDepth, const size_t block,
                                                      const LvlType projExcess) {
  const size_t dim = scratch.projDims[projDepth];
  const LvlType deltaLvl = deltaTGMI[dim];
  const Weights1DOfDim weights1DOfDim = context.weights1D[dim];
  const size_t weightsStart = deltaNodesStart(weights1DOfDim, deltaLvl);
  const size_t width = blockWidth(projDepth, projExcess);
  const bool isLeafDepth = (projDepth + 1 == scratch.projDims.size());
  std::vector<ProjectedTrieNode>& nodes = scratch.trieNodes[projDepth];
  bool appended = false;

  for (LvlType excess = 0; excess < width; excess++) {
    const double* const weights = weights1DOfDim[deltaLvl + excess].data() + weightsStart;

    if (isLeafDepth) {
      const double coeff = scratch.projLeafSlots[block + excess].value();

      if (coeff != 0.0) {
        nodes.push_back({weights, 0, coeff});
        appended = true;
      }
    } else if (const size_t childBlock = scratch.projChildBlocks[projDepth][block + excess];
               childBlock != DeltaWeightedSumScratch::NO_BLOCK) {
      nodes.push_back({weights, 0, 0.0});

      if (appendProjectedChildren(projDepth + 1, childBlock,
                                  static_cast<LvlType>(projExcess + excess))) {
        nodes.back().childEnd = scratch.trieNodes[projDepth + 1].size();
        appended = true;
      } else {  // No non-zero projected coefficient below the slot
        nodes.pop_back();
      }
    }
  }

  return appended;
}

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
