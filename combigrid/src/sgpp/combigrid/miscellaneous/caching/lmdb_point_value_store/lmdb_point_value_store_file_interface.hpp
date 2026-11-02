// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <fcntl.h>
#include <lmdb.h>

#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_read_slot.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/**
 * TODO: Document
 */
struct FileInterface {
  ~FileInterface();

  /// Opens the environment (file @p path), its sub-databases and the read slots.
  void open(const std::string& path, size_t storageSpaceLimit, size_t nReadSlots,
            size_t existingBytes);

  /// Throws if the file is shorter than its last page (a truncated file would crash LMDB).
  void checkSize(const std::string& path) const;

  /// Opens the sub-databases and reads and checks the header.
  void openDatabases(const std::string& path);

  void readHeader(MDB_txn* txn, const std::string& path);

  /// Writes the header in @p txn (the caller holds LMDB's writer lock).
  int writeHeader(MDB_txn* txn, const source_func_caching::RecordLayout layout) const;

  /// @name Attributes
  /// @{

  MDB_env* env = nullptr;
  MDB_dbi records = 0;
  MDB_dbi meta = 0;
  int lockFd = -1;  ///< Descriptor holding the flock of a persistent file.
  size_t nSlots = 0;
  std::unique_ptr<ReadSlot[]> slots;
  mutable std::mutex headerMutex;
  source_func_caching::RecordLayout header{0, 0};  ///< Guarded by headerMutex ({0, 0}: none).

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

// Empty definition to let the library link.
struct FileInterface {};

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
