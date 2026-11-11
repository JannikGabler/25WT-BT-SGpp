#!/bin/bash
# Profiles the interp / interplin e2e cases of snapshot WP-36 and splits them (RP-3 method).
set -u
cd "/home/jannik/DEV/University/7. Semester/25WT-BT-SGpp"
source .venv/bin/activate
OUT=combigrid/perf/results/WP-36/profiles
LIB=combigrid/perf/.work/snapshots/WP-36/lib
run() {  # case threads samples
  local c="$1" t="$2" s="$3"
  local slug=$(echo "$c" | tr '/' '_')
  python3 combigrid/perf/scripts/perf.py profile "^$c\$" --snapshot WP-36 --threads "$t" --samples "$s" > "$OUT/$slug-t$t.txt" 2>&1
  local data=$(sed -n 's/^perf data: \(.*\.data\) (inspect.*/\1/p' "$OUT/$slug-t$t.txt")
  .venv/bin/python combigrid/perf/results/RP-3/profile_split.py "$data" "$LIB" 30 > "$OUT/$slug-t$t-split.txt" 2>&1
  echo "done $c t$t"
}
for c in interp/sg/cc/b0/d10/L2/p8 interp/sg/cc/b0/d7/L4/p16 interp/sg/cc/b0/d5/L6/p64 interp/sg/cc/b0/d3/L9/p64 interp/sg/cc/b0/d2/L12/p64 interp/sg/cheb1/b0/d5/L4/p64 interp/sg/cheb1/b0/d3/L6/p64 interp/sg/equi/b0/d3/L5/p64 interplin/sg/equi/b1/d10/L5/p32 interplin/sg/equi/b1/d8/L5/p256 interplin/sg/equi/b1/d5/L8/p1024 interplin/sg/equi/b1/d2/L16/p16384; do
  run "$c" 16 2
done
for c in interp/sg/cc/b0/d10/L2/p8 interp/sg/cc/b0/d7/L4/p16 interp/sg/cc/b0/d5/L6/p64 interplin/sg/equi/b1/d10/L5/p32 interplin/sg/equi/b1/d8/L5/p256; do
  run "$c" 1 1
done
