// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_cache_libs.cpp
 * @brief WP-25: cache designs on external libraries that need linking (not built by SCons; see
 * build.sh, judged flags): oneTBB concurrent_hash_map (in-memory index, same cases as
 * perf_cache_proto.hpp) and LMDB, RocksDB, SQLite, HDF5 as disk tiers (write of n records in
 * one batch + sync, random point lookups, bulk load).
 *
 * Keys are the bytes of the d doubles (bitwise user-domain point), values m = 1 double.
 * Directory: $PERF_CACHE_DIR (default /var/tmp, ext4 on the SSD).
 */

#include <H5Cpp.h>
#include <lmdb.h>
#include <omp.h>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/table.h>
#include <rocksdb/write_batch.h>
#include <sqlite3.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "perf_cache_proto.hpp"  // first: configures Boost.Unordered

#include <oneapi/tbb/concurrent_hash_map.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "perf_harness.hpp"

namespace cp = sgpp::combigrid::perf::cache_proto;
namespace perf = sgpp::combigrid::perf;
using sgpp::base::DataVector;
using sgpp::combigrid::perf::doNotOptimize;
using sgpp::combigrid::perf::MicroRun;
using sgpp::combigrid::perf::Suite;

namespace {

/// oneTBB concurrent_hash_map from the 64-bit point hash to a record index (key verified).
class TbbTable {
 public:
  TbbTable(const cp::RecordLayout layout, const size_t capacityHint, size_t /*memLimit*/)
      : layout_(layout), arena_(layout), map_(capacityHint) {}

  bool find(const DataVector& x, const uint64_t hash, double* out) {
    Map::const_accessor acc;
    if (!map_.find(acc, hash)) return false;
    const uint64_t* record = arena_.record(acc->second);
    if (!cp::recordMatches(record, layout_, cp::Probe{x.data(), hash})) return false;
    std::memcpy(out, record + 1 + layout_.d, 8 * layout_.m);
    return true;
  }

  bool insert(const DataVector& x, const uint64_t hash, const double* value) {
    Map::accessor acc;
    if (!map_.insert(acc, hash)) return false;  // present (or a 64-bit hash collision)
    const uint32_t idx = arena_.allocate();
    cp::writeRecord(arena_.record(idx), layout_, x.data(), hash, value);
    acc->second = idx;
    return true;
  }

  size_t size() const { return map_.size(); }

 private:
  struct HashCompare {
    static size_t hash(const uint64_t k) { return k; }
    static bool equal(const uint64_t a, const uint64_t b) { return a == b; }
  };
  using Map = oneapi::tbb::concurrent_hash_map<uint64_t, uint32_t, HashCompare>;

