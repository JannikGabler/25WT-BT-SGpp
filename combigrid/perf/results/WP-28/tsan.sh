#!/bin/bash
# WP-28: builds and runs tsan_cache_driver.cpp (storage policies, LMDB) with ThreadSanitizer (memory-capped).
# Paths updated for the layout of 1e279c1d0 (WP-28 re-check).
# Usage (from the repo root, after a debug or normal build of libsgppbase):
#   bash combigrid/perf/results/WP-28/tsan.sh <out-dir>
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
SRC=combigrid/src/sgpp/combigrid
g++ -std=c++23 -O1 -g -fsanitize=thread -fno-omit-frame-pointer -DSGPP_COMBIGRID_LMDB_LINKED=1 \
  -Ibase/src -Icombigrid/src \
  combigrid/perf/results/WP-28/tsan_cache_driver.cpp \
  $SRC/functions/source_functions/source_function.cpp \
  $SRC/functions/source_functions/vector_source_function.cpp \
  $SRC/functions/source_functions/caching/source_function_cache.cpp \
  $SRC/functions/source_functions/caching/cached_source_function.cpp \
  $SRC/functions/source_functions/caching/cached_vector_source_function.cpp \
  $SRC/miscellaneous/caching/boost_point_value_table/boost_point_value_table.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.cpp \
  $SRC/tools/caching/lmdb_tools.cpp \
  $SRC/tools/data_vector/normalization.cpp \
  -Llib -lsgppbase -llmdb -Wl,-rpath,"$PWD/lib" -o "$OUT/tsan_cache_driver"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/tsan_cache_driver"
