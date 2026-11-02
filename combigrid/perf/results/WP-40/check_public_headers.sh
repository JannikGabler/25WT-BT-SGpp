#!/bin/bash
# WP-40: checks that the public headers of combigrid (the closure of the umbrella header
# sgpp_combigrid.hpp) are free of the optional caching libraries and of the configuration
# macros, for every configuration of the build:
#   1. no header of the closure is boost/unordered*, xxhash.h or lmdb.h;
#   2. no header of the closure under base/src or combigrid/src tests a configuration macro
#      (SGPP_COMBIGRID_COMPILE_*, SGPP_COMBIGRID_DISABLE_*, SGPP_COMBIGRID_LMDB_LINKED), so no
#      public class layout can depend on how the library was configured;
#   3. the closure is the same with and without the SGPP_COMBIGRID_DISABLE_* flags.
# The configurations define SGPP_COMBIGRID_LMDB_LINKED (as SCons does when it finds LMDB): the
# worst case for check 1.
# Usage (from the repo root): bash combigrid/perf/results/WP-40/check_public_headers.sh
# Exit code 0: all checks passed; 1: a check failed (the offending headers are listed).
set -uo pipefail
CXX="${CXX:-g++}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
echo '#include <sgpp_combigrid.hpp>' > "$TMP/user.cpp"

closure() {  # closure of the umbrella header, one header per line, for the flags "$@"
  "$CXX" -std=c++23 -M -Ibase/src -Icombigrid/src -DSGPP_COMBIGRID_LMDB_LINKED=1 "$@" \
    "$TMP/user.cpp" | tr ' \\' '\n\n' | grep -E '\.(h|hpp|hxx)$|/c\+\+/' | grep -v '^$' | sort -u
}

status=0
configs=("" "-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE" "-DSGPP_COMBIGRID_DISABLE_LMDB_STORE"
         "-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE -DSGPP_COMBIGRID_DISABLE_LMDB_STORE")
for i in "${!configs[@]}"; do
  # shellcheck disable=SC2086
  closure ${configs[$i]} > "$TMP/closure$i.txt" || { echo "FAIL: compiler error"; exit 1; }
done
echo "closure of sgpp_combigrid.hpp: $(wc -l < "$TMP/closure0.txt") headers," \
  "$(grep -cE '^(base|combigrid)/src/' "$TMP/closure0.txt") of them SG++ headers"

forbidden="$(grep -E '(^|/)(boost/unordered[^ ]*|xxhash\.h|lmdb\.h)$' "$TMP/closure0.txt")"
if [ -n "$forbidden" ]; then
  echo "FAIL 1: library headers in the public closure:"; echo "$forbidden" | sed 's/^/  /'
  status=1
else
  echo "OK 1: no boost/unordered*, xxhash.h or lmdb.h"
fi

macros='SGPP_COMBIGRID_(COMPILE_|DISABLE_|LMDB_LINKED)'
offenders="$(grep -E '^(base|combigrid)/src/' "$TMP/closure0.txt" | while read -r h; do
  grep -lE "^[[:space:]]*#[[:space:]]*(if|ifdef|ifndef|elif).*$macros" "$h"; done)"
if [ -n "$offenders" ]; then
  echo "FAIL 2: public headers that test configuration macros:"; echo "$offenders" | sed 's/^/  /'
  status=1
else
  echo "OK 2: no public header tests a configuration macro"
fi

same=1
for i in 1 2 3; do
  if ! diff -q "$TMP/closure0.txt" "$TMP/closure$i.txt" > /dev/null; then
    echo "FAIL 3: closure differs with ${configs[$i]}:"
    diff "$TMP/closure0.txt" "$TMP/closure$i.txt" | sed 's/^/  /' | head -20
    same=0
    status=1
  fi
done
[ $same -eq 1 ] && echo "OK 3: same closure with and without the SGPP_COMBIGRID_DISABLE_* flags"
exit $status