  cp::RecordLayout layout_;
  cp::ConcurrentArena arena_;
  Map map_;
};

/// Keys (d doubles each, contiguous) and random query order of the store cases.
struct StoreData {
  size_t nDim;
  size_t n;
  std::vector<double> keys;
  std::vector<uint32_t> queries;
  static std::shared_ptr<StoreData> get(const size_t nDim, const size_t n) {
    static std::shared_ptr<StoreData> current;
    if (!current || current->nDim != nDim || current->n != n) {
      current.reset();
      current = std::make_shared<StoreData>();
      current->nDim = nDim;
      current->n = n;
      current->keys = cp::distinctGridPoints(nDim, n);
      perf::Rng rng(11);
      current->queries.resize(4096);
      for (uint32_t& q : current->queries) q = static_cast<uint32_t>(rng.index(n));
    }
    return current;
  }
  const double* key(const size_t i) const { return keys.data() + i * nDim; }
  size_t keyBytes() const { return 8 * nDim; }
};

std::string storePath(const std::string& lib, const size_t nDim, const size_t n) {
  return cp::diskDir() + "/sgpp_perf_cache_" + lib + "_" + std::to_string(nDim) + "_" +
         std::to_string(n);
}

uintmax_t diskBytes(const std::string& path) {
  namespace fs = std::filesystem;
  if (fs::is_regular_file(path)) return fs::file_size(path);
  uintmax_t sum = 0;
  for (const auto& e : fs::recursive_directory_iterator(path)) {
    if (e.is_regular_file()) sum += e.file_size();
  }
  return sum;
}

/// One store: write all records (one batch + sync), random lookups (per thread), bulk load.
template <typename Store>
void addStoreCases(Suite& suite, const std::string& lib, const size_t nDim, const size_t n) {
  const std::string tag = lib + "/" + cp::dn(nDim, n);
  suite.addMicro("micro/cache/store/write_sync/" + tag, [=](MicroRun& run) {
    const auto data = StoreData::get(nDim, n);
    const std::string path = storePath(lib, nDim, n);
    run.setItems(static_cast<double>(n));
    std::string onDisk;
    run.measure([&]() {
      std::filesystem::remove_all(path);
      Store store(path, nDim, true);
      store.writeAll(*data);
      onDisk = store.diskPath();
    });
    run.setCounter("disk_bytes_per_record",
                   static_cast<double>(diskBytes(onDisk)) / static_cast<double>(n));
  });
  suite.addMicro("micro/cache/store/lookup/" + tag, [=](MicroRun& run) {
    const auto data = StoreData::get(nDim, n);
    const std::string path = storePath(lib, nDim, n);
    Store store(path, nDim, false);
    run.setItems(static_cast<double>(data->queries.size()));
    run.setCounter("threads", omp_get_max_threads());
    size_t found = 0;
    run.measure([&]() {
      size_t hits = 0;
#pragma omp parallel reduction(+ : hits)
      {
        auto reader = store.reader();
#pragma omp for schedule(static)
        for (size_t q = 0; q < data->queries.size(); q++) {
          double value;
          hits += reader.get(data->key(data->queries[q]), data->keyBytes(), &value) ? 1 : 0;
        }
      }
      found = hits;
    });
    run.setCounter("hit_fraction",
                   static_cast<double>(found) / static_cast<double>(data->queries.size()));
  });
  suite.addMicro("micro/cache/store/load_all/" + tag, [=](MicroRun& run) {
    const auto data = StoreData::get(nDim, n);
    Store store(storePath(lib, nDim, n), nDim, false);
    run.setItems(static_cast<double>(n));
    run.measure([&]() { doNotOptimize(store.scanAll()); });
  });
}

void check(const int rc, const char* what) {
  if (rc != 0) {
    std::fprintf(stderr, "%s failed: %d\n", what, rc);
    std::exit(1);
  }
}

/// LMDB (mmap B+ tree, one writer, lock-free readers).
class LmdbStore {
 public:
  LmdbStore(const std::string& path, size_t /*nDim*/, const bool create) : path_(path) {
    if (create) std::filesystem::create_directories(path);
    check(mdb_env_create(&env_), "mdb_env_create");
    check(mdb_env_set_mapsize(env_, size_t{4} << 30), "mdb_env_set_mapsize");
    check(mdb_env_set_maxreaders(env_, 64), "mdb_env_set_maxreaders");
    check(mdb_env_open(env_, path.c_str(), MDB_NOTLS, 0644), "mdb_env_open");
    MDB_txn* txn;
    check(mdb_txn_begin(env_, nullptr, 0, &txn), "mdb_txn_begin");
    check(mdb_dbi_open(txn, nullptr, 0, &dbi_), "mdb_dbi_open");
    check(mdb_txn_commit(txn), "mdb_txn_commit");
  }
  ~LmdbStore() { mdb_env_close(env_); }
  std::string diskPath() const { return path_; }

  void writeAll(const StoreData& data) {
    MDB_txn* txn;
    check(mdb_txn_begin(env_, nullptr, 0, &txn), "mdb_txn_begin");
    for (size_t i = 0; i < data.n; i++) {
      double value = static_cast<double>(i);
      MDB_val k{data.keyBytes(), const_cast<double*>(data.key(i))};
      MDB_val v{sizeof(double), &value};
      check(mdb_put(txn, dbi_, &k, &v, 0), "mdb_put");
    }
    check(mdb_txn_commit(txn), "mdb_txn_commit");  // durable (default sync)
  }

  class Reader {
   public:
    Reader(MDB_env* env, MDB_dbi dbi) : dbi_(dbi) {
      check(mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn_), "mdb_txn_begin(ro)");
    }
    ~Reader() { mdb_txn_abort(txn_); }
    bool get(const double* key, const size_t bytes, double* out) {
      MDB_val k{bytes, const_cast<double*>(key)};
      MDB_val v;
      if (mdb_get(txn_, dbi_, &k, &v) != 0) return false;
      std::memcpy(out, v.mv_data, sizeof(double));
      return true;
    }

   private:
    MDB_txn* txn_;
    MDB_dbi dbi_;
  };
  Reader reader() { return Reader(env_, dbi_); }

