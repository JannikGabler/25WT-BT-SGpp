#!/usr/bin/env bash
# RP-4: perf.py profile (16 threads, self time) of the top cases per group on snapshot rp-4, then
# the inline-chain split (RP-3 method, ../RP-3/profile_split.py). Cases shorter than ~0.5 s get more
# samples so that every profile covers >= ~1 s of the measured pipeline.
# Usage (repo root): bash combigrid/perf/results/RP-4/run_profiles.sh [CASE:SAMPLES ...]
set -euo pipefail
PERF="python3 combigrid/perf/scripts/perf.py"
OUT=combigrid/perf/results/RP-4/profiles
LIB=combigrid/perf/.work/snapshots/rp-4/lib
mkdir -p "$OUT"
CASES=("$@")
if [ ${#CASES[@]} -eq 0 ]; then
  CASES=(
    quad/dsg/cc-uq/b0/d10/L7:2 quad/dsg/equi-exp/b0/d10/L7:2 quad/dsg/cc-uq/b0/d7/L10:2
    quad/dsg/equi-exp/b0/d7/L10:2 quad/dsg/equi-exp/b1/d20/L6:2 quad/dsg/equi-exp/b1/d15/L7:2
    quad/dsg/equi-exp/b0/d3/L16:12
    quad/sg/equi-sin/b0/d7/L7:2 quad/sg/cc-uq/b0/d10/L4:2 quad/sg/equi-exp/b0/d10/L4:2
    quad/sg/equi-exp/b1/d20/L5:6 quad/sg/cc-uq/b0/d2/L20:2 quad/sg/equi-exp/b0/d2/L20:3
    interp/sg/cc/b0/d10/L2/p8:2 interp/sg/cc/b0/d7/L4/p16:2 interp/sg/cc/b0/d5/L6/p64:2
    interp/sg/cheb1/b0/d5/L4/p64:10 interp/sg/cc/b0/d2/L12/p64:10
    interp/dsg/cc/b0/d10/L2/p8:4 interp/dsg/cc/b0/d7/L4/p16:8 interp/dsg/cc/b0/d5/L6/p64:8
    interp/dsg/cc/b0/d2/L12/p64:25 interp/dsg/equi/b0/d3/L5/p64:150
    interplin/sg/equi/b1/d10/L5/p32:2 interplin/sg/equi/b1/d8/L5/p256:2
    interplin/sg/equi/b1/d5/L8/p1024:4
    interplin/dsg/equi/b1/d10/L5/p32:4 interplin/dsg/equi/b1/d8/L5/p256:4
    interplin/dsg/equi/b1/d5/L8/p1024:8 interplin/dsg/equi/b1/d2/L16/p16384:8
    mivec/sg/cc-exp/b1/d5/S16/n1000:2 mivec/sg/cc-exp/b1/d10/S12/n1000:3
  )
fi
for spec in "${CASES[@]}"; do
  c=${spec%%:*}; n=${spec##*:}
  stem=$(echo "$c" | sed 's/[^A-Za-z0-9]\+/_/g')-t16
  log=$($PERF profile "^${c}\$" --snapshot rp-4 --samples "$n" --top 40 2>&1)
  data=$(echo "$log" | sed -n 's/^perf data: \(.*\.data\) .*/\1/p')
  echo "$log" | sed '/^perf data:/d' > "$OUT/$stem.txt"
  echo "samples per process: 1 warm-up + $n" >> "$OUT/$stem.txt"
  .venv/bin/python combigrid/perf/results/RP-3/profile_split.py "$data" "$LIB" 40 > "$OUT/$stem-split.txt"
  echo "done $c ($data)"
done
