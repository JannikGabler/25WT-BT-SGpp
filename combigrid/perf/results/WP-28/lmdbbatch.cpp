#include <lmdb.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>
using Clock = std::chrono::steady_clock;
#define CK(x) do { int rc_ = (x); if (rc_) { fprintf(stderr, "%s: %s\n", #x, mdb_strerror(rc_)); abort(); } } while (0)
int main(int argc, char** argv) {
  std::string path = std::string(argv[1]) + "/batch.mdb";
  for (unsigned extra : {0u, (unsigned)MDB_WRITEMAP}) {
    for (size_t existing : {100000ul, 1000000ul}) {
      for (size_t batch : {256ul, 4096ul, 65536ul}) {
        for (int sorted = 0; sorted < 2; sorted++) {
          unlink(path.c_str()); unlink((path + "-lock").c_str());
          MDB_env* env; CK(mdb_env_create(&env)); CK(mdb_env_set_mapsize(env, size_t{4} << 30));
          CK(mdb_env_open(env, path.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOTLS | extra, 0644));
          MDB_dbi dbi; MDB_txn* txn; std::vector<char> val(48, 1);
          std::mt19937_64 rng(1);
          std::vector<uint64_t> keys(existing); for (auto& k : keys) k = rng();
          std::sort(keys.begin(), keys.end());
          CK(mdb_txn_begin(env, nullptr, 0, &txn)); CK(mdb_dbi_open(txn, nullptr, MDB_INTEGERKEY, &dbi));
          for (auto k : keys) { MDB_val kv{8, &k}, vv{48, val.data()}; CK(mdb_put(txn, dbi, &kv, &vv, MDB_APPEND)); }
          CK(mdb_txn_commit(txn));
          size_t n = 100000; std::vector<uint64_t> nk(n); for (auto& k : nk) k = rng();
          auto t = Clock::now();
          for (size_t b = 0; b < n; b += batch) {
            size_t e = std::min(n, b + batch);
            if (sorted) std::sort(nk.begin() + b, nk.begin() + e);
            CK(mdb_txn_begin(env, nullptr, 0, &txn));
            for (size_t i = b; i < e; i++) { MDB_val kv{8, &nk[i]}, vv{48, val.data()}; CK(mdb_put(txn, dbi, &kv, &vv, MDB_NOOVERWRITE)); }
            CK(mdb_txn_commit(txn));
          }
          double s = std::chrono::duration<double>(Clock::now() - t).count();
          printf("writemap=%d existing=%7zu batch=%6zu sorted=%d: %6.0f ns/rec\n", extra ? 1 : 0, existing, batch, sorted, s * 1e9 / n);
          mdb_env_close(env);
        }
      }
    }
  }
  unlink(path.c_str()); unlink((path + "-lock").c_str());
}
