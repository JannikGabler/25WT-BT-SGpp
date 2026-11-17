#!/usr/bin/env bash
# WP-76: measures the expensive/ cases at every history state (pseudo-snapshots from
# build_history.sh), ROUNDS interleaved rounds of `perf.py run e2e` (16 threads, one process per
# case, 1 warm-up + 1 sample), raw JSON in runs/<state>-r<round>.json.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
ROUNDS="${1:-3}"
cd "$ROOT"
mkdir -p combigrid/perf/results/WP-76/history/runs
for r in $(seq 1 "$ROUNDS"); do
  for state in baseline rp-1 rp-2 rp-3 rp-4 rp-5 head; do
    python3 combigrid/perf/scripts/perf.py run e2e --snapshot "hist-$state" --filter '^expensive/' \
      --save "combigrid/perf/results/WP-76/history/runs/$state-r$r.json" | grep -E "Sum|foreign"
  done
done
