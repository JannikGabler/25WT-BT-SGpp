// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <memory>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_layout.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

SourceFuncCache::SourceFuncCache(const SourceFuncCacheConfig& config) : config_(config) {
  // Checked here, not when the table is created at the first lookup: that happens inside the
  // operators' parallel regions, where an exception would terminate the program.
#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
  misc::BoostPointValueTable::checkShardsCnt(config.nShards);
#else
  throw base::dependency_exception(
      "SourceFuncCache: SG++ was built without the libraries of the source-function cache. "
      SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION);
#endif
}

SourceFuncCache::~SourceFuncCache() = default;

const SourceFuncCacheConfig& SourceFuncCache::config() const { return config_; }

size_t SourceFuncCache::nOutputs() const {
  const std::lock_guard<std::mutex> guard(mutex_);
  return nOutputs_;
}

CacheStatistics SourceFuncCache::statistics() const {
  const misc::BoostPointValueTable* const table = table_.load(std::memory_order_acquire);
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
    throw std::invalid_argument(
        "The cache holds the values of a function with another number of outputs");
  }
}

misc::BoostPointValueTable* SourceFuncCache::table(const size_t nDim) {
  misc::BoostPointValueTable* table = table_.load(std::memory_order_acquire);

  if (table == nullptr) {
    table = createTable(nDim);
  }

  return (table->nDim() == nDim) ? table : nullptr;
}

misc::BoostPointValueTable* SourceFuncCache::createTable(const size_t nDim) {
  const std::lock_guard<std::mutex> guard(mutex_);

  // Another thread may have created it in the meantime.
  if (ownedTable_ == nullptr) {
    assert(nOutputs_ >= 1);
    ownedTable_ = std::make_shared<misc::BoostPointValueTable>(
        misc::boost_point_value_table::RecordLayout{nDim, nOutputs_}, config_.memoryLimit,
        config_.nShards);
    table_.store(ownedTable_.get(), std::memory_order_release);
  }

  return ownedTable_.get();
}

}  // namespace combigrid
}  // namespace sgpp
