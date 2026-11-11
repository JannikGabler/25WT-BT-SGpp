#!/bin/bash
# Re-runs the RP-3 inline-chain split for every WP-36 profile (data path from the profile text).
cd "/home/jannik/DEV/University/7. Semester/25WT-BT-SGpp"
OUT=combigrid/perf/results/WP-36/profiles
LIB=combigrid/perf/.work/snapshots/WP-36/lib
for txt in "$OUT"/*-t1.txt "$OUT"/*-t16.txt; do
  case "$txt" in *-split.txt) continue;; esac
  data=$(sed -n 's/^perf data: \(.*\.data\) (inspect.*/\1/p' "$txt")
  [ -n "$data" ] || { echo "no data in $txt"; continue; }
  .venv/bin/python combigrid/perf/results/RP-3/profile_split.py "$data" "$LIB" 30 > "${txt%.txt}-split.txt" 2>&1
  echo "split $(basename "$txt")"
done
