// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <atomic>
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
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_query.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <tuple>
#include <utility>

using sgpp::combigrid::misc::ClosureTrie;
using sgpp::combigrid::misc::DSGLinInterpIndex;
using sgpp::combigrid::misc::DSGLinInterpQuery;
using sgpp::combigrid::misc::DSGLinInterpScratch;

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

/**
 * Calls visit(corner, slot) for the 2^d corners of the tensor grid whose corner ids are ids
 * (lower / upper neighbor of dimension j at 2j / 2j + 1), in corner index order (bit k of the
 * corner selects the upper neighbor in dimension k). The walk state (trie node and sub-index after
 * the dimensions d - 1, ..., j) is updated for the dimensions whose bit changes, as the coordinates
 * in lin_interpolation::moveToCorner.
 */
template <typename Visit>
inline void forEachCornerSlot(const uint32_t* const firstChild, const size_t* const slotOffsets,
                              const uint32_t* const ids, const size_t nDim, uint32_t* const node,
                              size_t* const sub, Visit&& visit) {
  const auto step = [&](const size_t j, const uint32_t id) {
    node[j] = firstChild[node[j + 1]] + DSGLinInterpQuery::idLvl(id);
    sub[j] = (sub[j + 1] << DSGLinInterpQuery::idShift(id)) | DSGLinInterpQuery::idSub(id);
  };

  node[nDim] = ClosureTrie::ROOT;
  sub[nDim] = 0;
  for (size_t j = nDim; j-- > 0;) {
    step(j, ids[2 * j]);
  }
  visit(size_t{0}, slotOffsets[node[0]] + sub[0]);

  const size_t nCorners = size_t{1} << nDim;
  for (size_t corner = 1; corner < nCorners; corner++) {
    const auto t = static_cast<size_t>(std::countr_zero(corner));
    step(t, ids[2 * t + 1]);
    for (size_t j = t; j-- > 0;) {
      step(j, ids[2 * j]);
    }
    visit(corner, slotOffsets[node[0]] + sub[0]);
  }
}

/// Sets the corner ids and the 1D weights of the tensor grid of level @p mi in @p scratch and
/// fills its weight table.
void prepareTG(const DSGLinInterpIndex& index, const DSGLinInterpQuery& query, const LvlMI& mi,
               DSGLinInterpScratch& scratch) {
  double* const factors = scratch.factors.data();
  for (size_t j = 0; j < index.nDim(); j++) {
    const size_t r = index.row(j, mi[j]);
    scratch.cornerIds[2 * j] = query.cornerIds[2 * r];
    scratch.cornerIds[2 * j + 1] = query.cornerIds[2 * r + 1];
    factors[2 * j] = query.factors[2 * r];
    factors[2 * j + 1] = query.factors[2 * r + 1];
  }
  lin_interpolation::fillWeightTable(factors, scratch.tableDim, scratch.weightTable.data());
}

