#pragma once

#include <cstring>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

SourceFunctionCache::SourceFunctionCache(const size_t shard_count, const size_t per_shard_reserve)
    : shards_(std::max<size_t>(1, shard_count)) {
  for (auto& s : shards_) {
    if (per_shard_reserve) s.map.reserve(per_shard_reserve);
  }
}

bool SourceFunctionCache::find(const base::DataVector& point, double& f_out) const {
  size_t idx = shard_index(point);
  Shard const& sh = shards_[idx];
  std::lock_guard<std::mutex> lock(sh.mutex);
  auto it = sh.map.find(point);
  if (it != sh.map.end()) {
    f_out = it->second;
    return true;
  } else {
    return false;
  }
}

// Insert / overwrite
void SourceFunctionCache::insert(const base::DataVector& point, double value) {
  size_t idx = shard_index(point);
  Shard& sh = shards_[idx];
  std::lock_guard<std::mutex> lock(sh.mutex);
  sh.map[point] = value;
}

// Insert / overwrite
void SourceFunctionCache::insert(base::DataVector&& point, double value) {
  size_t idx = shard_index(point);
  Shard& sh = shards_[idx];
  std::lock_guard<std::mutex> lock(sh.mutex);
  sh.map[std::move(point)] = value;
}

bool SourceFunctionCache::erase(const base::DataVector& point) {
  size_t idx = shard_index(point);
  Shard& sh = shards_[idx];
  std::lock_guard<std::mutex> lock(sh.mutex);
  auto it = sh.map.find(point);
  if (it != sh.map.end()) {
    sh.map.erase(it);
    return true;
  }
  return false;
}

size_t SourceFunctionCache::size() const {
  size_t total = 0;
  for (const auto& sh : shards_) {
    std::lock_guard<std::mutex> lock(sh.mutex);
    total += sh.map.size();
  }
  return total;
}

size_t SourceFunctionCache::shardCnt() const noexcept { return shards_.size(); }

double SourceFunctionCache::findOrCompute(const base::DataVector& point,
                                          const SourceFunc sourceFunc) {
  double val;

  if (find(point, val)) {
    return val;
  }

  double computed = sourceFunc(point);

  insert(point, computed);
  return computed;
}

size_t SourceFunctionCache::shard_index(const base::DataVector& v) const noexcept {
  return hasher_(v) % shards_.size();
}

std::vector<std::unordered_map<base::DataVector, double, DataVectorHash>>
SourceFunctionCache::dump() const {
  std::vector<std::unordered_map<base::DataVector, double, DataVectorHash>> result;
  result.reserve(shards_.size());

  for (const auto& shard : shards_) {
    std::lock_guard<std::mutex> lock(shard.mutex);
    result.emplace_back(shard.map);  // Kopie der gesamten Shard-Map
  }

  return result;
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp