// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/grids/node_storage.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {

constexpr size_t MIN_PRUNE_SIZE = 64;

/// Number of nodes of the row of @p key (with the boundary nodes).
size_t rowSize(const NodeRowKey& key) {
  if (key.lvl2GPCntFunc == nullptr) {
    return static_cast<size_t>(key.size) + 2;
  }
  return tools::getDeltaNodeCnt(static_cast<LvlType>(key.size), key.lvl2GPCntFunc, true);
}

/// Generates the row of @p key on the unit interval.
NodeStorage::RowPtr genUnitRow(const NodeRowKey& key) {
  std::shared_ptr<base::DataVector> row = std::make_shared<base::DataVector>(rowSize(key));

  if (key.lvl2GPCntFunc == nullptr) {
    key.nodeFamily->genNodesWithBoundary(static_cast<GPCntType>(row->size()), *row);
  } else {
    key.nodeFamily->genDeltaNodesWithBoundary(static_cast<LvlType>(key.size), key.lvl2GPCntFunc,
                                              std::span<double>(*row));
  }
  return row;
}

/// Maps @p unitRow onto the interval of @p key (@c tools::mapToDomain).
NodeStorage::RowPtr mapRow(const NodeRowKey& key, const base::DataVector& unitRow) {
  const double width = key.upper - key.lower;

  std::shared_ptr<base::DataVector> row = std::make_shared<base::DataVector>(unitRow.size());

  for (size_t i = 0; i < unitRow.size(); i++) {
    (*row)[i] = tools::mapToDomain(unitRow[i], key.lower, width);
  }

  return row;
}

/**
 * Generates the rows @p generate(mine[j]) into @p rows; in parallel if they are large (every row
 * is allocated by the thread that generates it). Exceptions are caught per row (they must not
 * leave the parallel region) and the first one is returned.
 */
std::exception_ptr generateRows(const std::vector<size_t>& mine,
                                const std::function<NodeStorage::RowPtr(size_t)>& generate,
                                const std::function<size_t(size_t)>& expectedSize,
                                std::vector<NodeStorage::RowPtr>& rows) {
  size_t nNodes = 0;
  for (const size_t idx : mine) nNodes += expectedSize(idx);
  const bool parallel = nNodes >= constants::sparse_grid_storage::MIN_NODES_FOR_CONCURRENCY;

  rows.assign(mine.size(), nullptr);

  std::vector<std::exception_ptr> errors(mine.size());

#pragma omp parallel for schedule(dynamic) if (parallel)
  for (size_t j = 0; j < mine.size(); j++) {
    try {
      rows[j] = generate(mine[j]);
    } catch (...) {
      errors[j] = std::current_exception();
    }
  }

  const auto failed = std::ranges::find_if(errors, [](const auto& e) { return e != nullptr; });
  return failed == errors.end() ? nullptr : *failed;
}

}  // namespace

NodeStorage::NodeStorage() : pruneAt_(MIN_PRUNE_SIZE) {}

NodeStorage::~NodeStorage() = default;

const std::shared_ptr<NodeStorage>& NodeStorage::global() {
  static const std::shared_ptr<NodeStorage> storage = std::make_shared<NodeStorage>();
  return storage;
}

/**
 * First the rows on the unit interval (the requested ones and those the other rows are mapped
 * from), then the rows on other intervals, mapped from them.
 */
std::vector<NodeStorage::RowPtr> NodeStorage::getRows(const std::span<const NodeRowKey> keys) {
  std::vector<NodeRowKey> unitKeys(keys.size());
  for (size_t i = 0; i < keys.size(); i++) unitKeys[i] = keys[i].onInterval(0.0, 1.0);

  std::vector<RowPtr> rows = resolve(
      unitKeys, [&](const size_t i) { return genUnitRow(unitKeys[i]); },
      [&](const size_t i) { return rowSize(unitKeys[i]); });

  std::vector<size_t> mapped;  // Keys not on the unit interval
  std::vector<NodeRowKey> mappedKeys;
  for (size_t i = 0; i < keys.size(); i++) {
    if (!keys[i].isOnUnitInterval()) {
      mapped.push_back(i);
      mappedKeys.push_back(keys[i]);
    }
  }

  const std::vector<RowPtr> mappedRows = resolve(
      mappedKeys, [&](const size_t j) { return mapRow(mappedKeys[j], *rows[mapped[j]]); },
      [&](const size_t j) { return rows[mapped[j]]->size(); });

  for (size_t j = 0; j < mapped.size(); j++) rows[mapped[j]] = mappedRows[j];

  return rows;
}

