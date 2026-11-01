#include <lmdb.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include <omp.h>
using Clock = std::chrono::steady_clock;
static double secs(Clock::time_point a) { return std::chrono::duration<double>(Clock::now() - a).count(); }
#define CK(x) do { int rc_ = (x); if (rc_) { fprintf(stderr, "%s: %s\n", #x, mdb_strerror(rc_)); abort(); } } while (0)
int main(int argc, char** argv) {
  std::string dir = argv[1];
  for (size_t valBytes : {48, 168}) {
    for (int sorted = 0; sorted < 2; sorted++) {
      size_t n = 1000000;
      std::mt19937_64 rng(42);
      std::vector<uint64_t> keys(n);
      for (auto& k : keys) k = rng();
      std::vector<uint64_t> order = keys;
      if (sorted) std::sort(order.begin(), order.end());
      std::string path = dir + "/db.mdb";
      MDB_env* env; CK(mdb_env_create(&env)); CK(mdb_env_set_mapsize(env, size_t{16} << 30));
      CK(mdb_env_open(env, path.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOTLS, 0644));
      MDB_dbi dbi; MDB_txn* txn;
      std::vector<char> val(valBytes, 3);
      auto t = Clock::now();
      // batches of 64k records per txn
      for (size_t b = 0; b < n; b += 65536) {
        CK(mdb_txn_begin(env, nullptr, 0, &txn)); CK(mdb_dbi_open(txn, nullptr, MDB_INTEGERKEY, &dbi));
        for (size_t i = b; i < std::min(n, b + 65536); i++) {
          uint64_t k = order[i]; MDB_val kv{8, &k}, vv{valBytes, val.data()};
          CK(mdb_put(txn, dbi, &kv, &vv, 0));
        }
        CK(mdb_txn_commit(txn));
      }
      double w = secs(t);
      // bulk sorted batches in one txn but sorted within batch only (realistic: hook batch sorted)
      // lookups: random existing keys, 1 and 16 threads
      double l1, l16;
      {
        size_t nl = 200000;
        t = Clock::now();
        MDB_txn* rt; CK(mdb_txn_begin(env, nullptr, MDB_RDONLY, &rt));
        double sink = 0;
        for (size_t i = 0; i < nl; i++) { uint64_t k = keys[(i * 7919) % n]; MDB_val kv{8, &k}, vv; CK(mdb_get(rt, dbi, &kv, &vv)); sink += ((char*)vv.mv_data)[0]; }
        mdb_txn_abort(rt);
        l1 = secs(t) * 1e9 / nl;
        t = Clock::now();
#pragma omp parallel num_threads(16) reduction(+:sink)
        {
          MDB_txn* rt2; CK(mdb_txn_begin(env, nullptr, MDB_RDONLY, &rt2));
#pragma omp for
          for (size_t i = 0; i < 16 * nl; i++) { uint64_t k = keys[(i * 7919) % n]; MDB_val kv{8, &k}, vv; CK(mdb_get(rt2, dbi, &kv, &vv)); sink += ((char*)vv.mv_data)[0]; }
          mdb_txn_abort(rt2);
        }
        l16 = secs(t) * 1e9 / (16 * nl);
        // per-lookup txn reset/renew
        t = Clock::now();
        CK(mdb_txn_begin(env, nullptr, MDB_RDONLY, &rt)); mdb_txn_reset(rt);
        for (size_t i = 0; i < nl; i++) { CK(mdb_txn_renew(rt)); uint64_t k = keys[(i * 7919) % n]; MDB_val kv{8, &k}, vv; CK(mdb_get(rt, dbi, &kv, &vv)); sink += ((char*)vv.mv_data)[0]; mdb_txn_reset(rt);}
        mdb_txn_abort(rt);
        printf("  renew+get+reset 1t: %.0f ns\n", secs(t) * 1e9 / nl);
        if (sink == 12345) printf("x");
      }
      MDB_stat st; CK(mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn)); mdb_stat(txn, dbi, &st); mdb_txn_abort(txn);
      size_t bytes = (st.ms_branch_pages + st.ms_leaf_pages + st.ms_overflow_pages) * st.ms_psize;
      printf("lmdb val=%3zuB sorted=%d: write %.0f ns/rec, lookup %.0f ns (1t) %.0f ns wall (16t), %.1f B/rec\n", valBytes, sorted, w * 1e9 / n, l1, l16, (double)bytes / n);
      mdb_env_close(env); unlink(path.c_str()); unlink((path + "-lock").c_str());
    }
  }
}
