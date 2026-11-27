// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

namespace {

constexpr size_t INITIAL_TABLE_SIZE = 64;

}  // namespace

RefinementIndexSet::RefinementIndexSet(const size_t nDim)
    : nDim_(nDim), table_(INITIAL_TABLE_SIZE, 0) {}

/**
 * The SplitMix64 finalizer of (dimension, level): consecutive levels and dimensions get
 * independent-looking 64-bit values, so sums of them collide only by chance.
 */
uint64_t RefinementIndexSet::entryHash(const size_t dim, const LvlType lvl) noexcept {
  uint64_t z = ((static_cast<uint64_t>(dim) << 32) | lvl) + 0x9E3779B97F4A7C15ull;
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  return z ^ (z >> 31);
}

uint64_t RefinementIndexSet::hashOf(const LvlMIView mi) const noexcept {
  assert(mi.size() == nDim_);
  uint64_t hash = 0;
  for (size_t dim = 0; dim < nDim_; dim++) {
    hash += entryHash(dim, mi[dim]);
  }
  return hash;
}

template <typename Matches>
size_t RefinementIndexSet::lookup(const uint64_t hash, const Matches& matches) const noexcept {
  const size_t mask = table_.size() - 1;

  for (size_t slot = hash & mask;; slot = (slot + 1) & mask) {
    const size_t stored = table_[slot];
    if (stored == 0) {
      return NOT_FOUND;
    }
    const size_t idx = stored - 1;
    if (hashes_[idx] == hash && matches(levels_.data() + idx * nDim_)) {
      return idx;
    }
  }
}

size_t RefinementIndexSet::find(const LvlMIView mi) const noexcept {
  return lookup(hashOf(mi), [&](const LvlType* const levels) {
    return std::equal(levels, levels + nDim_, mi.data());
  });
}

size_t RefinementIndexSet::findNeighbour(const LvlMIView mi, const uint64_t hash, const size_t dim,
                                         const int step) const noexcept {
  assert(dim < nDim_ && (step == 1 || step == -1));
  if (step < 0 && mi[dim] == 0) {
    return NOT_FOUND;
  }

  const LvlType lvl = static_cast<LvlType>(static_cast<int64_t>(mi[dim]) + step);
  const uint64_t neighbourHash = hash - entryHash(dim, mi[dim]) + entryHash(dim, lvl);

  return lookup(neighbourHash, [&](const LvlType* const levels) {
    for (size_t k = 0; k < nDim_; k++) {
      if (levels[k] != (k == dim ? lvl : mi[k])) {
        return false;
      }
    }
    return true;
  });
}

size_t RefinementIndexSet::findBackward(const LvlMIView mi, const uint64_t hash,
                                        const std::span<const size_t> dims,
                                        const uint64_t mask) const noexcept {
  assert(dims.size() <= 64 && std::ranges::is_sorted(dims));
  uint64_t backwardHash = hash;
  for (uint64_t bits = mask; bits != 0; bits &= bits - 1) {
    const size_t dim = dims[static_cast<size_t>(std::countr_zero(bits))];
    assert(mi[dim] > 0);
    backwardHash += entryHash(dim, mi[dim] - 1) - entryHash(dim, mi[dim]);
  }

  // dims is ascending: one merged pass compares every entry with its expected level
  return lookup(backwardHash, [&](const LvlType* const levels) {
    size_t i = 0;
    for (size_t k = 0; k < nDim_; k++) {
      LvlType expected = mi[k];
      if (i < dims.size() && dims[i] == k) {
        expected -= static_cast<LvlType>((mask >> i) & 1u);
        i++;
      }
      if (levels[k] != expected) {
        return false;
      }
    }
    return true;
  });
}

size_t RefinementIndexSet::insert(const LvlMIView mi) {
  assert(mi.size() == nDim_);
  assert(find(mi) == NOT_FOUND);

  if (2 * (size() + 1) > table_.size()) {
    growTable();
  }

  const size_t idx = size();
  hashes_.push_back(hashOf(mi));
  levels_.insert(levels_.end(), mi.data(), mi.data() + nDim_);
  place(idx);

  return idx;
}

void RefinementIndexSet::reserve(const size_t nMI) {
  levels_.reserve(nMI * nDim_);
  hashes_.reserve(nMI);
  while (2 * nMI > table_.size()) {
    growTable();
  }
}

void RefinementIndexSet::place(const size_t idx) noexcept {
  const size_t mask = table_.size() - 1;
  size_t slot = hashes_[idx] & mask;
  while (table_[slot] != 0) {
    slot = (slot + 1) & mask;
  }
  table_[slot] = idx + 1;
}

void RefinementIndexSet::growTable() {
  table_.assign(table_.size() * 2, 0);
  for (size_t idx = 0; idx < size(); idx++) {
    place(idx);
  }
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
