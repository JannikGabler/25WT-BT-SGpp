#!/bin/bash
# WP-73: builds concurrency_bench.cpp against the snapshots WP-73-before (headers of the base
# commit, without the cache header: lookup variant "a" missing) and WP-73-after (working tree).
# Usage (from the repo root): bash combigrid/perf/results/WP-73/build_bench.sh <out-dir> <base-rev>
set -euo pipefail
OUT="${1:?output directory}"
REV="${2:?base revision}"
SNAP=combigrid/perf/.work/snapshots
mkdir -p "$OUT/before-src"
git archive "$REV" combigrid/src base/src | tar -x -C "$OUT/before-src"
FLAGS=(-std=c++23 -O3 -DNDEBUG -march=native -fopenmp)
g++ "${FLAGS[@]}" -I"$OUT/before-src/base/src" -I"$OUT/before-src/combigrid/src" \
  combigrid/perf/results/WP-73/concurrency_bench.cpp -L"$SNAP/WP-73-before/lib" -lsgppcombigrid \
  -lsgppbase -Wl,-rpath,"$PWD/$SNAP/WP-73-before/lib" -o "$OUT/bench-before"
g++ "${FLAGS[@]}" -DWITH_CACHE_HEADER -Ibase/src -Icombigrid/src \
  combigrid/perf/results/WP-73/concurrency_bench.cpp -L"$SNAP/WP-73-after/lib" -lsgppcombigrid \
  -lsgppbase -Wl,-rpath,"$PWD/$SNAP/WP-73-after/lib" -o "$OUT/bench-after"
