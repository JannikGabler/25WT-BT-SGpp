#!/usr/bin/env bash
# Builds a WP-76 prototype driver with the judged flags against the in-tree lib/ (normal build).
# Usage: build_proto.sh SOURCE.cpp OUTPUT
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../../.." && pwd)"
cd "$ROOT"
g++ -march=native -flto=auto -O3 -DNDEBUG -funroll-loops -fno-semantic-interposition -fopenmp \
  -mavx2 -mfma -mfpmath=sse -std=c++23 -DUSE_EIGEN=1 -DUSE_ARMADILLO=1 \
  -Ibase/src -Icombigrid/src "$1" -o "$2" -Llib -lsgppcombigrid -lsgppbase -Wl,-rpath,"$ROOT/lib"
