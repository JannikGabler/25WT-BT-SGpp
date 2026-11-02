#!/bin/bash
# WP-40: build time and size of perf_micro alone (the compile and link commands of the judged
# 'normal' build, taken from the SCons log; objects go to a temporary directory, the tree is not
# touched), plus the case counts of `perf_micro --list`.
# Usage (from the repo root, after `perf.py build normal`): bash combigrid/perf/results/WP-40/perf_micro_build.sh [runs]
set -euo pipefail
RUNS="${1:-2}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
FLAGS="-march=native -flto=auto -O3 -DNDEBUG -fno-omit-frame-pointer -g -Wall -Wextra -Wcast-qual
  -Wconversion -Wformat=2 -Wformat-nonliteral -Wformat-security -Winit-self
  -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wunreachable-code -Wunused
  -Wno-unused-parameter -Wswitch-enum -Wredundant-decls -pedantic -funroll-loops
  -fno-semantic-interposition -fopenmp -mavx2 -mfma -mfpmath=sse -fPIC -std=c++23 -DZLIB=1
  -DSGPP_COMBIGRID_LMDB_LINKED=1 -DUSE_EIGEN=1 -DUSE_ARMADILLO=1 -Ibase/src -Icombigrid/src
  -I/usr/include -Itools"
for r in $(seq "$RUNS"); do
  t0=$(date +%s.%N)
  # shellcheck disable=SC2086
  g++ -o "$TMP/perf_micro.o" -c $FLAGS combigrid/benchmarks/perf/perf_micro.cpp
  t1=$(date +%s.%N)
  g++ -o "$TMP/perf_micro" -march=native -flto=auto -fno-semantic-interposition -fopenmp \
    "$TMP/perf_micro.o" -L/usr/lib/x86_64-linux-gnu -Llib -larmadillo -lsgppcombigrid \
    -lsgppbase -llmdb
  t2=$(date +%s.%N)
  printf 'run %d: compile %.1f s, link %.1f s, total %.1f s\n' "$r" \
    "$(echo "$t1 - $t0" | bc)" "$(echo "$t2 - $t1" | bc)" "$(echo "$t2 - $t0" | bc)"
done
echo "object: $(stat -c %s "$TMP/perf_micro.o") bytes; binary: $(stat -c %s "$TMP/perf_micro") bytes" \
  "(stripped: $(strip -o "$TMP/stripped" "$TMP/perf_micro" && stat -c %s "$TMP/stripped") bytes)"
LIST="$(LD_LIBRARY_PATH="$PWD/lib" "$TMP/perf_micro" --list)"
echo "cases: $(echo "$LIST" | grep -c '^micro/')"
echo "  micro/cache/: $(echo "$LIST" | grep -c '^micro/cache/')"
echo "  micro/vec/:   $(echo "$LIST" | grep -c '^micro/vec/')"
echo "  *_vec / *_scalar_x_m (WP-29): $(echo "$LIST" | grep -v '^micro/vec/' | grep -cE '(_vec|_scalar_x_m)/')"
echo "  classic: $(echo "$LIST" | grep '^micro/' | grep -vE '^micro/(vec|cache)/' | grep -cvE '(_vec|_scalar_x_m)/')"
