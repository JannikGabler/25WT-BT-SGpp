// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <fcntl.h>
#include <lmdb.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_read_slot.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>
#include <sgpp/combigrid/miscellaneous/exceptions/cache_file_exception.hpp>
#include <sgpp/combigrid/tools/caching/lmdb_tools.hpp>
#include <string>

using namespace sgpp::combigrid::tools::lmdb;
using sgpp::combigrid::misc::lmdb_point_value_store::FileInterface;
using sgpp::combigrid::misc::lmdb_point_value_store::ReadSlot;
using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

namespace misc {

/// @name Factories
/// @{

std::unique_ptr<LMDBPointValueStore> LMDBPointValueStore::openPersistent(
    const std::string& path, const size_t storageSpaceLimit, const size_t nReadSlots) {
  // Files this call creates are removed again if the store cannot be opened.
  const bool newFile = !std::filesystem::exists(path);
  const bool newLockFile = !std::filesystem::exists(path + "-lock");
  try {
    return openPersistentFile(path, storageSpaceLimit, nReadSlots);
  } catch (...) {
    std::error_code ignored;
    if (newLockFile) std::filesystem::remove(path + "-lock", ignored);
    if (newFile) std::filesystem::remove(path, ignored);
    throw;
  }
}

std::unique_ptr<LMDBPointValueStore> LMDBPointValueStore::openPersistentFile(
    const std::string& path, const size_t storageSpaceLimit, const size_t nReadSlots) {
  std::unique_ptr<FileInterface> interface = std::make_unique<FileInterface>();
  interface->lockFd = ::open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0644);
  if (interface->lockFd < 0) {
    throw CacheFileException("SourceFuncCache: cannot open '" + path +
                             "': " + std::strerror(errno));
  }
  if (flock(interface->lockFd, LOCK_EX | LOCK_NB) != 0) {
    throw CacheFileException("SourceFuncCache: '" + path +
                             "' is used by another cache (it is locked)");
  }
  struct stat fileStat;
  const size_t existingBytes =
      (fstat(interface->lockFd, &fileStat) == 0) ? static_cast<size_t>(fileStat.st_size) : 0;
  interface->open(path, storageSpaceLimit, nReadSlots, existingBytes);
  return std::unique_ptr<LMDBPointValueStore>(new LMDBPointValueStore(std::move(interface), path));
}

std::unique_ptr<LMDBPointValueStore> LMDBPointValueStore::createTemporary(
    const std::string& directory, const size_t storageSpaceLimit, const size_t nReadSlots) {
  std::string path = directory + "/sgpp-source-func-cache-XXXXXX";
  const int fd = mkstemp(path.data());
  if (fd < 0) {
    throw CacheFileException("SourceFuncCache: cannot create a temporary file in '" + directory +
                             "': " + std::strerror(errno));
  }
  close(fd);

  std::unique_ptr<FileInterface> interface = std::make_unique<FileInterface>();
  try {
    interface->open(path, storageSpaceLimit, nReadSlots, 0);
  } catch (...) {
    unlink(path.c_str());
    unlink((path + "-lock").c_str());
    throw;
  }
  // Unnamed from now on: the file disappears when the process ends, however it ends.
  unlink(path.c_str());
  unlink((path + "-lock").c_str());

  int envFd;
  mdb_env_get_fd(interface->env, &envFd);
#ifdef __linux__
  const std::string fdPath = "/proc/self/fd/" + std::to_string(envFd);
#else
  const std::string fdPath = "/dev/fd/" + std::to_string(envFd);
#endif
  return std::unique_ptr<LMDBPointValueStore>(
      new LMDBPointValueStore(std::move(interface), fdPath));
}

std::string LMDBPointValueStore::temporaryDirectory() {
  const char* const tmpdir = std::getenv("TMPDIR");
  if (tmpdir != nullptr && tmpdir[0] != '\0') return tmpdir;
  return std::filesystem::is_directory("/var/tmp") ? "/var/tmp" : "/tmp";
}

/// @}

LMDBPointValueStore::LMDBPointValueStore(std::unique_ptr<FileInterface> interface, std::string path)
    : interface_(std::move(interface)), path_(std::move(path)) {}

LMDBPointValueStore::~LMDBPointValueStore() = default;

RecordLayout LMDBPointValueStore::header() const {
  const std::lock_guard<std::mutex> guard(interface_->headerMutex);
  return interface_->header;
}

bool LMDBPointValueStore::find(const size_t slot, const RecordLayout layout, uint64_t hash,
                               const double* point, double* values) {
  ReadSlot& s = interface_->slots[slot];
  const std::lock_guard<SpinLock> guard(s.lock);
  if (mdb_txn_renew(s.txn) != 0) return false;

  MDB_val key{sizeof(hash), &hash};
  MDB_val value;
  const size_t pointBytes = sizeof(double) * layout.nDim;
  const bool hit = mdb_get(s.txn, interface_->records, &key, &value) == 0 &&
                   value.mv_size == pointBytes + sizeof(double) * layout.nOutputs &&
                   std::memcmp(value.mv_data, point, pointBytes) == 0;
  if (hit) {
    std::memcpy(values, static_cast<const char*>(value.mv_data) + pointBytes,
                sizeof(double) * layout.nOutputs);
  }
  mdb_txn_reset(s.txn);

  if (hit) hits_.fetch_add(1, std::memory_order_relaxed);
  return hit;
}

