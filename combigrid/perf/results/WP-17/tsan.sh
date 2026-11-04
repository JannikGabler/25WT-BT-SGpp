#!/bin/bash
# WP-17: builds and runs tsan_batch_driver.cpp (block evaluation of the cached functions) and the
# WP-27 / WP-28 drivers with ThreadSanitizer (memory-capped).
# WP-27's script with the sources of the evaluators (miscellaneous/source_function_evaluation/).
# Usage (from the repo root, after a debug or normal build of libsgppbase):
#   bash combigrid/perf/results/WP-17/tsan.sh <out-dir>
set -euo pipefail
OUT="${1:?output directory}"
mkdir -p "$OUT"
SRC=combigrid/src/sgpp/combigrid
for DRIVER in WP-17/tsan_batch_driver WP-27/tsan_adaptive_driver WP-28/tsan_cache_driver; do
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
  $SRC/miscellaneous/source_function_evaluation/batch_source_func_evaluator.cpp \
  $SRC/miscellaneous/source_function_evaluation/cached_source_func_evaluator.cpp \
  $SRC/miscellaneous/source_function_evaluation/adaptive_cached_source_func_evaluator.cpp \
  $SRC/miscellaneous/caching/boost_point_value_table/boost_point_value_table.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.cpp \
  $SRC/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store_file_interface.cpp \
  $SRC/tools/caching/lmdb_tools.cpp \
  $SRC/tools/data_vector/normalization.cpp \
  -Llib -lsgppbase -llmdb -Wl,-rpath,"$PWD/lib" -o "$OUT/$NAME"
OPENBLAS_NUM_THREADS=1 TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" \
  systemd-run --user --scope -q -p MemoryMax=8G "$OUT/$NAME"
done
