// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file lmdb_point_value_store_file_interface.hpp
 * @brief LMDB environment, handles and read slots of a @c misc::LMDBPointValueStore (its pimpl).
 *
 * Internal header: it includes @c lmdb.h and its definition depends on the configuration
 * (@c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE); only the implementation of the store
 * includes it, the store's header declares the type only.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <string>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <lmdb.h>

#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_read_slot.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/**
 * @brief The LMDB side of a @c LMDBPointValueStore: the environment (one file,
 * @c MDB_NOSUBDIR), the sub-databases @c records and @c meta, the read slots, the lock of a
 * persistent file and the cached header.
 *
 * Created empty by the factories of the store, which lock the file (persistent) and call
 * @ref open; the store uses the handles directly. The destructor releases everything that was
 * acquired, also after a failed @ref open.
 */
struct FileInterface {
  /// @brief Aborts the read transactions, closes the environment and releases the lock.
  ~FileInterface();

  /**
   * @brief Opens the environment, its sub-databases (@ref openDatabases) and the read slots.
   * @param path              File of the store (created by LMDB if missing).
   * @param storageSpaceLimit Maximal size of the file in bytes (map size, see
   *                          @c tools::lmdb::mapSize).
   * @param nReadSlots        Number of read slots.
   * @param existingBytes     Size of the existing file (0 for a new one).
   * @throws base::file_exception if LMDB cannot open the file, it is no store of the
   * source-function cache, truncated (@ref checkSize) or has another header.
   */
  void open(const std::string& path, size_t storageSpaceLimit, size_t nReadSlots,
            size_t existingBytes);

  /**
   * @brief Throws if the file is shorter than its last page (a truncated file would crash LMDB).
   * @param path File of the store (for the message).
   * @throws base::file_exception if the file is truncated.
   */
  void checkSize(const std::string& path) const;

  /**
   * @brief Opens (or creates) the sub-databases and reads and checks the header.
   * @param path File of the store (for the messages).
   * @throws base::file_exception if the file holds another LMDB database or a bad header.
   */
  void openDatabases(const std::string& path);

  /**
   * @brief Reads the header from @c meta into @ref header and checks it (magic, format version,
   * key scheme); a file without header must hold no records.
   * @param txn  Transaction of @ref openDatabases.
   * @param path File of the store (for the messages).
   * @throws base::file_exception if the header is missing, corrupted or of another version.
   */
  void readHeader(MDB_txn* txn, const std::string& path);

  /**
   * @brief Writes the header for records of @p layout in @p txn (the caller holds LMDB's
   * writer lock).
   * @param txn    Write transaction.
   * @param layout Layout of the records.
   * @return Return code of @c mdb_put.
   */
  int writeHeader(MDB_txn* txn, source_func_caching::RecordLayout layout) const;

  /// @name Attributes
  /// @{

  MDB_env* env = nullptr;             ///< The environment (nullptr until @ref open).
  MDB_dbi records = 0;                ///< Sub-database of the records (key: hash of the point).
  MDB_dbi meta = 0;                   ///< Sub-database of the header.
  int lockFd = -1;                    ///< Descriptor holding the flock of a persistent file.
  size_t nSlots = 0;                  ///< Number of read slots.
  std::unique_ptr<ReadSlot[]> slots;  ///< Read slots (one per shard of the table).
  mutable std::mutex headerMutex;     ///< Guards @c header.
  source_func_caching::RecordLayout header{0, 0};  ///< Layout of the header ({0, 0}: none).

  /// @}
};

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/// @brief Without LMDB no store is ever opened: an empty definition, so that the store's
/// stubs compile.
struct FileInterface {};

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
