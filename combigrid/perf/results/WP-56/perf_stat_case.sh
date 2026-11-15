#!/bin/bash
# WP-56: perf stat (instructions, cycles, op-cache accesses / misses, user / sys time) of one e2e case
# per snapshot, 16 threads, 1 warm-up + 1 sample, N processes. Usage (repo root):
#   bash perf_stat_case.sh CASE N SNAP [SNAP ...]
case=$1; n=$2; shift 2
for snap in "$@"; do
  for i in $(seq "$n"); do
    OMP_NUM_THREADS=16 OMP_PROC_BIND=close OMP_PLACES=threads \
    LD_LIBRARY_PATH="combigrid/perf/.work/snapshots/$snap/lib" \
      perf stat -x , -e instructions,cycles,op_cache_hit_miss.all_op_cache_accesses,op_cache_hit_miss.op_cache_miss,task-clock \
      -- "combigrid/perf/.work/snapshots/$snap/bin/perf_e2e" --filter "^${case}\$" --samples 1 --warmups 1 \
      2>&1 >/dev/null | awk -F, -v s="$snap" '{printf "%s %s %s\n", s, $3, $1}'
  done
done
