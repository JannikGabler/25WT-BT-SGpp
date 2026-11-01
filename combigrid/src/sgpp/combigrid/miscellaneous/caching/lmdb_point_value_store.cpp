// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/base/exception/file_exception.hpp>
#include <sgpp/combigrid/miscellaneous/caching/cache_file_exception.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <string>

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
#include <limits>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>
#include <utility>
#include <vector>
#endif

using sgpp::combigrid::misc::boost_point_value_table::RecordLayout;

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

namespace sgpp {
namespace combigrid {

namespace misc {

namespace {

/// Value of the key @c HEADER_KEY in the sub-database @c meta.
struct FileHeader {
  char magic[8];
  uint32_t version;
  uint32_t keyScheme;
  uint64_t nDim;
  uint64_t nOutputs;
};

constexpr char MAGIC[8] = {'S', 'G', 'P', 'P', 'C', 'G', 'S', 'F'};
/// Format version of the file (records and header).
constexpr uint32_t VERSION = 1;
/// Key scheme 1: XXH3-64 of the bit patterns of the point in problem coordinates (the points the
/// function receives, mapped with tools::mapToDomain).
constexpr uint32_t KEY_SCHEME = 1;
constexpr char HEADER_KEY[] = "header";

std::string lmdbMessage(const std::string& what, const std::string& path, const int rc) {
  return "SourceFuncCache: " + what + " '" + path + "': " + mdb_strerror(rc);
}

/// Map size: the limit, bounded by the free space of the file system plus the existing file.
size_t mapSize(const size_t storageSpaceLimit, const std::string& directory,
               const size_t existingBytes) {
  const size_t pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
  size_t size = storageSpaceLimit;
  struct statvfs fs;
  if (statvfs(directory.c_str(), &fs) == 0) {
    const size_t available = static_cast<size_t>(fs.f_bavail) * static_cast<size_t>(fs.f_frsize);
    size = std::min(size, existingBytes + available);
  }
  size -= size % pageSize;
  return std::max(size, constants::source_func_cache::MIN_STORE_PAGES * pageSize);
}

std::string directoryOf(const std::string& path) {
  const std::filesystem::path parent = std::filesystem::path(path).parent_path();
  return parent.empty() ? std::string(".") : parent.string();
}

}  // namespace

struct LmdbPointValueStore::Impl {
  /// A reusable read transaction (reset between lookups).
  struct alignas(64) ReadSlot {
    SpinLock lock;
    MDB_txn* txn = nullptr;
  };

  ~Impl() {
    for (size_t i = 0; i < nSlots; i++) {
      if (slots[i].txn != nullptr) mdb_txn_abort(slots[i].txn);
    }
    if (env != nullptr) mdb_env_close(env);
    if (lockFd >= 0) close(lockFd);  // releases the flock
  }

  /// Opens the environment (file @p path), its sub-databases and the read slots.
  void open(const std::string& path, const size_t storageSpaceLimit, const size_t nReadSlots,
            const size_t existingBytes) {
    int rc = mdb_env_create(&env);
    if (rc == 0) rc = mdb_env_set_maxdbs(env, 2);
    if (rc == 0) rc = mdb_env_set_maxreaders(env, static_cast<unsigned int>(nReadSlots + 8));
    if (rc == 0) {
      rc = mdb_env_set_mapsize(env, mapSize(storageSpaceLimit, directoryOf(path), existingBytes));
    }
    if (rc != 0) throw CacheFileException(lmdbMessage("cannot set up", path, rc));

    rc = mdb_env_open(env, path.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOTLS | MDB_NORDAHEAD,
                      0644);
    if (rc == MDB_INVALID) {
      throw CacheFileException("SourceFuncCache: '" + path +
                               "' is no file of a source-function cache");
    }
    if (rc != 0) throw CacheFileException(lmdbMessage("cannot open", path, rc));
    checkSize(path);
    openDatabases(path);

    nSlots = nReadSlots;
    slots = std::make_unique<ReadSlot[]>(nReadSlots);
    for (size_t i = 0; i < nReadSlots; i++) {
      rc = mdb_txn_begin(env, nullptr, MDB_RDONLY, &slots[i].txn);
      if (rc != 0) throw CacheFileException(lmdbMessage("cannot read", path, rc));
      mdb_txn_reset(slots[i].txn);
    }
  }

  /// Throws if the file is shorter than its last page (a truncated file would crash LMDB).
  void checkSize(const std::string& path) const {
    MDB_envinfo info;
    MDB_stat stat;
    struct stat fileStat;
    mdb_env_info(env, &info);
    mdb_env_stat(env, &stat);
    int fd;
    mdb_env_get_fd(env, &fd);
    if (fstat(fd, &fileStat) != 0 ||
        static_cast<size_t>(fileStat.st_size) <
            (static_cast<size_t>(info.me_last_pgno) + 1) * stat.ms_psize) {
      throw CacheFileException("SourceFuncCache: '" + path + "' is truncated");
    }
  }