void LMDBPointValueStore::write(std::span<const uint64_t*> records, const RecordLayout layout) {
  if (full()) {
    rejected_.fetch_add(records.size(), std::memory_order_relaxed);
    return;
  }
  std::ranges::sort(records, {}, [](const uint64_t* record) { return record[0]; });

  const size_t pointBytes = sizeof(double) * layout.nDim;
  const size_t valueBytes = pointBytes + sizeof(double) * layout.nOutputs;
  const size_t txnRecords = constants::source_func_cache::STORE_RECORDS_PER_TXN;

  for (size_t begin = 0; begin < records.size(); begin += txnRecords) {
    const size_t end = std::min(records.size(), begin + txnRecords);
    MDB_txn* txn;
    int rc = mdb_txn_begin(interface_->env, nullptr, 0, &txn);
    if (rc != 0) {
      keepError(lmdbMessage("cannot write", path_, rc));
      return;
    }

    // The writer lock is held: the header cannot change concurrently.
    bool newHeader = false;
    {
      const std::lock_guard<std::mutex> guard(interface_->headerMutex);
      if (interface_->header.nDim == 0) {
        rc = interface_->writeHeader(txn, layout);
        newHeader = true;
      } else if (interface_->header.nDim != layout.nDim ||
                 interface_->header.nOutputs != layout.nOutputs) {
        mdb_txn_abort(txn);
        keepError("SourceFuncCache: records of another layout for '" + path_ + "'");
        return;
      }
    }

    size_t collisions = 0;
    for (size_t i = begin; rc == 0 && i < end; i++) {
      uint64_t hash = records[i][0];
      MDB_val key{sizeof(hash), &hash};
      MDB_val value{valueBytes, const_cast<uint64_t*>(records[i] + 1)};
      rc = mdb_put(txn, interface_->records, &key, &value, MDB_NOOVERWRITE);
      if (rc == MDB_KEYEXIST) {
        // value now holds the stored record: the same point (skip) or a hash collision.
        if (std::memcmp(value.mv_data, records[i] + 1, pointBytes) != 0) collisions++;
        rc = 0;
      }
    }
    if (rc == 0) {
      rc = mdb_txn_commit(txn);
    } else {
      mdb_txn_abort(txn);
    }

    if (rc == MDB_MAP_FULL || rc == ENOSPC) {
      rejected_.fetch_add(records.size() - begin, std::memory_order_relaxed);
      reportFull(rc == ENOSPC ? "the disk is full" : "storageSpaceLimit reached");
      return;
    }
    if (rc != 0) {
      keepError(lmdbMessage("cannot write", path_, rc));
      return;
    }
    rejected_.fetch_add(collisions, std::memory_order_relaxed);
    if (newHeader) {
      const std::lock_guard<std::mutex> guard(interface_->headerMutex);
      interface_->header = layout;
    }
  }
}

void LMDBPointValueStore::forEachRecord(
    const std::function<void(const uint64_t* record)>& visit) const {
  const RecordLayout layout = header();
  if (layout.nDim == 0) return;

  MDB_txn* txn;
  int rc = mdb_txn_begin(interface_->env, nullptr, MDB_RDONLY, &txn);
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot read", path_, rc));
  MDB_cursor* cursor;
  rc = mdb_cursor_open(txn, interface_->records, &cursor);
  if (rc != 0) {
    mdb_txn_abort(txn);
    throw CacheFileException(lmdbMessage("cannot read", path_, rc));
  }

  std::vector<uint64_t> record(layout.stride());
  const size_t valueBytes = layout.bytes() - sizeof(uint64_t);
  MDB_val key;
  MDB_val value;
  std::string corruption;
  for (rc = mdb_cursor_get(cursor, &key, &value, MDB_FIRST); rc == 0;
       rc = mdb_cursor_get(cursor, &key, &value, MDB_NEXT)) {
    if (key.mv_size != sizeof(uint64_t) || value.mv_size != valueBytes) {
      corruption = "a record of a wrong size";
      break;
    }
    std::memcpy(record.data(), key.mv_data, sizeof(uint64_t));
    std::memcpy(record.data() + 1, value.mv_data, valueBytes);
    const auto* point = reinterpret_cast<const double*>(record.data() + 1);
    if (BoostPointValueTable::hash(point, layout.nDim) != record[0]) {
      corruption = "a record whose key is not the hash of its point";
      break;
    }
    visit(record.data());
  }
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);

  if (!corruption.empty()) {
    throw CacheFileException("SourceFuncCache: '" + path_ + "' is corrupted (" + corruption + ")");
  }
  if (rc != MDB_NOTFOUND) {
    throw CacheFileException(lmdbMessage("cannot read", path_, rc));
  }
}