NodeStorage::RowPtr NodeStorage::getRow(const NodeRowKey& key) {
  return getRows(std::span<const NodeRowKey>(&key, 1)).front();
}

size_t NodeStorage::nRows() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return static_cast<size_t>(
      std::ranges::count_if(rows_, [](const auto& entry) { return !entry.second.expired(); }));
}

size_t NodeStorage::nStoredNodes() const {
  std::lock_guard<std::mutex> lock(mutex_);
  size_t result = 0;
  for (const auto& entry : rows_) {
    if (const RowPtr row = entry.second.lock()) result += row->size();
  }
  return result;
}

/**
 * The rows this request generates are registered as pending under the lock, generated without
 * it and published under it again; then the promises are fulfilled (other requests waiting for
 * them continue) and the rows of other requests are waited for. A request never waits while it
 * has rows to generate, so concurrent requests cannot block each other.
 */
std::vector<NodeStorage::RowPtr> NodeStorage::resolve(
    const std::span<const NodeRowKey> keys, const Generator& generate,
    const std::function<size_t(size_t)>& expectedSize) {
  std::vector<RowPtr> result(keys.size());
  std::vector<std::shared_future<RowPtr>> waits(keys.size());
  std::vector<size_t> mine;
  std::vector<std::promise<RowPtr>> promises;
  claim(keys, result, waits, mine, promises);

  std::vector<RowPtr> generated;
  const std::exception_ptr error = generateRows(mine, generate, expectedSize, generated);
  publish(keys, mine, generated);

  for (size_t j = 0; j < mine.size(); j++) {
    if (generated[j] != nullptr) {
      promises[j].set_value(generated[j]);
      result[mine[j]] = generated[j];
    } else {
      promises[j].set_exception(error);
    }
  }
  if (error) std::rethrow_exception(error);

  for (size_t i = 0; i < keys.size(); i++) {
    if (waits[i].valid()) result[i] = waits[i].get();
  }

  return result;
}

void NodeStorage::claim(const std::span<const NodeRowKey> keys, std::vector<RowPtr>& result,
                        std::vector<std::shared_future<RowPtr>>& waits, std::vector<size_t>& mine,
                        std::vector<std::promise<RowPtr>>& promises) {
  std::lock_guard<std::mutex> lock(mutex_);

  pruneIfLarge();

  for (size_t i = 0; i < keys.size(); i++) {
    const auto row = rows_.find(keys[i]);
    if (row != rows_.end() && (result[i] = row->second.lock()) != nullptr) continue;

    const auto pending = pending_.find(keys[i]);
    if (pending != pending_.end()) {
      waits[i] = pending->second;  // Also a duplicate key of this request: fulfilled before waiting
      continue;
    }

    promises.emplace_back();
    pending_.emplace(keys[i], promises.back().get_future().share());
    mine.push_back(i);
  }
}

void NodeStorage::publish(const std::span<const NodeRowKey> keys, const std::vector<size_t>& mine,
                          const std::vector<RowPtr>& rows) {
  std::lock_guard<std::mutex> lock(mutex_);

  for (size_t j = 0; j < mine.size(); j++) {
    const NodeRowKey& key = keys[mine[j]];
    pending_.erase(key);
    if (rows[j] != nullptr) rows_.insert_or_assign(key, rows[j]);
  }
}

void NodeStorage::pruneIfLarge() {
  if (rows_.size() < pruneAt_) return;

  std::erase_if(rows_, [](const auto& entry) { return entry.second.expired(); });
  pruneAt_ = std::max(MIN_PRUNE_SIZE, 2 * rows_.size());
}

}  // namespace combigrid
}  // namespace sgpp