  /// Opens the sub-databases and reads and checks the header.
  void openDatabases(const std::string& path) {
    MDB_txn* txn;
    int rc = mdb_txn_begin(env, nullptr, 0, &txn);
    if (rc == 0) {
      rc = mdb_dbi_open(txn, "meta", 0, &meta);
      if (rc == MDB_NOTFOUND) {
        // New file: the main database must be empty (else it is another LMDB file).
        MDB_dbi main;
        MDB_stat stat;
        rc = mdb_dbi_open(txn, nullptr, 0, &main);
        if (rc == 0) rc = mdb_stat(txn, main, &stat);
        if (rc == 0 && stat.ms_entries != 0) {
          mdb_txn_abort(txn);
          throw CacheFileException("SourceFuncCache: '" + path +
                                   "' is no file of a source-function cache");
        }
        if (rc == 0) rc = mdb_dbi_open(txn, "meta", MDB_CREATE, &meta);
      }
    }
    if (rc == 0) rc = mdb_dbi_open(txn, "records", MDB_CREATE | MDB_INTEGERKEY, &records);
    if (rc == 0) {
      try {
        readHeader(txn, path);
      } catch (...) {
        mdb_txn_abort(txn);
        throw;
      }
      rc = mdb_txn_commit(txn);
    } else if (txn != nullptr) {
      mdb_txn_abort(txn);
    }
    if (rc != 0) throw CacheFileException(lmdbMessage("cannot open", path, rc));
  }

  void readHeader(MDB_txn* txn, const std::string& path) {
    MDB_val key{sizeof(HEADER_KEY), const_cast<char*>(HEADER_KEY)};
    MDB_val value;
    const int rc = mdb_get(txn, meta, &key, &value);
    if (rc == MDB_NOTFOUND) {
      MDB_stat stat;
      if (mdb_stat(txn, records, &stat) == 0 && stat.ms_entries != 0) {
        throw CacheFileException("SourceFuncCache: '" + path +
                                 "' holds records but no header (corrupted)");
      }
      return;
    }
    if (rc != 0) throw CacheFileException(lmdbMessage("cannot read", path, rc));

    FileHeader h;
    if (value.mv_size != sizeof(h)) {
      throw CacheFileException("SourceFuncCache: '" + path + "' has a corrupted header");
    }
    std::memcpy(&h, value.mv_data, sizeof(h));
    if (std::memcmp(h.magic, MAGIC, sizeof(MAGIC)) != 0 || h.nDim == 0 || h.nOutputs == 0) {
      throw CacheFileException("SourceFuncCache: '" + path + "' has a corrupted header");
    }
    if (h.version != VERSION || h.keyScheme != KEY_SCHEME) {
      throw CacheFileException("SourceFuncCache: '" + path + "' has format version " +
                               std::to_string(h.version) + " / key scheme " +
                               std::to_string(h.keyScheme) + ", expected " +
                               std::to_string(VERSION) + " / " + std::to_string(KEY_SCHEME));
    }
    header = RecordLayout{h.nDim, h.nOutputs};
  }

  /// Writes the header in @p txn (the caller holds LMDB's writer lock).
  int writeHeader(MDB_txn* txn, const RecordLayout layout) const {
    FileHeader h{};
    std::memcpy(h.magic, MAGIC, sizeof(MAGIC));
    h.version = VERSION;
    h.keyScheme = KEY_SCHEME;
    h.nDim = layout.nDim;
    h.nOutputs = layout.nOutputs;
    MDB_val key{sizeof(HEADER_KEY), const_cast<char*>(HEADER_KEY)};
    MDB_val value{sizeof(h), &h};
    return mdb_put(txn, meta, &key, &value, 0);
  }

