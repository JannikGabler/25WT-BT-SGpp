#!/bin/bash
# Runs perf_adaptive of the in-tree normal build (perf.py build normal) at 16 threads (all
# schedules) and at 1 thread (sync only: the reference of the speedups); JSON lines to <out-dir>.
# Usage (repo root, idle machine): bash combigrid/perf/results/WP-89/run_adaptive.sh <out-dir> [samples]
set -euo pipefail
OUT="${1:?output directory}"
SAMPLES="${2:-5}"
mkdir -p "$OUT"
BIN=combigrid/benchmarks/perf/perf_adaptive
export LD_LIBRARY_PATH="$PWD/lib"
OMP_PROC_BIND=close OMP_PLACES=threads "$BIN" --threads 16 --samples "$SAMPLES" --warmups 1 --json \
  > "$OUT/adaptive_t16.jsonl"
OMP_PROC_BIND=close OMP_PLACES='{4}' "$BIN" --threads 1 --samples 3 --warmups 0 --json \
  --filter '/sync$' > "$OUT/adaptive_t1.jsonl"
