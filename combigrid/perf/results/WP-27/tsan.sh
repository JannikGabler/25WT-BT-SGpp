#!/bin/bash
# WP-27: builds and runs tsan_adaptive_driver.cpp (adaptive policy) and the WP-28 driver with
# ThreadSanitizer (memory-capped).
# WP-28's script with the sources of the adaptive policy (WP-27); runs both drivers.
# Usage (from the repo root, after a debug or normal build of libsgppbase):
#   bash combigrid/perf/results/WP-27/tsan.sh <out-dir>
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
SRC=combigrid/src/sgpp/combigrid
for DRIVER in WP-27/tsan_adaptive_driver WP-28/tsan_cache_driver; do
NAME=$(basename $DRIVER)
g++ -std=c++23 -O1 -g -fsanitize=thread -fno-omit-frame-pointer -DSGPP_COMBIGRID_LMDB_LINKED=1 \
  -Ibase/src -Icombigrid/src \
  combigrid/perf/results/$DRIVER.cpp \
  $SRC/functions/source_functions/source_function.cpp \
  $SRC/functions/source_functions/vector_source_function.cpp \
  $SRC/functions/source_functions/caching/source_function_cache.cpp \
  $SRC/functions/source_functions/caching/source_function_cache_adaptive.cpp \
  $SRC/miscellaneous/caching/adaptive/adaptive_cache_controller.cpp \
  $SRC/miscellaneous/caching/adaptive/adaptive_rules.cpp \
  $SRC/miscellaneous/caching/adaptive/cost_sample_window.cpp \
  $SRC/miscellaneous/caching/adaptive/cost_timer.cpp \
  $SRC/miscellaneous/caching/adaptive/hit_rate_tracker.cpp \
  $SRC/functions/source_functions/caching/cached_source_function.cpp \
  $SRC/functions/source_functions/caching/cached_vector_source_function.cpp \
  $SRC/miscellaneous/caching/boost_point_value_table/boost_point_value_table.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.cpp \
  $SRC/tools/caching/lmdb_tools.cpp \
  $SRC/tools/data_vector/normalization.cpp \
  -Llib -lsgppbase -llmdb -Wl,-rpath,"$PWD/lib" -o "$OUT/$NAME"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/$NAME"
done
