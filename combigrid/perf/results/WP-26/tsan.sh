#!/bin/bash
# WP-26: builds and runs tsan_cache_driver.cpp with ThreadSanitizer (memory-capped).
# Usage (from the repo root, after a debug or normal build of libsgppbase):
#   bash combigrid/perf/results/WP-26/tsan.sh <out-dir>
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
SRC=combigrid/src/sgpp/combigrid
g++ -std=c++23 -O1 -g -fsanitize=thread -fno-omit-frame-pointer \
  -Ibase/src -Icombigrid/src \
  combigrid/perf/results/WP-26/tsan_cache_driver.cpp \
  $SRC/functions/source_functions/source_function.cpp \
  $SRC/functions/source_functions/vector_source_function.cpp \
  $SRC/functions/source_functions/caching/source_function_cache.cpp \
  $SRC/functions/source_functions/caching/cached_source_function.cpp \
  $SRC/functions/source_functions/caching/cached_vector_source_function.cpp \
  $SRC/miscellaneous/caching/boost_point_value_table.cpp \
  $SRC/tools/data_vector/normalization.cpp \
  -Llib -lsgppbase -Wl,-rpath,"$PWD/lib" -o "$OUT/tsan_cache_driver"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/tsan_cache_driver"
