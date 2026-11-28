// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <bit>
#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace {

/// Mixes @p value into @p hash (boost::hash_combine with a 64-bit constant).
uint64_t combine(const uint64_t hash, const uint64_t value) noexcept {
  return hash ^ (value + 0x9E3779B97F4A7C15ull + (hash << 6) + (hash >> 2));
}

}  // namespace

NodeRowKey NodeRowKey::regular(const NodeFamily* const nodeFamily,
                               const GPCntType nInnerNodes) noexcept {
  NodeRowKey key;
  key.nodeFamily = nodeFamily;
  key.size = nInnerNodes;
  return key;
}

NodeRowKey NodeRowKey::delta(const NodeFamily* const nodeFamily, const Lvl2GPCntFunc lvl2GPCntFunc,
                             const LvlType lvl) noexcept {
  NodeRowKey key;
  key.nodeFamily = nodeFamily;
  key.lvl2GPCntFunc = lvl2GPCntFunc;
  key.size = lvl;
  return key;
}

NodeRowKey NodeRowKey::onInterval(const double lower, const double upper) const noexcept {
  NodeRowKey key = *this;
  key.lower = lower;
  key.upper = upper;
  return key;
}

bool NodeRowKey::isOnUnitInterval() const noexcept {
  return std::bit_cast<uint64_t>(lower) == std::bit_cast<uint64_t>(0.0) &&
         std::bit_cast<uint64_t>(upper) == std::bit_cast<uint64_t>(1.0);
}

size_t NodeRowKey::hash() const noexcept {
  uint64_t hash = reinterpret_cast<uintptr_t>(nodeFamily);
  hash = combine(hash, reinterpret_cast<uintptr_t>(lvl2GPCntFunc));
  hash = combine(hash, size);
  hash = combine(hash, std::bit_cast<uint64_t>(lower));
  hash = combine(hash, std::bit_cast<uint64_t>(upper));
  return static_cast<size_t>(hash);
}

bool NodeRowKey::operator==(const NodeRowKey& other) const noexcept {
  return nodeFamily == other.nodeFamily && lvl2GPCntFunc == other.lvl2GPCntFunc &&
         size == other.size &&
         std::bit_cast<uint64_t>(lower) == std::bit_cast<uint64_t>(other.lower) &&
         std::bit_cast<uint64_t>(upper) == std::bit_cast<uint64_t>(other.upper);
}

}  // namespace combigrid
}  // namespace sgpp
