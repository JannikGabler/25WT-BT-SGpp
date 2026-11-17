#!/usr/bin/env bash
# WP-76: builds perf_e2e_history.cpp against the headers of each state and the libraries of its
# snapshot, as pseudo-snapshots .work/snapshots/hist-<state> (bin/perf_e2e + lib/), so that
# `perf.py run e2e --snapshot hist-<state>` measures the expensive/ cases at that state.
# Usage: build_history.sh SCRATCH_DIR
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
SCRATCH="$1"
SNAP="$ROOT/combigrid/perf/.work/snapshots"
# state  commit (source of the headers)  snapshot (libraries)
STATES="baseline:fed2cdb4e:baseline rp-1:b0b7faac7:rp-1 rp-2:50a8737e0:rp-2 rp-3:e1257538d:rp-3
rp-4:cf698f31d:rp-4 rp-5:d426bddcf:rp-5 head:HEAD:WP-76-after"
for entry in $STATES; do
  IFS=: read -r state commit snapshot <<< "$entry"
  hdr="$SCRATCH/hist-src/$state"
  rm -rf "$hdr" && mkdir -p "$hdr"
  git -C "$ROOT" archive "$commit" base/src combigrid/src | tar -x -C "$hdr"
  out="$SNAP/hist-$state"
  rm -rf "$out" && mkdir -p "$out/bin" "$out/lib"
  cp "$SNAP/$snapshot/lib/"*.so "$out/lib/"
  g++ -march=native -flto=auto -O3 -DNDEBUG -funroll-loops -fno-semantic-interposition -fopenmp \
    -mavx2 -mfma -mfpmath=sse -std=c++23 -DUSE_EIGEN=1 -DUSE_ARMADILLO=1 \
    -I"$hdr/base/src" -I"$hdr/combigrid/src" -I"$ROOT/combigrid/benchmarks/perf" \
    "$ROOT/combigrid/perf/results/WP-76/history/perf_e2e_history.cpp" -o "$out/bin/perf_e2e" \
    -L"$out/lib" -lsgppcombigrid -lsgppbase
  printf '{"name": "hist-%s", "source": "WP-76 history driver, headers %s, libraries of snapshot %s"}\n' \
    "$state" "$commit" "$snapshot" > "$out/meta.json"
  echo "hist-$state: $(LD_LIBRARY_PATH="$out/lib" "$out/bin/perf_e2e" --list | wc -l) cases"
done