void LMDBPointValueStore::sync() {
  const int rc = mdb_env_sync(interface_->env, 1);
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot sync", path_, rc));
}

void LMDBPointValueStore::clear() {
  MDB_txn* txn;
  int rc = mdb_txn_begin(interface_->env, nullptr, 0, &txn);
  if (rc == 0) rc = mdb_drop(txn, interface_->records, 0);
  if (rc == 0) rc = mdb_drop(txn, interface_->meta, 0);
  if (rc == 0) {
    rc = mdb_txn_commit(txn);
  } else if (txn != nullptr) {
    mdb_txn_abort(txn);
  }
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot clear", path_, rc));

  const std::lock_guard<std::mutex> guard(interface_->headerMutex);
  interface_->header = RecordLayout{0, 0};
  full_.store(false, std::memory_order_relaxed);
  warned_.store(false, std::memory_order_relaxed);
}

void LMDBPointValueStore::rethrowError() {
  if (!hasError_.load(std::memory_order_acquire)) return;
  std::string message;
  {
    const std::lock_guard<std::mutex> guard(errorMutex_);
    message = std::move(error_);
    error_.clear();
    hasError_.store(false, std::memory_order_relaxed);
  }
  throw CacheFileException(message);
}

size_t LMDBPointValueStore::entries() const {
  MDB_txn* txn;
  MDB_stat stat;
  if (mdb_txn_begin(interface_->env, nullptr, MDB_RDONLY, &txn) != 0) return 0;
  const size_t n = (mdb_stat(txn, interface_->records, &stat) == 0) ? stat.ms_entries : 0;
  mdb_txn_abort(txn);
  return n;
}

size_t LMDBPointValueStore::fileBytes() const {
  int fd;
  struct stat fileStat;
  if (mdb_env_get_fd(interface_->env, &fd) != 0 || fstat(fd, &fileStat) != 0) return 0;
  return static_cast<size_t>(fileStat.st_size);
}

void LMDBPointValueStore::resetStatistics() {
  hits_.store(0, std::memory_order_relaxed);
  rejected_.store(0, std::memory_order_relaxed);
}

/// @name Private helpers
/// @{

void LMDBPointValueStore::reportFull(const char* reason) {
  full_.store(true, std::memory_order_relaxed);
  if (!warned_.exchange(true)) {
    std::cerr << "Warning: SourceFuncCache: the storage of '" << path_ << "' is full (" << reason
              << "); further values are not stored in it." << std::endl;
  }
}

void LMDBPointValueStore::keepError(const std::string& message) {
  const std::lock_guard<std::mutex> guard(errorMutex_);
  if (error_.empty()) error_ = message;
  hasError_.store(true, std::memory_order_release);
}

/// @}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include "sgpp/base/exception/dependency_exception.hpp"

using sgpp::combigrid::misc::lmdb_point_value_store::FileInterface;
using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

namespace misc {

// Without LMDB no store is ever created: the factories throw (SourceFuncCache calls them in its
// constructor). The other definitions only let the library link.

std::unique_ptr<LMDBPointValueStore> LMDBPointValueStore::openPersistent(
    const std::string& /*path*/, const size_t /*storageSpaceLimit*/, const size_t /*nReadSlots*/) {
  throw base::dependency_exception(
      "SourceFuncCache: SG++ was built without the libraries of the storage "
      "policies. " SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION);
}

std::unique_ptr<LMDBPointValueStore> LMDBPointValueStore::createTemporary(
    const std::string& directory, const size_t storageSpaceLimit, const size_t nReadSlots) {
  return openPersistent(directory, storageSpaceLimit, nReadSlots);
}

std::string LMDBPointValueStore::temporaryDirectory() { return ""; }

LMDBPointValueStore::LMDBPointValueStore(std::unique_ptr<FileInterface> interface, std::string path)
    : interface_(std::move(interface)), path_(std::move(path)) {}

LMDBPointValueStore::~LMDBPointValueStore() = default;

RecordLayout LMDBPointValueStore::header() const { return RecordLayout{0, 0}; }

bool LMDBPointValueStore::find(const size_t /*slot*/, const RecordLayout /*layout*/,
                               uint64_t /*hash*/, const double* /*point*/, double* /*values*/) {
  return false;
}

void LMDBPointValueStore::write(std::span<const uint64_t*> /*records*/,
                                const RecordLayout /*layout*/) {}

void LMDBPointValueStore::forEachRecord(
    const std::function<void(const uint64_t* record)>& /*visit*/) const {}

void LMDBPointValueStore::sync() {}

void LMDBPointValueStore::clear() {}

void LMDBPointValueStore::rethrowError() {}

size_t LMDBPointValueStore::entries() const { return 0; }

size_t LMDBPointValueStore::fileBytes() const { return 0; }

void LMDBPointValueStore::resetStatistics() {}

void LMDBPointValueStore::reportFull(const char* /*reason*/) {}

void LMDBPointValueStore::keepError(const std::string& /*message*/) {}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