/// Sets the coordinates of the first slot of the block of @p leaf in @c scratch.gp and collects
/// the dimensions that contribute a bit to the sub-index; returns their number.
size_t prepareBlock(const DSGLinInterpIndex& index, const DSGLinInterpQuery& query,
                    const size_t leaf, DSGLinInterpScratch& scratch) {
  const LvlMI& lambda = index.leafMI(leaf);
  size_t nMulti = 0;
  for (size_t j = 0; j < index.nDim(); j++) {
    const size_t r = index.row(j, lambda[j]);
    scratch.gp[j] = query.coords[2 * r];
    if (index.maxNewCoords(r) == 2) {
      scratch.multiDims[nMulti] = j;
      scratch.multiRows[nMulti] = r;
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
                     const std::array<uint32_t, 2>& prevIds, DSGLinInterpQuery& query) {
  const bool twoNew = (index.maxNewCoords(r) == 2);
  uint32_t nNew = 0;
  const auto idOf = [&](const double coord) {
    if (coord == prev[0]) return prevIds[0];
    if (coord == prev[1]) return prevIds[1];
    query.coords[2 * r + nNew] =
        tools::mapToDomain(coord, domain.first, domain.second - domain.first);
    return DSGLinInterpQuery::encodeId(lvl, twoNew, nNew++);
  };

  query.cornerIds[2 * r] = idOf(bracket.first);
  query.cornerIds[2 * r + 1] =
      (bracket.second == bracket.first) ? query.cornerIds[2 * r] : idOf(bracket.second);
  assert(nNew <= index.maxNewCoords(r));
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

void prepareQuery(const DSGLinInterpIndex& index, const base::DataVector& normPoint,
                  const HyperCubeArea& domain, DSGLinInterpQuery& query) {
  for (size_t dim = 0; dim < index.nDim(); dim++) {
    prepareDimension(index, normPoint[dim], dim, domain[dim], query);
  }
}

void prepareDimension(const DSGLinInterpIndex& index, const double x, const size_t dim,
                      const std::pair<double, double> domain, DSGLinInterpQuery& query) {
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

    assignCornerIds(index, lvl, r, bracket, domain, prev, prevIds, query);
    std::tie(query.factors[2 * r], query.factors[2 * r + 1]) =
        lin_interpolation::weights1D(x, bracket.first, bracket.second);
    prev = {bracket.first, bracket.second};
    prevIds = {query.cornerIds[2 * r], query.cornerIds[2 * r + 1]};
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

void setCornerIds(const DSGLinInterpIndex& index, const DSGLinInterpQuery& query, const LvlMI& mi,
                  DSGLinInterpScratch& scratch) {
  for (size_t j = 0; j < index.nDim(); j++) {
    const size_t r = index.row(j, mi[j]);
    scratch.cornerIds[2 * j] = query.cornerIds[2 * r];
    scratch.cornerIds[2 * j + 1] = query.cornerIds[2 * r + 1];
  }
}

void markCorners(const DSGLinInterpIndex& index, DSGLinInterpQuery& query,
                 DSGLinInterpScratch& scratch) {
  const std::span<const TensorGridCTData* const> tgs = index.combinationTGs();
  const uint32_t* const firstChild = index.trie().firstChild();
  const size_t* const slotOffsets = index.slotOffsets();
  uint8_t* const marks = query.marks.data();

#pragma omp for schedule(static)
  for (size_t t = 0; t < tgs.size(); t++) {
    setCornerIds(index, query, tgs[t]->mi, scratch);
    forEachCornerSlot(firstChild, slotOffsets, scratch.cornerIds.data(), index.nDim(),
                      scratch.node.data(), scratch.sub.data(), [&](size_t, const size_t slot) {
                        // Read first: most slots are shared, writing only once keeps their cache
                        // lines shared among the threads
                        std::atomic_ref<uint8_t> mark(marks[slot]);
                        if (mark.load(std::memory_order_relaxed) == 0) {
                          mark.store(1, std::memory_order_relaxed);
                        }
                      });
  }
}

/**
 * Evaluates the source function at the marked slots of the block of @p leaf. The coordinates of
 * the block are prepared at its first marked slot (most blocks have none); the dimensions with two
 * new coordinates then take theirs from the bits of the slot's sub-index.
 */
template <typename ValueType>
void evaluateBlock(const ValueType values, const DSGLinInterpIndex& index,
                   const DSGLinInterpQuery& query, const size_t leaf,
                   DSGLinInterpScratch& scratch) {
  const size_t begin = index.slotOffsets()[leaf];
  const size_t end = index.slotOffsets()[leaf + 1];
  const uint8_t* const marks = query.marks.data();
  const double* const coords = query.coords.data();
  double* const store = query.values.get();
  double* const gp = scratch.gp.data();
  const size_t* const multiDims = scratch.multiDims.data();
  const size_t* const multiRows = scratch.multiRows.data();
  size_t nMulti = 0;
  bool prepared = false;

  for (size_t slot = begin; slot < end; slot++) {
    if (marks[slot] == 0) {
      continue;
    }
    if (!prepared) {
      nMulti = prepareBlock(index, query, leaf, scratch);
      prepared = true;
    }
    const size_t bits = slot - begin;
    for (size_t i = 0; i < nMulti; i++) {
      gp[multiDims[i]] = coords[2 * multiRows[i] + ((bits >> i) & 1)];
    }
    values.evaluateInto(scratch.gp, store + slot * values.nOutputs());
  }
}

template <typename ValueType>
void evaluateMarked(const ValueType values, const DSGLinInterpIndex& index,
                    DSGLinInterpQuery& query, DSGLinInterpScratch& scratch) {
#pragma omp for schedule(dynamic, 16)
  for (size_t leaf = 0; leaf < index.nLeaves(); leaf++) {
    evaluateBlock(values, index, query, leaf, scratch);
  }
}

/**
 * Adds the weighted values of the corners of the tensor grid prepared in @p scratch (prepareTG) to
 * @p tgSum, with the weights and in the order of lin_interpolation::sumOverCorners: the table
 * entry, then the factors of the dimensions beyond the table (fixed per block of 2^tableDim
 * corners).
 */
template <typename ValueType>
void sumTGCorners(const ValueType values, const DSGLinInterpIndex& index,
                  const DSGLinInterpQuery& query, DSGLinInterpScratch& scratch,
                  typename ValueType::Acc& tgSum) {
  const double* const store = query.values.get();
  const size_t nDim = index.nDim();
  const size_t tableDim = scratch.tableDim;
  const size_t nHigh = nDim - tableDim;
  const size_t tableMask = (size_t{1} << tableDim) - 1;
  const double* const table = scratch.weightTable.data();
  const double* const factors = scratch.factors.data();
  double* const high = scratch.highFactors.data();

  forEachCornerSlot(index.trie().firstChild(), index.slotOffsets(), scratch.cornerIds.data(), nDim,
                    scratch.node.data(), scratch.sub.data(),
                    [&](const size_t corner, const size_t slot) {
                      if (nHigh > 0 && (corner & tableMask) == 0) {
                        for (size_t k = 0; k < nHigh; k++) {
                          high[k] = factors[2 * (tableDim + k) + ((corner >> (tableDim + k)) & 1)];
                        }
                      }
                      double weight = table[corner & tableMask];
                      for (size_t k = 0; k < nHigh; k++) {
                        weight *= high[k];
                      }
                      values.addScaled(tgSum, weight, values.valueAt(store, slot));
                    });
}

template <typename ValueType>
typename ValueType::Acc gatherCorners(const ValueType values, const DSGLinInterpIndex& index,
                                      const DSGLinInterpQuery& query,
                                      DSGLinInterpScratch& scratch) {
  const std::span<const TensorGridCTData* const> tgs = index.combinationTGs();
  typename ValueType::Acc total = values.zeroAcc(DSGLinInterpScratch::THREAD_ACC);

#pragma omp for schedule(static) nowait
  for (size_t t = 0; t < tgs.size(); t++) {
    prepareTG(index, query, tgs[t]->mi, scratch);
    typename ValueType::Acc tgSum = values.zeroAcc(DSGLinInterpScratch::TG_ACC);
    sumTGCorners(values, index, query, scratch, tgSum);
    values.addScaled(total, static_cast<double>(tgs[t]->coefficient), tgSum);
  }

  return total;
}

template void evaluateMarked(misc::ScalarValues, const DSGLinInterpIndex&, DSGLinInterpQuery&,
                             DSGLinInterpScratch&);
template void evaluateMarked(misc::VectorValues, const DSGLinInterpIndex&, DSGLinInterpQuery&,
                             DSGLinInterpScratch&);
template misc::ScalarValues::Acc gatherCorners(misc::ScalarValues, const DSGLinInterpIndex&,
                                               const DSGLinInterpQuery&, DSGLinInterpScratch&);
template misc::VectorValues::Acc gatherCorners(misc::VectorValues, const DSGLinInterpIndex&,
                                               const DSGLinInterpQuery&, DSGLinInterpScratch&);

}  // namespace dsg_lin_interpolation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
