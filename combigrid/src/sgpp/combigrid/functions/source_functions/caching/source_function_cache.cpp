// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <cstdint>
#include <memory>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <sgpp/combigrid/miscellaneous/exceptions/cache_file_exception.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using sgpp::combigrid::misc::BoostPointValueTable;
using sgpp::combigrid::misc::LMDBPointValueStore;
using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

SourceFuncCache::SourceFuncCache(const SourceFuncCacheConfig& config) : config_(config) {
  // Checked here, not when the table is created at the first lookup: that happens inside the
  // operators' parallel regions, where an exception would terminate the program.
#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
  BoostPointValueTable::checkShardsCnt(config.nShards);
#else
  throw base::dependency_exception(
      "SourceFuncCache: SG++ was built without the libraries of the source-function "
      "cache. " SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION);
#endif
  checkConfig();
  if (config_.policy != CachePolicy::MemoryOnly) {
    openStore();
  }
}

SourceFuncCache::~SourceFuncCache() {
  if (hasStore()) {
    try {
      writeToStore(false);  // as at the end of an operator call (flush() also syncs)
    } catch (...) {
      // A destructor must not throw; flush() reports errors.
    }
  }
  // The table refers to the store: destroy it first.
  table_.store(nullptr, std::memory_order_relaxed);
  ownedTable_.reset();
}

const SourceFuncCacheConfig& SourceFuncCache::config() const { return config_; }

size_t SourceFuncCache::nOutputs() const {
  const std::lock_guard<std::mutex> guard(mutex_);
  return nOutputs_;
}

CacheStatistics SourceFuncCache::statistics() const {
  const BoostPointValueTable* const table = table_.load(std::memory_order_acquire);
  CacheStatistics stats = (table != nullptr) ? table->statistics() : CacheStatistics();
  stats.operatorCalls = operatorCalls_.load(std::memory_order_relaxed);
  if (hasStore()) {
    stats.diskHits = store_->hits();
    stats.hits += stats.diskHits;
    stats.rejectedInserts += store_->rejected();
    stats.diskEntries = store_->entries();
    stats.fileBytes = store_->fileBytes();
    stats.loadedEntries = loadedEntries_;
  }
  return stats;
}

void SourceFuncCache::clear(const bool deletePersistentFile) {
  const std::lock_guard<std::mutex> storeGuard(storeMutex_);
  const std::lock_guard<std::mutex> guard(mutex_);
  table_.store(nullptr, std::memory_order_release);
  ownedTable_.reset();
  operatorCalls_.store(0, std::memory_order_relaxed);
  otherDimLookups_.store(0, std::memory_order_relaxed);
  loadedEntries_ = 0;

  if (hasStore()) {
    if (config_.policy == CachePolicy::TemporaryStorage || deletePersistentFile) {
      store_->clear();
    }
    store_->resetStatistics();
  }
}

void SourceFuncCache::flush() {
  if (hasStore()) {
    writeToStore(config_.policy == CachePolicy::PersistentStorage);
  }
}

void SourceFuncCache::finishOperatorCall() {
  operatorCalls_.fetch_add(1, std::memory_order_relaxed);
  if (!hasStore()) {
    return;
  }

  writeToStore(false);
  const size_t otherDim = otherDimLookups_.exchange(0, std::memory_order_relaxed);
  if (otherDim > 0) {
    const BoostPointValueTable* const table = table_.load(std::memory_order_acquire);
    throw CacheFileException("SourceFuncCache: the cache holds points of dimension " +
                             std::to_string(table->nDim()) + " ('" + config_.file + "'); " +
                             std::to_string(otherDim) +
                             " evaluations at points of another dimension were not cached");
  }
}

