#!/usr/bin/env bash
# RP-6: builds tg_sizes.cpp against the in-tree headers and the rp-6 snapshot libraries and runs it.
# Usage (repo root): bash combigrid/perf/results/RP-6/build_tg_sizes.sh OUT_DIR
set -euo pipefail
OUT="$1"; mkdir -p "$OUT"
SNAP=combigrid/perf/.work/snapshots/rp-6/lib
g++ -std=c++23 -O2 -fopenmp -I combigrid/src -I base/src -I combigrid/benchmarks/perf \
  combigrid/perf/results/RP-6/tg_sizes.cpp -L "$SNAP" -lsgppcombigrid -lsgppbase \
  -o "$OUT/tg_sizes"
LD_LIBRARY_PATH="$SNAP" OMP_NUM_THREADS=16 "$OUT/tg_sizes" > combigrid/perf/results/RP-6/tg-sizes.jsonl
echo "wrote combigrid/perf/results/RP-6/tg-sizes.jsonl"
g++ -std=c++23 -O2 -fopenmp -I combigrid/src -I base/src -I combigrid/benchmarks/perf \
  combigrid/perf/results/RP-6/cache_bytes.cpp -L "$SNAP" -lsgppcombigrid -lsgppbase \
  -o "$OUT/cache_bytes"
LD_LIBRARY_PATH="$SNAP" OMP_NUM_THREADS=16 OMP_PROC_BIND=close OMP_PLACES=threads "$OUT/cache_bytes" \
  > combigrid/perf/results/RP-6/cache-bytes.md
echo "wrote combigrid/perf/results/RP-6/cache-bytes.md"
