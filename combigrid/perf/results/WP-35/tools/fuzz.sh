#!/bin/bash
# fuzz.sh <bindir> <run_test filter> <first> <count> <out> : runs one process per seed (FAKE_TIME),
# appends "seed<TAB>failing test" lines to <out>.
B=$1; T=$2; FIRST=$3; N=$4; OUT=$5
S=$(dirname "$0")
export LD_LIBRARY_PATH=$B OMP_NUM_THREADS=2
seq 0 $((N-1)) | xargs -P 8 -I{} bash -c '
  seed=$(( '"$FIRST"' + {} * 7919 ))
  FAKE_TIME=$seed LD_PRELOAD='"$S"'/shim/fake_time.so '"$B"'/test_combigrid_boost --run_test="'"$T"'" --log_level=error 2>&1 \
    | grep -o "error: in \"[^\"]*\"" | sort -u | sed "s/^/$seed\t/"' >> $OUT
echo done >> $OUT.done
