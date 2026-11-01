// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file lmdb_point_value_store.hpp
 * @brief Disk store of the source-function cache (LMDB): the records beyond the memory of the
 * table, and with the persistent policy all records.
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <span>
#include <string>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Disk store of the records of a @c SourceFuncCache (@c CachePolicy::TemporaryStorage
 * and @c PersistentStorage), an LMDB environment with one file (@c MDB_NOSUBDIR).
 *
 * - Records: key = the 8-byte XXH3 hash of the point (@c MDB_INTEGERKEY), value = point and
 *   values (8 (d + m) bytes); a lookup compares the point bitwise. A second point with the same
 *   hash is not stored (probability about n^2 / 2^65).
 * - Header (sub-database @c meta): magic, format version, key scheme, d, m; written with the
 *   first records. Opening an existing file checks it, the file size against the last page
 *   and, while the records are loaded (@ref forEachRecord), every key against the hash of its
 *   point. Any mismatch throws @c base::file_exception.
 * - Crash safety: LMDB commits copy-on-write, so a program crash never corrupts the file
 *   (@c MDB_NOSYNC: only an operating-system crash or a power loss can lose or damage the last
 *   commits; @ref sync writes them through).
 * - Thread-safe: one writer at a time (LMDB's writer lock), lookups in parallel through one
 *   reusable read transaction per slot (@c MDB_NOTLS; the caller passes the slot, e.g. the
 *   shard of the point).
 * - Size: at most @c storageSpaceLimit bytes (the LMDB map size; also bounded by the free space
 *   of the file system at creation). A write beyond it fails: the store reports @ref full,
 *   prints one warning on @c std::cerr and takes no further records.
 * - Persistent files are locked exclusively (@c flock); LMDB keeps its lock file
 *   <tt>file + "-lock"</tt> next to the file. Temporary files (and their lock file) are
 *   unlinked right after their creation, so they disappear when the process ends, however it
 *   ends.
 *
 * Needs LMDB and POSIX (@c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE, see
 * dependencies_defines.hpp); without them the class compiles, but its factories throw
 * @c base::dependency_exception.
 */
class LMDBPointValueStore {
 public:
  /**
   * @brief Opens (or creates) a persistent store.
   * @param path              File of the store (created if missing).
   * @param storageSpaceLimit Maximal size of the file in bytes.
   * @param nReadSlots        Number of read slots (@ref find).
   * @return The store.
   * @throws base::file_exception if the file cannot be opened or locked, is no store of the
   * source-function cache, is truncated or has another format version or key scheme.
   * @throws base::dependency_exception without LMDB.
   */
  static std::unique_ptr<LMDBPointValueStore> openPersistent(const std::string& path,
                                                             size_t storageSpaceLimit,
                                                             size_t nReadSlots);

  /**
   * @brief Creates a temporary store (an unnamed file in @p directory).
   * @param directory         Directory of the file.
   * @param storageSpaceLimit Maximal size of the file in bytes.
   * @param nReadSlots        Number of read slots (@ref find).
   * @return The store (@ref path: @c /proc/self/fd/N).
   * @throws base::file_exception if the file cannot be created.
   * @throws base::dependency_exception without LMDB.
   */
  static std::unique_ptr<LMDBPointValueStore> createTemporary(const std::string& directory,
                                                              size_t storageSpaceLimit,
                                                              size_t nReadSlots);

  /// @brief Returns the directory of temporary stores: @c $TMPDIR if set, else @c /var/tmp
  /// (@c /tmp is a RAM file system on many systems).
  static std::string temporaryDirectory();

  ~LMDBPointValueStore();

  LMDBPointValueStore(const LMDBPointValueStore&) = delete;
  LMDBPointValueStore& operator=(const LMDBPointValueStore&) = delete;

  /// @brief Returns the path of the file (temporary: @c /proc/self/fd/N, Linux).
  const std::string& path() const { return path_; }

  /// @brief Returns the layout of the records of the header ({0, 0}: no header yet, i.e. no
  /// records).
  source_func_caching::RecordLayout header() const;

  /**
   * @brief Looks a point up.
   * @param slot   Read slot (< nReadSlots; concurrent lookups of one slot are serialized).
   * @param layout Layout of the records.
   * @param hash   Hash of the point.
   * @param point  Coordinates (@c layout.nDim).
   * @param values Output: the values on a hit (@c layout.nOutputs).
   * @return Whether the point is stored.
   */
  bool find(size_t slot, source_func_caching::RecordLayout layout, uint64_t hash,
            const double* point, double* values);

  /**
   * @brief Writes records unless the store is @ref full; records of points that are stored
   * already are skipped. Writes the header with the first records.
   * @param records Records @c [hash | point | values] of the layout @p layout; the pointers are
   *                sorted by hash in place.
   * @param layout  Layout of the records (must equal @ref header once it is set).
   * @note Never throws (it is called inside parallel regions): an error is kept and thrown by
   * @ref rethrowError.
   */
  void write(std::span<const uint64_t*> records, source_func_caching::RecordLayout layout);

  /**
   * @brief Calls @p visit for every stored record (@c [hash | point | values], of the layout of
   * @ref header), checking each key against the hash of its point.
   * @param visit Callable receiving the record (valid during the call only).
   * @throws base::file_exception if a record is corrupted or LMDB reports an error.
   */
  void forEachRecord(const std::function<void(const uint64_t* record)>& visit) const;

  /// @brief Writes all committed records through to the disk (@c mdb_env_sync).
  void sync();

  /// @brief Removes all records and the header.
  void clear();

  /// @brief Throws the first error of @ref write since the last call (@c base::file_exception).
  void rethrowError();

  /// @brief Whether the storage space limit (or the disk) is full.
  bool full() const { return full_.load(std::memory_order_relaxed); }

  /// @brief Returns the number of stored records.
  size_t entries() const;

  /// @brief Returns the size of the file in bytes.
  size_t fileBytes() const;

  /// @brief Returns the number of lookups answered by @ref find (since the last reset).
  size_t hits() const { return hits_.load(std::memory_order_relaxed); }

  /// @brief Returns the number of records that were not stored (full store, hash collision).
  size_t rejected() const { return rejected_.load(std::memory_order_relaxed); }

  /// @brief Resets @ref hits and @ref rejected.
  void resetStatistics();

 private:
  LMDBPointValueStore(std::unique_ptr<lmdb_point_value_store::FileInterface> interface,
                      std::string path);

  /// @brief @ref openPersistent without the cleanup of the files it created on errors.
  static std::unique_ptr<LMDBPointValueStore> openPersistentFile(const std::string& path,
                                                                 size_t storageSpaceLimit,
                                                                 size_t nReadSlots);

  /// @brief Marks the store as full and prints the warning (once).
  void reportFull(const char* reason);

  /// @brief Keeps the first error for @ref rethrowError.
  void keepError(const std::string& message);

  std::unique_ptr<lmdb_point_value_store::FileInterface>
      interface_;                      ///< LMDB environment and handles.
  std::string path_;                   ///< Path of the file.
  std::atomic<bool> full_{false};      ///< Whether no further records fit.
  std::atomic<bool> warned_{false};    ///< Whether the warning was printed.
  std::atomic<size_t> hits_{0};        ///< Lookups answered.
  std::atomic<size_t> rejected_{0};    ///< Records not stored.
  std::atomic<bool> hasError_{false};  ///< Whether @c error_ holds an error.
  std::mutex errorMutex_;              ///< Guards @c error_.
  std::string error_;                  ///< First error of @ref write.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
