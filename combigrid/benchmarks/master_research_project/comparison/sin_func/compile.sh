#!/usr/bin/env bash
# Builds SG++ (base + combigrid) and one comparison benchmark like compileNormal.sh, plus GCC
# profile-guided optimization (PGO):
#
#   1. instrumented build  (-fprofile-generate): libraries + benchmark binary only
#   2. training runs       of the benchmark for TRAIN_DIMS with increasing levels; every run
#                          adds its counts to the .gcda files next to the object files
#   3. optimized build     (-fprofile-use) of the same targets, then the .gcda files are removed
#
# Usage (from any directory):
#   ./compile.sh [benchmark source relative to this directory]
#   ./compile.sh complete_sg_sin_func_quadrature.cpp      (default)
#   TRAIN_DIMS="2 5 10 20" TRAIN_SECONDS=30 ./compile.sh
#
# Afterwards lib/ holds the PGO libraries and the binary sits next to its source; run it with
#   export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH
#   combigrid/benchmarks/bachelor_thesis/comparison/run_benchmarks.py <binary> 30min "8 GiB" "[2, 5, 10]"
# Any later normal build (compileNormal.sh, perf.py build normal) rebuilds everything without PGO.

set -euo pipefail

# ---------------------------------------------------------------------------------------------
# Configuration (each can be overridden from the environment)
# ---------------------------------------------------------------------------------------------

JOBS="${JOBS:-16}"
# Dimensions of the training runs: use the dimensions you are going to benchmark
TRAIN_DIMS="${TRAIN_DIMS:-3 5 10}"
# Per dimension the level starts at TRAIN_START_LEVEL and grows until one (instrumented) run
# takes at least TRAIN_SECONDS, or TRAIN_MAX_LEVEL is reached
TRAIN_START_LEVEL="${TRAIN_START_LEVEL:-3}"
TRAIN_MAX_LEVEL="${TRAIN_MAX_LEVEL:-30}"
TRAIN_SECONDS="${TRAIN_SECONDS:-20}"
# Hard limits of one training run (a killed run contributes no profile and ends the dimension)
TRAIN_TIMEOUT="${TRAIN_TIMEOUT:-600}"
TRAIN_MEMORY_MAX="${TRAIN_MEMORY_MAX:-8G}"
# OpenMP threads of the training runs (empty = OpenMP default, i.e. all hardware threads)
TRAIN_THREADS="${TRAIN_THREADS:-}"
# 1: code not executed in training is optimized normally instead of for size
PARTIAL_TRAINING="${PARTIAL_TRAINING:-0}"
# 1: keep the .gcda profile files after the final build
KEEP_PROFILES="${KEEP_PROFILES:-0}"

# ---------------------------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Repository root = nearest parent directory with SConstruct and site_scons/ (the script can move)
REPO="$SCRIPT_DIR"
while [[ ! -f "$REPO/SConstruct" || ! -d "$REPO/site_scons" ]]; do
  [[ "$REPO" != / ]] || { echo "Error: no SG++ repository above $SCRIPT_DIR" >&2; exit 1; }
  REPO="$(dirname "$REPO")"
done
BENCHMARK_SOURCE="$SCRIPT_DIR/${1:-complete_sg_sin_func_quadrature.cpp}"
BENCHMARK_BINARY="${BENCHMARK_SOURCE%.cpp}"
# SCons target = binary path relative to the repository root
BENCHMARK_TARGET="${BENCHMARK_BINARY#"$REPO"/}"
TRAIN_LOG="$SCRIPT_DIR/pgo_training.log"

[[ -f "$BENCHMARK_SOURCE" ]] || { echo "Error: $BENCHMARK_SOURCE not found" >&2; exit 1; }

# ---------------------------------------------------------------------------------------------
# Flags: compileNormal.sh + PGO
# ---------------------------------------------------------------------------------------------

# Same as compileNormal.sh, but only the libraries and the benchmark binary are built. SConstruct
# makes every example / benchmark depend on the Boost test runs, so the tests are switched off:
# otherwise they run on the instrumented libraries (their counts would enter the profiles) and
# their results decide whether the benchmark is linked at all.
SCONS_ARGS=(-j"$JOBS" SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 OPT=1 ARCH=avx2 USE_EIGEN=1
            USE_ARMADILLO=1 COMPILE_BOOST_TESTS=0 RUN_BOOST_TESTS=0)
