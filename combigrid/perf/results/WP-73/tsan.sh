#!/bin/bash
# WP-73: builds all combigrid sources and tsan_driver.cpp with ThreadSanitizer and runs it
# (memory-capped). Usage (from the repo root; LIBDIR: a directory with a non-ASan libsgppbase,
# default lib):
#   [LIBDIR=...] bash combigrid/perf/results/WP-73/tsan.sh <out-dir>
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT/obj"
export TSAN_FLAGS="-std=c++23 -O1 -g -fsanitize=thread -fno-omit-frame-pointer -fopenmp -Ibase/src -Icombigrid/src"
export OUT
LIBDIR="${LIBDIR:-$PWD/lib}"
find combigrid/src -name '*.cpp' | xargs -P8 -I{} bash -c \
  'g++ $TSAN_FLAGS -c "{}" -o "$OUT/obj/$(echo "{}" | tr / _).o"'
g++ $TSAN_FLAGS combigrid/perf/results/WP-73/tsan_driver.cpp "$OUT"/obj/*.o -L"$LIBDIR" -lsgppbase \
  -lxxhash -Wl,-rpath,"$LIBDIR" -o "$OUT/tsan_driver"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/tsan_driver"
