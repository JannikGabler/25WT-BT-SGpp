#!/bin/bash
# build_variant.sh <name> <before|after> <normal|off|fma> : test binary with both cache TUs varied.
set -e
V=$(dirname "$0"); NAME=$1; SRC=$2; MODE=$3; OUT=$V/$NAME; mkdir -p "$OUT"
D=combigrid/tests/functions/source_functions
link=$(cat "$V/link_cmd.txt")
for tu in cached_source_function_test adaptive_cached_source_function_test; do
  if [ "$SRC" = before ]; then git show HEAD:$D/$tu.cpp > "$OUT/$tu.cpp"; elif [ "$SRC" = keep ]; then :; else cp $D/$tu.cpp "$OUT/$tu.cpp"; fi
  extra=""
  [ "$MODE" = off ] && extra="-ffp-contract=off"
  if [ "$MODE" = fma ]; then
    [ "$SRC" = keep ] || sed -i 's/-0\.5 + 0\.1 \* static_cast<double>(k)/std::fma(0.1, static_cast<double>(k), -0.5)/' "$OUT/$tu.cpp"
    grep -q "std::fma(0.1" "$OUT/$tu.cpp" || { echo "fma substitution failed in $tu"; exit 1; }
  fi
  cmd=$(sed "s#-o $D/$tu.os#-o $OUT/$tu.os#; s#$D/$tu.cpp#$OUT/$tu.cpp#" "$V/acc_cmd.txt" | sed "s/adaptive_cached_source_function_test/$tu/g")
  [ "$tu" = cached_source_function_test ] && cmd=$(sed "s#-o $D/$tu.os#-o $OUT/$tu.os#; s#$D/$tu.cpp#$OUT/$tu.cpp#" "$V/cc_cmd.txt")
  eval "$cmd $extra" 2> "$OUT/$tu.log" || { tail "$OUT/$tu.log"; exit 1; }
  link=${link//"$D/$tu.os"/"$OUT/$tu.os"}
done
link=${link/"-o combigrid/tests/test_combigrid_boost"/"-o $OUT/test_combigrid_boost"}
eval "$link" 2> "$OUT/link.log" || { tail "$OUT/link.log"; exit 1; }
echo "built $OUT/test_combigrid_boost"