COMMON_FLAGS="-march=native -flto=auto"
# Counter updates: "single" (GCC's default) is racy between OpenMP threads, but GCC keeps loop
# counters in registers, so instrumented runs cost about as much as normal ones;
# -fprofile-correction smooths the resulting inconsistencies. "prefer-atomic" is exact, but the
# threads then contend for the counters of the shared hot loops (an OpenMP loop with 16 threads
# ran ~100x slower than with "single"), so only short training runs would fit into TRAIN_SECONDS.
PROFILE_UPDATE="${PROFILE_UPDATE:-single}"
GENERATE_FLAGS="$COMMON_FLAGS -fprofile-generate -fprofile-update=$PROFILE_UPDATE"
USE_FLAGS="$COMMON_FLAGS -fprofile-use -fprofile-correction"
if [[ "$PARTIAL_TRAINING" == 1 ]]; then
  USE_FLAGS="$USE_FLAGS -fprofile-partial-training"
fi

# ---------------------------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------------------------

section() { printf '\n========== %s ==========\n' "$*"; }

build() {
  local flags="$1"
  (cd "$REPO" && scons "${SCONS_ARGS[@]}" CPPFLAGS="$flags" LINKFLAGS="$flags" "$BENCHMARK_TARGET")
}

remove_profiles() {
  find "$REPO/base" "$REPO/combigrid" -name '*.gcda' -delete
}

count_profiles() {
  find "$REPO/base" "$REPO/combigrid" -name '*.gcda' | wc -l
}

# Memory cap as in run_benchmarks.py, if a user scope can be created
MEMORY_CAP=()
if command -v systemd-run > /dev/null &&
    systemd-run --user --quiet --scope -p MemoryMax="$TRAIN_MEMORY_MAX" true 2> /dev/null; then
  MEMORY_CAP=(systemd-run --user --quiet --scope -p MemoryMax="$TRAIN_MEMORY_MAX"
              -p MemorySwapMax=0 -p OOMPolicy=continue)
else
  echo "Warning: no memory-limited systemd scope available; training runs without memory cap"
fi

# One training run: 0 warmups, 1 round, no minimum times (as the probe of run_benchmarks.py).
# Prints the wall time in milliseconds; returns the exit status of the benchmark.
train_run() {
  local dims="$1" level="$2" start end status=0
  start=$(date +%s%N)
  {
    printf '\n----- d=%s level=%s\n' "$dims" "$level"
    "${MEMORY_CAP[@]}" timeout "$TRAIN_TIMEOUT" "$BENCHMARK_BINARY" 0 1 0 0 "$dims" "$level"
  } >> "$TRAIN_LOG" 2>&1 || status=$?
  end=$(date +%s%N)
  echo $(((end - start) / 1000000))
  return "$status"
}

train() {
  export LD_LIBRARY_PATH="$REPO/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
  export OMP_PROC_BIND=close OMP_PLACES=threads
  if [[ -n "$TRAIN_THREADS" ]]; then export OMP_NUM_THREADS="$TRAIN_THREADS"; fi
  : > "$TRAIN_LOG"

  local dims level ms status successful_runs=0
  for dims in $TRAIN_DIMS; do
    for ((level = TRAIN_START_LEVEL; level <= TRAIN_MAX_LEVEL; level++)); do
      status=0
      ms=$(train_run "$dims" "$level") || status=$?
      if ((status != 0)); then
        echo "d=$dims level=$level: exit status $status after ${ms} ms" \
             "(124 = timeout, 137 = memory limit) -> next dimension"
        break
      fi
      successful_runs=$((successful_runs + 1))
      echo "d=$dims level=$level: ${ms} ms"
      if ((ms >= TRAIN_SECONDS * 1000)); then break; fi
    done
  done

  if ((successful_runs == 0)); then
    echo "Error: no training run succeeded; see $TRAIN_LOG" >&2
    exit 1
  fi
}

# ---------------------------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------------------------

echo "Repository: $REPO"
echo "Benchmark:  $BENCHMARK_TARGET"
echo "Training:   d in [$TRAIN_DIMS], levels $TRAIN_START_LEVEL..$TRAIN_MAX_LEVEL until a run" \
     "takes ${TRAIN_SECONDS} s, threads ${TRAIN_THREADS:-all}"

section "1/3 Instrumented build"
# Old profiles would be merged into the new ones (or mismatch the new objects)
remove_profiles
build "$GENERATE_FLAGS"
# Only the training runs below may contribute (e.g. not configure checks run during the build)
remove_profiles

section "2/3 Training runs (output: $TRAIN_LOG)"
train
echo "Profiles written: $(count_profiles) .gcda files"

section "3/3 Build with profiles"
build "$USE_FLAGS"

if [[ "$KEEP_PROFILES" != 1 ]]; then
  remove_profiles
fi

section "Done"
echo "Binary: $BENCHMARK_BINARY"
echo "Run with: export LD_LIBRARY_PATH=\"$REPO/lib:\$LD_LIBRARY_PATH\""
