#!/bin/bash
# Allocations per single-point call: heaptrack of the driver's alloc probe with N1 and N2 calls,
# (allocs(N2) - allocs(N1)) / (N2 - N1). 16 threads as the judged runs; memory-capped.
cd "/home/jannik/DEV/University/7. Semester/25WT-BT-SGpp"
export LD_LIBRARY_PATH="$PWD/lib:$LD_LIBRARY_PATH"
export OMP_PROC_BIND=close OMP_PLACES=threads OMP_NUM_THREADS=${THREADS:-16}
TMP="${TMPDIR:-/tmp}/wp36-heaptrack"
mkdir -p "$TMP"
count() {  # case n -> number of allocation calls
  local f="$TMP/ht-$(echo "$1" | tr '/' '_')-$2-t$OMP_NUM_THREADS"
  rm -f "$f".*
  systemd-run --user --scope -q -p MemoryMax=8G heaptrack -o "$f" combigrid/benchmarks/perf/perf_interp_explore --filter "$1" --alloc-probe "$2" > /dev/null 2>&1
  heaptrack_print "$(ls "$f".* | head -1)" 2>/dev/null | sed -n 's/^calls to allocation functions: \([0-9]*\).*/\1/p'
}
printf "| case | threads | allocations per call |\n|---|---|---|\n"
for c in interp/sg/cc/b0/d10/L2/p8 interp/sg/cc/b0/d7/L4/p16 interp/sg/cc/b0/d5/L6/p64 interp/sg/cc/b0/d3/L9/p64 interp/sg/cc/b0/d2/L12/p64 interp/sg/cheb1/b0/d5/L4/p64 interp/sg/cheb1/b0/d3/L6/p64 interp/sg/equi/b0/d3/L5/p64 interplin/sg/equi/b1/d10/L5/p32 interplin/sg/equi/b1/d8/L5/p256 interplin/sg/equi/b1/d5/L8/p1024 interplin/sg/equi/b1/d2/L16/p16384; do
  a=$(count "$c" 1); b=$(count "$c" 3)
  echo "| $c | $OMP_NUM_THREADS | $(( (b - a) / 2 )) |"
done