  size_t scanAll() {
    MDB_txn* txn;
    check(mdb_txn_begin(env_, nullptr, MDB_RDONLY, &txn), "mdb_txn_begin(ro)");
    MDB_cursor* cur;
    check(mdb_cursor_open(txn, dbi_, &cur), "mdb_cursor_open");
    MDB_val k, v;
    size_t n = 0;
    double sum = 0;
    while (mdb_cursor_get(cur, &k, &v, n == 0 ? MDB_FIRST : MDB_NEXT) == 0) {
      double value;
      std::memcpy(&value, v.mv_data, sizeof(double));
      sum += value;
      n++;
    }
    mdb_cursor_close(cur);
    mdb_txn_abort(txn);
    doNotOptimize(sum);
    return n;
  }

 private:
  std::string path_;
  MDB_env* env_;
  MDB_dbi dbi_;
};

/// RocksDB (LSM tree; bloom filter 10 bits/key for point lookups).
class RocksStore {
 public:
  RocksStore(const std::string& path, size_t /*nDim*/, const bool create) : path_(path) {
    rocksdb::Options options;
    options.create_if_missing = create;
    options.IncreaseParallelism(8);
    rocksdb::BlockBasedTableOptions table;
    table.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10));
    options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(table));
    rocksdb::DB* db;
    const rocksdb::Status s = rocksdb::DB::Open(options, path, &db);
    if (!s.ok()) {
      std::fprintf(stderr, "rocksdb open: %s\n", s.ToString().c_str());
      std::exit(1);
    }
    db_.reset(db);
  }

  std::string diskPath() const { return path_; }

  void writeAll(const StoreData& data) {
    rocksdb::WriteBatch batch;
    for (size_t i = 0; i < data.n; i++) {
      const double value = static_cast<double>(i);
      batch.Put(rocksdb::Slice(reinterpret_cast<const char*>(data.key(i)), data.keyBytes()),
                rocksdb::Slice(reinterpret_cast<const char*>(&value), sizeof(double)));
    }
    rocksdb::WriteOptions wo;
    wo.sync = true;
    db_->Write(wo, &batch);
    db_->Flush(rocksdb::FlushOptions());  // memtable -> SST file
  }

  class Reader {
   public:
    explicit Reader(rocksdb::DB* db) : db_(db) {}
    bool get(const double* key, const size_t bytes, double* out) {
      if (!db_->Get(rocksdb::ReadOptions(),
                    rocksdb::Slice(reinterpret_cast<const char*>(key), bytes), &value_)
               .ok()) {
        return false;
      }
      std::memcpy(out, value_.data(), sizeof(double));
      return true;
    }

   private:
    rocksdb::DB* db_;
    std::string value_;
  };
  Reader reader() { return Reader(db_.get()); }

  size_t scanAll() {
    std::unique_ptr<rocksdb::Iterator> it(db_->NewIterator(rocksdb::ReadOptions()));
    size_t n = 0;
    double sum = 0;
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      double value;
      std::memcpy(&value, it->value().data(), sizeof(double));
      sum += value;
      n++;
    }
    doNotOptimize(sum);
    return n;
  }

 private:
  std::string path_;
  std::unique_ptr<rocksdb::DB> db_;
};

/// SQLite (B tree, WAL; one connection per reader thread).
class SqliteStore {
 public:
  SqliteStore(const std::string& path, size_t /*nDim*/, const bool create) : path_(path + ".db") {
    if (create) std::filesystem::remove(path_);
    check(sqlite3_open_v2(path_.c_str(), &db_,
                          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
                          nullptr),
          "sqlite3_open");
    exec("PRAGMA journal_mode=WAL");
    exec("CREATE TABLE IF NOT EXISTS cache (k BLOB PRIMARY KEY, v REAL) WITHOUT ROWID");
  }
  ~SqliteStore() { sqlite3_close(db_); }
  std::string diskPath() const { return path_; }

  void writeAll(const StoreData& data) {
    exec("PRAGMA synchronous=FULL");
    exec("BEGIN");
    sqlite3_stmt* st;
    check(sqlite3_prepare_v2(db_, "INSERT OR IGNORE INTO cache VALUES (?, ?)", -1, &st, nullptr),
          "prepare");
    for (size_t i = 0; i < data.n; i++) {
      sqlite3_bind_blob(st, 1, data.key(i), static_cast<int>(data.keyBytes()), SQLITE_STATIC);
      sqlite3_bind_double(st, 2, static_cast<double>(i));
      sqlite3_step(st);
      sqlite3_reset(st);
    }
    sqlite3_finalize(st);
    exec("COMMIT");
  }

