#!/bin/bash
# Thread scan of the short interpolation cases (snapshot WP-36): 2, 4, 8 threads (1 and 16 exist).
cd "/home/jannik/DEV/University/7. Semester/25WT-BT-SGpp"
source .venv/bin/activate
F='^interplin/sg/equi/b1/d2/|^interplin/sg/equi/b1/d5/|^interp/sg/cheb1/|^interp/sg/equi/|^interp/sg/cc/b0/d2/|^interp/sg/cc/b0/d3/'
for t in 2 4 8; do
  python3 combigrid/perf/scripts/perf.py run e2e --snapshot WP-36 --filter "$F" --threads $t --save combigrid/perf/results/WP-36/e2e-t$t.json > combigrid/perf/results/WP-36/e2e-t$t.txt 2>&1
  echo "done t$t"
done
