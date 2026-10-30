#!/usr/bin/env bash
# WP-25: builds perf_cache_libs (not part of the SCons build: needs -ltbb -llmdb -lrocksdb
# -lsqlite3 -lhdf5) with the judged flags (perf.py build normal) against the in-tree lib/.
# Run from anywhere; the binary lands next to this script.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../../../.." && pwd)"
g++ -std=c++23 -O3 -DNDEBUG -march=native -mavx2 -mfma -mfpmath=sse -flto=auto -funroll-loops \
    -fno-semantic-interposition -fno-omit-frame-pointer -g -fopenmp -fPIC -Wall -Wextra \
    -I"$repo/base/src" -I"$repo/combigrid/src" -I"$repo/combigrid/benchmarks/perf" \
    -I/usr/include/hdf5/serial \
    "$here/perf_cache_libs.cpp" -o "$here/perf_cache_libs" \
    -L"$repo/lib" -lsgppcombigrid -lsgppbase \
    -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_cpp -lhdf5 \
    -ltbb -llmdb -lrocksdb -lsqlite3 -fopenmp -flto=auto
