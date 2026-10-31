// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/point_value_table.hpp>
#include <sgpp/combigrid/optional_libraries.hpp>

namespace sgpp {
namespace combigrid {

SourceFuncCache::SourceFuncCache(const CacheConfig& config) : config_(config) {
  requireAvailable();
}

SourceFuncCache::~SourceFuncCache() = default;

bool SourceFuncCache::isAvailable() { return HAS_CACHE_LIBRARIES; }

void SourceFuncCache::requireAvailable() {
  if (!isAvailable()) {
    throw base::not_implemented_exception(
        "SourceFuncCache / CachedSourceFunc / CachedVectorSourceFunc: SG++ was built without "
        "Boost >= 1.81 (Boost.Unordered) or xxHash >= 0.8, the source-function cache is not "
        "available");
  }
}

const CacheConfig& SourceFuncCache::config() const { return config_; }

size_t SourceFuncCache::nOutputs() const {
  const std::lock_guard<std::mutex> guard(mutex_);
  return nOutputs_;
}

CacheStatistics SourceFuncCache::statistics() const {
  const misc::PointValueTable* const table = table_.load(std::memory_order_acquire);
  CacheStatistics stats = (table != nullptr) ? table->statistics() : CacheStatistics();
  stats.operatorCalls = operatorCalls_.load(std::memory_order_relaxed);
  return stats;
}

void SourceFuncCache::clear() {
  const std::lock_guard<std::mutex> guard(mutex_);
  table_.store(nullptr, std::memory_order_release);
  ownedTable_.reset();
  operatorCalls_.store(0, std::memory_order_relaxed);
}

void SourceFuncCache::finishOperatorCall() {
  operatorCalls_.fetch_add(1, std::memory_order_relaxed);
}

void SourceFuncCache::attach(const size_t nOutputs) {
  assert(nOutputs >= 1);
  const std::lock_guard<std::mutex> guard(mutex_);

  if (nOutputs_ == 0) {
    nOutputs_ = nOutputs;
  } else if (nOutputs_ != nOutputs) {
    throw base::data_exception(
        "SourceFuncCache: the cache holds the values of a function with another number of "
        "outputs");
  }
}

misc::PointValueTable* SourceFuncCache::table(const size_t nDim) {
  misc::PointValueTable* table = table_.load(std::memory_order_acquire);

  if (table == nullptr) {
    table = createTable(nDim);
  }

  return (table->nDim() == nDim) ? table : nullptr;
}

misc::PointValueTable* SourceFuncCache::createTable(const size_t nDim) {
  const std::lock_guard<std::mutex> guard(mutex_);

  // Another thread may have created it in the meantime.
  if (ownedTable_ == nullptr) {
    assert(nOutputs_ >= 1);
    ownedTable_ = std::make_shared<misc::PointValueTable>(misc::RecordLayout{nDim, nOutputs_},
                                                          config_.memoryLimit);
    table_.store(ownedTable_.get(), std::memory_order_release);
  }

  return ownedTable_.get();
}

}  // namespace combigrid
}  // namespace sgpp