  class Reader {
   public:
    explicit Reader(const std::string& path) {
      check(sqlite3_open_v2(path.c_str(), &db_, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX,
                            nullptr),
            "sqlite3_open(ro)");
      check(sqlite3_prepare_v2(db_, "SELECT v FROM cache WHERE k = ?", -1, &st_, nullptr),
            "prepare");
    }
    Reader(Reader&& other) noexcept : db_(other.db_), st_(other.st_) {
      other.db_ = nullptr;
      other.st_ = nullptr;
    }
    ~Reader() {
      if (st_) sqlite3_finalize(st_);
      if (db_) sqlite3_close(db_);
    }
    bool get(const double* key, const size_t bytes, double* out) {
      sqlite3_bind_blob(st_, 1, key, static_cast<int>(bytes), SQLITE_STATIC);
      const bool found = sqlite3_step(st_) == SQLITE_ROW;
      if (found) *out = sqlite3_column_double(st_, 0);
      sqlite3_reset(st_);
      return found;
    }

   private:
    sqlite3* db_ = nullptr;
    sqlite3_stmt* st_ = nullptr;
  };
  Reader reader() { return Reader(path_); }

  size_t scanAll() {
    sqlite3_stmt* st;
    check(sqlite3_prepare_v2(db_, "SELECT k, v FROM cache", -1, &st, nullptr), "prepare");
    size_t n = 0;
    double sum = 0;
    while (sqlite3_step(st) == SQLITE_ROW) {
      sum += sqlite3_column_double(st, 1);
      n++;
    }
    sqlite3_finalize(st);
    doNotOptimize(sum);
    return n;
  }

 private:
  void exec(const char* sql) { check(sqlite3_exec(db_, sql, nullptr, nullptr, nullptr), sql); }
  std::string path_;
  sqlite3* db_ = nullptr;
};

/// HDF5: records as one n x (d + 1) dataset (no point lookups: bulk write / read only).
void addHdf5Cases(Suite& suite, const size_t nDim, const size_t n) {
  const std::string tag = "hdf5/" + cp::dn(nDim, n);
  const std::string path = storePath("hdf5", nDim, n) + ".h5";
  suite.addMicro("micro/cache/store/write_sync/" + tag, [=](MicroRun& run) {
    const auto data = StoreData::get(nDim, n);
    std::vector<double> rows(n * (nDim + 1));
    for (size_t i = 0; i < n; i++) {
      std::memcpy(rows.data() + i * (nDim + 1), data->key(i), data->keyBytes());
      rows[i * (nDim + 1) + nDim] = static_cast<double>(i);
    }
    run.setItems(static_cast<double>(n));
    run.measure([&]() {
      H5::H5File file(path, H5F_ACC_TRUNC);
      const hsize_t dims[2] = {n, nDim + 1};
      H5::DataSpace space(2, dims);
      H5::DataSet ds = file.createDataSet("records", H5::PredType::NATIVE_DOUBLE, space);
      ds.write(rows.data(), H5::PredType::NATIVE_DOUBLE);
      file.flush(H5F_SCOPE_GLOBAL);
      const int fd = ::open(path.c_str(), O_RDONLY);
      ::fsync(fd);
      ::close(fd);
    });
    run.setCounter("disk_bytes_per_record",
                   static_cast<double>(diskBytes(path)) / static_cast<double>(n));
  });
  suite.addMicro("micro/cache/store/load_all/" + tag, [=](MicroRun& run) {
    std::vector<double> rows(n * (nDim + 1));
    run.setItems(static_cast<double>(n));
    run.measure([&]() {
      H5::H5File file(path, H5F_ACC_RDONLY);
      file.openDataSet("records").read(rows.data(), H5::PredType::NATIVE_DOUBLE);
      doNotOptimize(rows[n - 1]);
    });
  });
}

}  // namespace

int main(int argc, char** argv) {
  Suite suite("micro", 3, 0);
  for (const size_t nDim : {2, 10, 20}) {
    for (const size_t n : {10'000, 1'000'000}) {
      cp::addTableCases<TbbTable>(suite, "tbb_chm", nDim, n);
    }
  }
  for (const size_t nDim : {2, 20}) {
    const size_t n = 1'000'000;
    addStoreCases<LmdbStore>(suite, "lmdb", nDim, n);
    addStoreCases<RocksStore>(suite, "rocksdb", nDim, n);
    addStoreCases<SqliteStore>(suite, "sqlite", nDim, n);
    addHdf5Cases(suite, nDim, n);
  }
  return suite.main(argc, argv);
}
