#!/bin/bash
# Builds the WP-62 prototype programs against the in-tree libraries (lib/ of the repository;
# use the judged build: perf.py build normal). Usage: bash build.sh [program ...]
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../../../.." && pwd)"
mkdir -p "$here/bin"
programs=("$@")
[ ${#programs[@]} -eq 0 ] && programs=(run_quad bench_structures)
for program in "${programs[@]}"; do
  [ -f "$here/$program.cpp" ] || continue
  g++ -std=c++23 -O3 -DNDEBUG -march=native -fopenmp -Wall -Wextra \
    -I"$repo/base/src" -I"$repo/combigrid/src" \
    "$here/$program.cpp" -o "$here/bin/$program" \
    -L"$repo/lib" -lsgppcombigrid -lsgppbase -Wl,-rpath,"$repo/lib"
  echo "built bin/$program"
done