  MDB_env* env = nullptr;
  MDB_dbi records = 0;
  MDB_dbi meta = 0;
  int lockFd = -1;  ///< Descriptor holding the flock of a persistent file.
  size_t nSlots = 0;
  std::unique_ptr<ReadSlot[]> slots;
  mutable std::mutex headerMutex;
  RecordLayout header{0, 0};  ///< Guarded by headerMutex ({0, 0}: none).
};

/// @name Factories
/// @{

std::unique_ptr<LmdbPointValueStore> LmdbPointValueStore::openPersistent(
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

std::unique_ptr<LmdbPointValueStore> LmdbPointValueStore::openPersistentFile(
    const std::string& path, const size_t storageSpaceLimit, const size_t nReadSlots) {
  auto impl = std::make_unique<Impl>();
  impl->lockFd = ::open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0644);
  if (impl->lockFd < 0) {
    throw CacheFileException("SourceFuncCache: cannot open '" + path +
                             "': " + std::strerror(errno));
  }
  if (flock(impl->lockFd, LOCK_EX | LOCK_NB) != 0) {
    throw CacheFileException("SourceFuncCache: '" + path +
                             "' is used by another cache (it is locked)");
  }
  struct stat fileStat;
  const size_t existingBytes =
      (fstat(impl->lockFd, &fileStat) == 0) ? static_cast<size_t>(fileStat.st_size) : 0;
  impl->open(path, storageSpaceLimit, nReadSlots, existingBytes);
  return std::unique_ptr<LmdbPointValueStore>(new LmdbPointValueStore(std::move(impl), path));
}

std::unique_ptr<LmdbPointValueStore> LmdbPointValueStore::createTemporary(
    const std::string& directory, const size_t storageSpaceLimit, const size_t nReadSlots) {
  std::string path = directory + "/sgpp-source-func-cache-XXXXXX";
  const int fd = mkstemp(path.data());
  if (fd < 0) {
    throw CacheFileException("SourceFuncCache: cannot create a temporary file in '" +
                             directory + "': " + std::strerror(errno));
  }
  close(fd);

  auto impl = std::make_unique<Impl>();
  try {
    impl->open(path, storageSpaceLimit, nReadSlots, 0);
  } catch (...) {
    unlink(path.c_str());
    unlink((path + "-lock").c_str());
    throw;
  }
  // Unnamed from now on: the file disappears when the process ends, however it ends.
  unlink(path.c_str());
  unlink((path + "-lock").c_str());

  int envFd;
  mdb_env_get_fd(impl->env, &envFd);
#ifdef __linux__
  const std::string fdPath = "/proc/self/fd/" + std::to_string(envFd);
#else
  const std::string fdPath = "/dev/fd/" + std::to_string(envFd);
#endif
  return std::unique_ptr<LmdbPointValueStore>(new LmdbPointValueStore(std::move(impl), fdPath));
}

std::string LmdbPointValueStore::temporaryDirectory() {
  const char* const tmpdir = std::getenv("TMPDIR");
  if (tmpdir != nullptr && tmpdir[0] != '\0') return tmpdir;
  return std::filesystem::is_directory("/var/tmp") ? "/var/tmp" : "/tmp";
}

/// @}

LmdbPointValueStore::LmdbPointValueStore(std::unique_ptr<Impl> impl, std::string path)
    : impl_(std::move(impl)), path_(std::move(path)) {}

LmdbPointValueStore::~LmdbPointValueStore() = default;

RecordLayout LmdbPointValueStore::header() const {
  const std::lock_guard<std::mutex> guard(impl_->headerMutex);
  return impl_->header;
}

bool LmdbPointValueStore::find(const size_t slot, const RecordLayout layout, uint64_t hash,
                               const double* point, double* values) {
  Impl::ReadSlot& s = impl_->slots[slot];
  const std::lock_guard<SpinLock> guard(s.lock);
  if (mdb_txn_renew(s.txn) != 0) return false;

  MDB_val key{sizeof(hash), &hash};
  MDB_val value;
  const size_t pointBytes = sizeof(double) * layout.nDim;
  const bool hit = mdb_get(s.txn, impl_->records, &key, &value) == 0 &&
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

void LmdbPointValueStore::write(std::span<const uint64_t*> records, const RecordLayout layout) {
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
    int rc = mdb_txn_begin(impl_->env, nullptr, 0, &txn);
    if (rc != 0) {
      keepError(lmdbMessage("cannot write", path_, rc));
      return;
    }

    // The writer lock is held: the header cannot change concurrently.
    bool newHeader = false;
    {
      const std::lock_guard<std::mutex> guard(impl_->headerMutex);
      if (impl_->header.nDim == 0) {
        rc = impl_->writeHeader(txn, layout);
        newHeader = true;
      } else if (impl_->header.nDim != layout.nDim ||
                 impl_->header.nOutputs != layout.nOutputs) {
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
      rc = mdb_put(txn, impl_->records, &key, &value, MDB_NOOVERWRITE);
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
      const std::lock_guard<std::mutex> guard(impl_->headerMutex);
      impl_->header = layout;
    }
  }
}

void LmdbPointValueStore::forEachRecord(
    const std::function<void(const uint64_t* record)>& visit) const {
  const RecordLayout layout = header();
  if (layout.nDim == 0) return;

  MDB_txn* txn;
  int rc = mdb_txn_begin(impl_->env, nullptr, MDB_RDONLY, &txn);
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot read", path_, rc));
  MDB_cursor* cursor;
  rc = mdb_cursor_open(txn, impl_->records, &cursor);
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
    throw CacheFileException("SourceFuncCache: '" + path_ + "' is corrupted (" + corruption +
                             ")");
  }
  if (rc != MDB_NOTFOUND) {
    throw CacheFileException(lmdbMessage("cannot read", path_, rc));
  }
}

void LmdbPointValueStore::sync() {
  const int rc = mdb_env_sync(impl_->env, 1);
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot sync", path_, rc));
}

void LmdbPointValueStore::clear() {
  MDB_txn* txn;
  int rc = mdb_txn_begin(impl_->env, nullptr, 0, &txn);
  if (rc == 0) rc = mdb_drop(txn, impl_->records, 0);
  if (rc == 0) rc = mdb_drop(txn, impl_->meta, 0);
  if (rc == 0) {
    rc = mdb_txn_commit(txn);
  } else if (txn != nullptr) {
    mdb_txn_abort(txn);
  }
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot clear", path_, rc));

  const std::lock_guard<std::mutex> guard(impl_->headerMutex);
  impl_->header = RecordLayout{0, 0};
  full_.store(false, std::memory_order_relaxed);
  warned_.store(false, std::memory_order_relaxed);
}

void LmdbPointValueStore::rethrowError() {
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

size_t LmdbPointValueStore::entries() const {
  MDB_txn* txn;
  MDB_stat stat;
  if (mdb_txn_begin(impl_->env, nullptr, MDB_RDONLY, &txn) != 0) return 0;
  const size_t n = (mdb_stat(txn, impl_->records, &stat) == 0) ? stat.ms_entries : 0;
  mdb_txn_abort(txn);
  return n;
}

size_t LmdbPointValueStore::fileBytes() const {
  int fd;
  struct stat fileStat;
  if (mdb_env_get_fd(impl_->env, &fd) != 0 || fstat(fd, &fileStat) != 0) return 0;
  return static_cast<size_t>(fileStat.st_size);
}

void LmdbPointValueStore::resetStatistics() {
  hits_.store(0, std::memory_order_relaxed);
  rejected_.store(0, std::memory_order_relaxed);
}

/// @name Private helpers
/// @{

void LmdbPointValueStore::reportFull(const char* reason) {
  full_.store(true, std::memory_order_relaxed);
  if (!warned_.exchange(true)) {
    std::cerr << "Warning: SourceFuncCache: the storage of '" << path_ << "' is full (" << reason
              << "); further values are not stored in it." << std::endl;
  }
}

void LmdbPointValueStore::keepError(const std::string& message) {
  const std::lock_guard<std::mutex> guard(errorMutex_);
  if (error_.empty()) error_ = message;
  hasError_.store(true, std::memory_order_release);
}

/// @}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

namespace sgpp {
namespace combigrid {

namespace misc {

// Without LMDB no store is ever created: the factories throw (SourceFuncCache calls them in its
// constructor). The other definitions only let the library link.

struct LmdbPointValueStore::Impl {};

std::unique_ptr<LmdbPointValueStore> LmdbPointValueStore::openPersistent(
    const std::string& /*path*/, const size_t /*storageSpaceLimit*/, const size_t /*nReadSlots*/) {
  throw base::dependency_exception(
      "SourceFuncCache: SG++ was built without the libraries of the storage policies. "
      SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION);
}

std::unique_ptr<LmdbPointValueStore> LmdbPointValueStore::createTemporary(
    const std::string& directory, const size_t storageSpaceLimit, const size_t nReadSlots) {
  return openPersistent(directory, storageSpaceLimit, nReadSlots);
}

std::string LmdbPointValueStore::temporaryDirectory() { return ""; }

LmdbPointValueStore::LmdbPointValueStore(std::unique_ptr<Impl> impl, std::string path)
    : impl_(std::move(impl)), path_(std::move(path)) {}

LmdbPointValueStore::~LmdbPointValueStore() = default;

RecordLayout LmdbPointValueStore::header() const { return RecordLayout{0, 0}; }

bool LmdbPointValueStore::find(const size_t /*slot*/, const RecordLayout /*layout*/,
                               uint64_t /*hash*/, const double* /*point*/, double* /*values*/) {
  return false;
}

void LmdbPointValueStore::write(std::span<const uint64_t*> /*records*/,
                                const RecordLayout /*layout*/) {}

void LmdbPointValueStore::forEachRecord(
    const std::function<void(const uint64_t* record)>& /*visit*/) const {}

void LmdbPointValueStore::sync() {}

void LmdbPointValueStore::clear() {}

void LmdbPointValueStore::rethrowError() {}

size_t LmdbPointValueStore::entries() const { return 0; }

size_t LmdbPointValueStore::fileBytes() const { return 0; }

void LmdbPointValueStore::resetStatistics() {}

void LmdbPointValueStore::reportFull(const char* /*reason*/) {}

void LmdbPointValueStore::keepError(const std::string& /*message*/) {}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
