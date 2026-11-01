// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <fcntl.h>
#include <lmdb.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_header.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>
#include <sgpp/combigrid/miscellaneous/exceptions/cache_file_exception.hpp>
#include <sgpp/combigrid/tools/caching/lmdb_tools.hpp>

using namespace sgpp::combigrid::tools::lmdb;
using namespace sgpp::combigrid::constants::lmdb_point_value_store;
using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

FileInterface::~FileInterface() {
  for (size_t i = 0; i < nSlots; i++) {
    if (slots[i].txn != nullptr) mdb_txn_abort(slots[i].txn);
  }
  if (env != nullptr) mdb_env_close(env);
  if (lockFd >= 0) close(lockFd);  // releases the flock
}

/// Opens the environment (file @p path), its sub-databases and the read slots.
void FileInterface::open(const std::string& path, const size_t storageSpaceLimit,
                         const size_t nReadSlots, const size_t existingBytes) {
  int rc = mdb_env_create(&env);
  if (rc == 0) rc = mdb_env_set_maxdbs(env, 2);
  if (rc == 0) rc = mdb_env_set_maxreaders(env, static_cast<unsigned int>(nReadSlots + 8));
  if (rc == 0) {
    rc = mdb_env_set_mapsize(env, mapSize(storageSpaceLimit, directoryOf(path), existingBytes));
  }
  if (rc != 0) throw CacheFileException(lmdbMessage("cannot set up", path, rc));

  rc = mdb_env_open(env, path.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOTLS | MDB_NORDAHEAD, 0644);
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
void FileInterface::checkSize(const std::string& path) const {
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
void FileInterface::openDatabases(const std::string& path) {
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

void FileInterface::readHeader(MDB_txn* txn, const std::string& path) {
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
                             std::to_string(h.keyScheme) + ", expected " + std::to_string(VERSION) +
                             " / " + std::to_string(KEY_SCHEME));
  }
  header = RecordLayout{h.nDim, h.nOutputs};
}

/// Writes the header in @p txn (the caller holds LMDB's writer lock).
int FileInterface::writeHeader(MDB_txn* txn, const source_func_caching::RecordLayout layout) const {
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

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
