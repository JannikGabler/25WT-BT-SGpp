#!/bin/bash
# WP-27 (copy of WP-40, plus CachePolicy::Adaptive in the messages): builds combigrid (debug, OPT=0) with each SGPP_COMBIGRID_DISABLE_* configuration, runs the
# full Boost suite (SCons runs it), prints the dependency_exception messages of the cache and its
# storage policies (user program against the public header only) and counts the undefined mdb_
# symbols of libsgppcombigrid.so.
# Usage (from the repo root): bash combigrid/perf/results/WP-27/nolib_matrix.sh <out-dir>
# Afterwards the tree holds the last of these builds: rebuild with perf.py before benchmarking.
set -uo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
cat > "$OUT/reason.cpp" <<'EOF'
#include <exception>
#include <iostream>
#include <sgpp_combigrid.hpp>

int main() {
  using sgpp::combigrid::CachePolicy;
  using sgpp::combigrid::SourceFuncCache;
  using sgpp::combigrid::SourceFuncCacheConfig;
  for (const CachePolicy policy :
       {CachePolicy::MemoryOnly, CachePolicy::TemporaryStorage, CachePolicy::Adaptive}) {
    try {
      const SourceFuncCache cache(SourceFuncCacheConfig{.policy = policy});
      std::cout << "  policy " << static_cast<int>(policy) << ": created\n";
    } catch (const std::exception& e) {
      std::cout << "  policy " << static_cast<int>(policy) << ": " << e.what() << "\n";
    }
  }
}
EOF
for flags in "-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE" "-DSGPP_COMBIGRID_DISABLE_LMDB_STORE" \
             "-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE -DSGPP_COMBIGRID_DISABLE_LMDB_STORE"; do
  echo "=== CPPFLAGS=$flags"
  scons -j16 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 OPT=0 CPPFLAGS="$flags" > "$OUT/build.log" 2>&1
  echo "scons exit $?; boost tests: $(grep -cE '\*\*\* No errors detected' "$OUT/build.log") x 'No errors detected'," \
    "$(grep -cE 'failure|error: ' "$OUT/build.log") failure/error lines"
  # The user program is compiled WITHOUT the flags: the public headers must not depend on them.
  g++ -std=c++23 -fopenmp -Ibase/src -Icombigrid/src "$OUT/reason.cpp" -Llib -lsgppcombigrid \
    -lsgppbase -Wl,-rpath,"$PWD/lib" -o "$OUT/reason" && "$OUT/reason"
  echo "  undefined mdb_ symbols: $(nm -D --undefined-only lib/libsgppcombigrid.so | grep -c ' mdb_')"
done
