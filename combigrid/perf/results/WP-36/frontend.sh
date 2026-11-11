#!/bin/bash
# perf stat front-end / op-cache counters of the large interpolation cases (snapshot WP-36, 16 t).
cd "/home/jannik/DEV/University/7. Semester/25WT-BT-SGpp"
SNAP=combigrid/perf/.work/snapshots/WP-36
OUT=combigrid/perf/results/WP-36/perfstat-frontend.txt
: > "$OUT"
for c in interp/sg/cc/b0/d10/L2/p8 interp/sg/cc/b0/d7/L4/p16 interp/sg/cc/b0/d5/L6/p64 interplin/sg/equi/b1/d10/L5/p32 interplin/sg/equi/b1/d8/L5/p256; do
  echo "### $c" >> "$OUT"
  LD_LIBRARY_PATH="$SNAP/lib" OMP_PROC_BIND=close OMP_PLACES=threads OMP_NUM_THREADS=16 \
    perf stat -x, -e instructions,cycles,op_cache_hit_miss.all_op_cache_accesses,op_cache_hit_miss.op_cache_miss,ic_tag_hit_miss.instruction_cache_miss,stalled-cycles-frontend \
    "$SNAP/bin/perf_e2e" --filter "^$c\$" --samples 2 --warmups 1 2>&1 >/dev/null | grep -v "^$" | cut -d, -f1,3 >> "$OUT"
done
cat "$OUT"
