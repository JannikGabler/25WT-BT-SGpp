// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <tuple>
#include <utility>

using sgpp::combigrid::misc::DSGLinInterpContext;
using sgpp::combigrid::misc::DSGLinInterpIndex;
using sgpp::combigrid::misc::DSGLinInterpScratch;
using sgpp::combigrid::misc::DSGLinInterpSlotScratch;

namespace sgpp {
namespace combigrid {
namespace tools {
namespace dsg_lin_interpolation {

namespace {

/// Lowers @p below to the largest value of @p nodes (sorted) less than @p x and @p atOrAbove to
/// the smallest one at least @p x (where smaller resp. larger): with these updates over the delta
/// nodes of the levels 0 ... l, they are the neighbors of @p x in the nodes of level l. Returns
/// the index of @p atOrAbove's candidate in @p nodes (searched from @p hint).
size_t updateNeighbors(const std::span<const double> nodes, const double x, const size_t hint,
                       double& below, double& atOrAbove) {
  const size_t pos = lowerBoundFrom(nodes, x, hint);
  if (pos != nodes.size()) {
    atOrAbove = std::min(atOrAbove, nodes[pos]);
  }
  if (pos != 0) {
    below = std::max(below, nodes[pos - 1]);
  }
  return pos;
}

/// Sets the coordinates of the first slot of the block of @p leaf in @c scratch.gp and collects
/// the dimensions that contribute a bit to the sub-index; returns their number.
size_t prepareBlock(const DSGLinInterpIndex& index, const DSGLinInterpContext& context,
                    const size_t leaf, DSGLinInterpScratch& scratch) {
  const LvlMIView lambda = index.leafMI(leaf);
  size_t nMulti = 0;
  for (size_t j = 0; j < index.nDim(); j++) {
    const size_t r = index.row(j, lambda[j]);
    scratch.gp[j] = context.coords[2 * r];
    if (index.maxNewCoords(r) == 2) {
      scratch.multiDims()[nMulti] = j;
      scratch.multiRows()[nMulti] = r;
      nMulti++;
    }
  }
  return nMulti;
}

/// Sets the corner ids of the bracket @p bracket of level @p lvl (row @p r): a coordinate of the
/// previous level's bracket @p prev keeps its id, a new one gets the next id of @p lvl and its
/// coordinate on the domain.
void assignCornerIds(const DSGLinInterpIndex& index, const LvlType lvl, const size_t r,
                     const std::pair<double, double> bracket,
                     const std::pair<double, double> domain, const std::array<double, 2>& prev,
                     const std::array<uint32_t, 2>& prevIds, DSGLinInterpContext& context) {
  const bool twoNew = (index.maxNewCoords(r) == 2);
  uint32_t nNew = 0;
  const auto idOf = [&](const double coord) {
    if (coord == prev[0]) return prevIds[0];
    if (coord == prev[1]) return prevIds[1];
    context.coords[2 * r + nNew] =
        tools::mapToDomain(coord, domain.first, domain.second - domain.first);
    return DSGLinInterpContext::encodeId(lvl, twoNew, nNew++);
  };

  context.cornerIds[2 * r] = idOf(bracket.first);
  context.cornerIds[2 * r + 1] =
      (bracket.second == bracket.first) ? context.cornerIds[2 * r] : idOf(bracket.second);
  assert(nNew <= index.maxNewCoords(r));
}

/// Returns the first slot of the id @p id along a fiber with the blocks @p blocks: its block is
/// that of its first level, the slots of s = 1 follow those of s = 0.
inline size_t slotOfId(const std::span<const size_t> blocks, const uint32_t id,
                       const size_t nInner) {
  return blocks[DSGLinInterpContext::idLvl(id)] + DSGLinInterpContext::idSub(id) * nInner;
}

/// Adds @p value times the 1D weights of row @p r to the two ids that bracket the query
/// coordinate at the row's level (from slot @p o of their blocks along a fiber with @p blocks),
/// and @p reach to their flags.
inline void pushToIds(const DSGLinInterpContext& context, const size_t r,
                      const std::span<const size_t> blocks, const size_t nInner, const size_t o,
                      const double value, const bool reach, double* const weights,
                      bool* const reached) {
  const size_t lowerDst = slotOfId(blocks, context.cornerIds[2 * r], nInner) + o;
  const size_t upperDst = slotOfId(blocks, context.cornerIds[2 * r + 1], nInner) + o;
  weights[lowerDst] += context.factors[2 * r] * value;
  reached[lowerDst] |= reach;
  weights[upperDst] += context.factors[2 * r + 1] * value;
  reached[upperDst] |= reach;
}

/**
 * Transform of dimension 0 along @p fiber: its inputs are the coefficients of its leaves, which
 * are consecutive, so the blocks of the fiber are one range of slots. The range is set to zero and
 * only the nonzero coefficients are added (a zero input would add zeros), in ascending order of
 * the level.
 */
void transformDim0Fiber(const DSGLinInterpIndex& index, const DSGLinInterpContext& context,
                        DSGLinInterpSlotScratch& slots, const size_t fiber) {
  const std::span<const size_t> blocks = index.fiberBlocks(fiber);
  const size_t firstLeaf = index.firstLeafOfDim0Fiber(fiber);
  const size_t begin = blocks.front();
  const size_t end = index.slotOffsets()[firstLeaf + blocks.size()];
  double* const weights = slots.slotWeights.data();
  bool* const reached = slots.reached.get();
  std::fill(weights + begin, weights + end, 0.0);
  std::fill(reached + begin, reached + end, false);

  for (size_t lvl = 0; lvl < blocks.size(); lvl++) {
    const double coefficient = index.leafCoefficient(firstLeaf + lvl);
    if (coefficient != 0.0) {
      pushToIds(context, index.row(0, static_cast<LvlType>(lvl)), blocks, 1, 0, coefficient, true,
                weights, reached);
    }
  }
}

}  // namespace

const DSGLinInterpIndex& getIndex(const DeltasSparseGrid& deltasSparseGrid) {
  return deltasSparseGrid.getOperatorData().get<DSGLinInterpIndex>(
      [&]() { return std::make_unique<DSGLinInterpIndex>(deltasSparseGrid); });
}

bool runInParallel(const DSGLinInterpIndex& index) {
  const size_t nDim = index.nDim();
  const size_t nTG = index.combinationTGs().size();
  const size_t minVisits = constants::lin_interpolation::DSG_PARALLEL_MIN_CORNER_VISITS;
  return nDim >= static_cast<size_t>(std::bit_width(minVisits)) || (nTG << nDim) >= minVisits;
}

void prepareContext(const DSGLinInterpIndex& index, const base::DataVector& normPoint,
                    const HyperCubeArea& domain, DSGLinInterpContext& context) {
  for (size_t dim = 0; dim < index.nDim(); dim++) {
    prepareDimension(index, normPoint[dim], dim, domain[dim], context);
  }
}

void prepareDimension(const DSGLinInterpIndex& index, const double x, const size_t dim,
                      const std::pair<double, double> domain, DSGLinInterpContext& context) {
  double below = -std::numeric_limits<double>::infinity();
  double atOrAbove = std::numeric_limits<double>::infinity();
  // Bracket of the previous level: a coordinate of the current bracket is either one of them or
  // new (the nodes of a level contain those of the previous one)
  std::array<double, 2> prev{std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN()};
  std::array<uint32_t, 2> prevIds{0, 0};
  // Position of x in the delta nodes of the previous level, scaled to the current one: the hint of
  // the search (the delta nodes of consecutive levels interleave)
  size_t pos = 0;
  size_t prevCnt = 0;

  for (LvlType lvl = 0; lvl <= index.maxLvl(dim); lvl++) {
    const size_t r = index.row(dim, lvl);
    const std::span<const double> nodes = index.deltaNodes(r);
    const size_t hint = (prevCnt == 0) ? nodes.size() / 2 : pos * nodes.size() / prevCnt;
    pos = updateNeighbors(nodes, x, hint, below, atOrAbove);
    prevCnt = nodes.size();
    const std::pair<double, double> bracket = bracketOf(below, atOrAbove, x);

    assignCornerIds(index, lvl, r, bracket, domain, prev, prevIds, context);
    std::tie(context.factors[2 * r], context.factors[2 * r + 1]) =
        lin_interpolation::weights1D(x, bracket.first, bracket.second);
    prev = {bracket.first, bracket.second};
    prevIds = {context.cornerIds[2 * r], context.cornerIds[2 * r + 1]};
  }
}

size_t lowerBoundFrom(const std::span<const double> nodes, const double x, const size_t hint) {
  const size_t n = nodes.size();
  if (n == 0) {
    return 0;
  }
  const double* const data = nodes.data();
  size_t pos = std::min(hint, n - 1);
  size_t step = 1;

  if (data[pos] < x) {  // The result lies in (pos, n]
    while (pos + step < n && data[pos + step] < x) {
      pos += step;
      step *= 2;
    }
    return static_cast<size_t>(std::lower_bound(data + pos + 1, data + std::min(pos + step, n), x) -
                               data);
  }
  // The result lies in [0, pos]
  while (pos >= step && !(data[pos - step] < x)) {
    pos -= step;
    step *= 2;
  }
  const size_t first = (pos >= step) ? pos - step + 1 : 0;
  return static_cast<size_t>(std::lower_bound(data + first, data + pos, x) - data);
}

std::pair<double, double> bracketOf(const double below, const double atOrAbove, const double x) {
  if (atOrAbove == std::numeric_limits<double>::infinity()) {  // No node >= x
    return {below, 1.0};
  }
  if (std::abs(atOrAbove - x) <= 1E-12) {  // There is a node equal to x
    return {atOrAbove, atOrAbove};
  }
  if (below == -std::numeric_limits<double>::infinity()) {  // No node < x
    return {0.0, atOrAbove};
  }
  return {below, atOrAbove};
}

/**
 * Dimension 0: transformDim0Fiber. Otherwise, per level l of the fiber (ascending), the weights
 * at the block of l (the inputs, the first nInner slots) are read, the slots of the ids new at l
 * are set to zero (they receive contributions from the levels >= l only; the s = 0 slots are the
 * inputs), and the inputs are added, times the 1D weights of level l, to the slots of the two ids
 * that bracket the query coordinate at l. Every slot thus sums its contributions in ascending order
 * of the level. Unreached inputs have weight 0 (no combination tensor grid contributes to them).
 */
void transformFiber(const DSGLinInterpIndex& index, const DSGLinInterpContext& context,
                    DSGLinInterpSlotScratch& slots, const size_t dim, const size_t fiber) {
  if (dim == 0) {
    transformDim0Fiber(index, context, slots, fiber);
    return;
  }
  const std::span<const size_t> blocks = index.fiberBlocks(fiber);
  double* const weights = slots.slotWeights.data();
  bool* const reached = slots.reached.get();
  const size_t nInner = size_t{1} << index.fiberShift(fiber);

  for (size_t lvl = 0; lvl < blocks.size(); lvl++) {
    const size_t r = index.row(dim, static_cast<LvlType>(lvl));
    const size_t in = blocks[lvl];
    const size_t inEnd = in + nInner * ((index.maxNewCoords(r) == 2) ? 2 : 1);

    for (size_t o = 0; o < nInner; o++) {
      const double value = weights[in + o];
      const bool reach = reached[in + o];
      for (size_t slot = in + o; slot < inEnd; slot += nInner) {
        weights[slot] = 0.0;
        reached[slot] = false;
      }
      pushToIds(context, r, blocks, nInner, o, value, reach, weights, reached);
    }
  }
}

void transformDimension(const DSGLinInterpIndex& index, const DSGLinInterpContext& context,
                        DSGLinInterpSlotScratch& slots, const size_t dim) {
#pragma omp for schedule(dynamic, 16)
  for (size_t fiber = index.firstFiber(dim); fiber < index.firstFiber(dim + 1); fiber++) {
    transformFiber(index, context, slots, dim, fiber);
  }
}

/**
 * Adds the weighted values of the reached slots of the block of @p leaf to @p acc. The coordinates
 * of the block are prepared at its first reached slot (most blocks have none); the dimensions with
 * two new coordinates then take theirs from the bits of the slot's sub-index.
 */
template <typename ValueType>
void evaluateBlock(const ValueType values, const DSGLinInterpIndex& index,
                   const DSGLinInterpContext& context, const DSGLinInterpSlotScratch& slots,
                   const size_t leaf, DSGLinInterpScratch& scratch, typename ValueType::Acc& acc) {
  const size_t begin = index.slotOffsets()[leaf];
  const size_t end = index.slotOffsets()[leaf + 1];
  const bool* const reached = slots.reached.get();
  const double* const weights = slots.slotWeights.data();
  const double* const coords = context.coords.data();
  double* const gp = scratch.gp.data();
  const size_t* const multiDims = scratch.multiDims();
  const size_t* const multiRows = scratch.multiRows();
  size_t nMulti = 0;
  bool prepared = false;

  for (size_t slot = begin; slot < end; slot++) {
    if (!reached[slot]) {
      continue;
    }
    if (!prepared) {
      nMulti = prepareBlock(index, context, leaf, scratch);
      prepared = true;
    }
    const size_t bits = slot - begin;
    for (size_t i = 0; i < nMulti; i++) {
      gp[multiDims[i]] = coords[2 * multiRows[i] + ((bits >> i) & 1)];
    }
    values.addScaled(acc, weights[slot], values.evaluate(scratch.gp));
  }
}

template <typename ValueType>
void evaluateChunks(const ValueType values, const DSGLinInterpIndex& index,
                    const DSGLinInterpContext& context, DSGLinInterpSlotScratch& slots,
                    DSGLinInterpScratch& scratch) {
  const size_t chunkLeaves = constants::lin_interpolation::DSG_SUM_CHUNK_LEAVES;
  const size_t nChunks = DSGLinInterpSlotScratch::nChunks(index);

#pragma omp for schedule(dynamic, 1) nowait
  for (size_t chunk = 0; chunk < nChunks; chunk++) {
    typename ValueType::Acc acc = values.zeroAcc(DSGLinInterpScratch::CHUNK_ACC);
    const size_t end = std::min(index.nLeaves(), (chunk + 1) * chunkLeaves);
    for (size_t leaf = chunk * chunkLeaves; leaf < end; leaf++) {
      evaluateBlock(values, index, context, slots, leaf, scratch, acc);
    }
    values.store(acc, slots.chunkSums.data() + chunk * values.nOutputs());
  }
}

void sumChunks(const DSGLinInterpIndex& index, const DSGLinInterpSlotScratch& slots,
               const std::span<double> result) {
  const size_t nOutputs = result.size();
  const double* const chunkSums = slots.chunkSums.data();
  for (size_t chunk = 0; chunk < DSGLinInterpSlotScratch::nChunks(index); chunk++) {
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += chunkSums[chunk * nOutputs + k];
    }
  }
}

template void evaluateChunks(misc::ScalarValues, const DSGLinInterpIndex&,
                             const DSGLinInterpContext&, DSGLinInterpSlotScratch&,
                             DSGLinInterpScratch&);
template void evaluateChunks(misc::VectorValues, const DSGLinInterpIndex&,
                             const DSGLinInterpContext&, DSGLinInterpSlotScratch&,
                             DSGLinInterpScratch&);

}  // namespace dsg_lin_interpolation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
