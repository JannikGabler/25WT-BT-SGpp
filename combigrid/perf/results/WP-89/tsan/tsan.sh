#!/bin/bash
# Builds the combigrid sources with ThreadSanitizer together with tsan_refinement_driver.cpp and
# runs it (memory-capped). Usage (repo root, after a build of libsgppbase):
#   bash combigrid/perf/results/WP-89/tsan/tsan.sh <out-dir>
# libgomp is not instrumented: races between data written before the parallel region and read in
# it (fork / join) can be reported falsely; races between two worker threads are real.
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
SRCS=$(find combigrid/src/sgpp/combigrid -name '*.cpp' -not -path '*/fast_quadrature.cpp')
g++ -std=c++23 -O1 -g -fsanitize=thread -fopenmp -fno-omit-frame-pointer \
  -DSGPP_COMBIGRID_LMDB_LINKED=1 -DUSE_EIGEN=1 -DUSE_ARMADILLO=1 -Ibase/src -Icombigrid/src \
  combigrid/perf/results/WP-89/tsan/tsan_refinement_driver.cpp $SRCS \
  -Llib -lsgppbase -llmdb -larmadillo -Wl,-rpath,"$PWD/lib" -o "$OUT/tsan_refinement_driver"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="second_deadlock_stack=1 report_signal_unsafe=0" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/tsan_refinement_driver"