void SourceFuncCache::attach(const size_t nOutputs) {
  assert(nOutputs >= 1);
  const std::lock_guard<std::mutex> guard(mutex_);

  if (nOutputs_ == 0) {
    nOutputs_ = nOutputs;
  } else if (nOutputs_ != nOutputs) {
    if (hasStore() && store_->header().nOutputs == nOutputs_) {
      throw CacheFileException("SourceFuncCache: the file '" + config_.file + "' holds " +
                               std::to_string(nOutputs_) + " values per point, the function " +
                               std::to_string(nOutputs));
    }
    throw std::invalid_argument(
        "The cache holds the values of a function with another number of outputs");
  }
}

BoostPointValueTable* SourceFuncCache::table(const size_t nDim) {
  BoostPointValueTable* table = table_.load(std::memory_order_acquire);

  if (table == nullptr) {
    table = createTable(nDim);
  }

  if (table->nDim() == nDim) {
    return table;
  }
  if (hasStore()) {
    otherDimLookups_.fetch_add(1, std::memory_order_relaxed);
  }
  return nullptr;
}

BoostPointValueTable* SourceFuncCache::createTable(const size_t nDim) {
  const std::lock_guard<std::mutex> guard(mutex_);

  // Another thread may have created it in the meantime.
  if (ownedTable_ == nullptr) {
    assert(nOutputs_ >= 1);
    // A file with records fixes the dimension (e.g. after clear(false)).
    const size_t fileDim = hasStore() ? store_->header().nDim : 0;
    ownedTable_ = std::make_shared<BoostPointValueTable>(
        RecordLayout{(fileDim != 0) ? fileDim : nDim, nOutputs_}, config_.memoryLimit,
        config_.nShards, store_.get(), config_.policy == CachePolicy::PersistentStorage);
    if (fileDim != 0) {
      ownedTable_->markOnDisk();
    }
    table_.store(ownedTable_.get(), std::memory_order_release);
  }

  return ownedTable_.get();
}

/// @name Private helpers
/// @{

void SourceFuncCache::checkConfig() const {
  if (config_.policy == CachePolicy::PersistentStorage && config_.file.empty()) {
    throw std::invalid_argument("SourceFuncCache: PersistentStorage needs a file");
  }
  if (config_.policy != CachePolicy::PersistentStorage && !config_.file.empty()) {
    throw std::invalid_argument(
        "SourceFuncCache: a file can only be given with PersistentStorage (TemporaryStorage "
        "creates its own)");
  }
}

void SourceFuncCache::openStore() {
  if (config_.policy == CachePolicy::TemporaryStorage) {
    store_ = LMDBPointValueStore::createTemporary(LMDBPointValueStore::temporaryDirectory(),
                                                  config_.storageSpaceLimit, config_.nShards);
    config_.file = store_->path();
    return;
  }

  store_ =
      LMDBPointValueStore::openPersistent(config_.file, config_.storageSpaceLimit, config_.nShards);
  if (store_->header().nDim != 0) {
    loadStore();
  }
}

void SourceFuncCache::loadStore() {
  const RecordLayout layout = store_->header();
  nOutputs_ = layout.nOutputs;
  ownedTable_ = std::make_shared<BoostPointValueTable>(layout, config_.memoryLimit, config_.nShards,
                                                       store_.get(), true);

  bool complete = true;
  store_->forEachRecord([&](const uint64_t* record) {
    loadedEntries_++;
    complete = ownedTable_->insertLoaded(record) && complete;
  });
  ownedTable_->markFlushed();
  if (!complete) {
    ownedTable_->markOnDisk();
  }
  table_.store(ownedTable_.get(), std::memory_order_release);
}

void SourceFuncCache::writeToStore(const bool sync) {
  const std::lock_guard<std::mutex> guard(storeMutex_);
  BoostPointValueTable* const table = table_.load(std::memory_order_acquire);
  if (table != nullptr) {
    std::vector<const uint64_t*> records;
    std::vector<uint64_t> pendingRecords;
    table->collectUnflushed(records, pendingRecords);
    if (!records.empty()) {
      store_->write(records, table->layout());
    }
  }
  store_->rethrowError();
  if (sync) {
    store_->sync();
  }
}

/// @}

}  // namespace combigrid
}  // namespace sgpp
