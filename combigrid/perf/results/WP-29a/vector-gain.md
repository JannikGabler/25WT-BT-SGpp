# WP-29a: vector-valued SG quadrature vs. m scalar runs (1 thread, snapshot WP-29a-after)

One `perf.py run micro` (raw: `vector-gain-run.json`, foreign CPU load 0.3 %). Integrand `vec_proto::expSumK`
(output k: exp(0.5 (1 + k/64) sum x_i); one output costs one scalar evaluation). gain = time(m scalar runs) /
time(one vector run) (> 1: the vector run is faster). prod = production kernels (`micro/quad/<kernel>_vec` vs.
`<kernel>_scalar_x_m`, this WP); WP-24 rt = the WP-24 prototype kernel (`micro/vec/quad/rt` vs. `scalar_x_m`, same run);
max ulp = largest distance of a vector output from the scalar run of that output, per tensor grid (counter `max_ulp`).

| config | m | scalar x m [ns/pt/output] | vector [ns/pt/output] | gain prod | gain WP-24 rt | max ulp prod |
|---|---|---|---|---|---|---|
| equi/b0/d10/L4 | 1 | 51.94 | 58.63 | 0.89 | 0.92 | 0 |
| equi/b0/d10/L4 | 2 | 51.89 | 37.93 | 1.37 | 1.39 | 1 |
| equi/b0/d10/L4 | 4 | 51.94 | 29.93 | 1.74 | 1.71 | 1 |
| equi/b0/d10/L4 | 8 | 51.94 | 23.32 | 2.23 | 2.15 | 1 |
| equi/b0/d10/L4 | 32 | 51.98 | 19.42 | 2.68 | 2.56 | 14 |
| equi/b1/d20/L4 (all TGs) | 1 | 63.09 | 68.15 | 0.93 | 0.93 | 1 |
| equi/b1/d20/L4 (all TGs) | 2 | 63.22 | 43.74 | 1.45 | 1.44 | 2 |
| equi/b1/d20/L4 (all TGs) | 4 | 63.26 | 36.99 | 1.71 | 1.66 | 2 |
| equi/b1/d20/L4 (all TGs) | 8 | 63.09 | 29.36 | 2.15 | 2.06 | 3 |
| equi/b1/d20/L4 (all TGs) | 32 | 63.23 | 25.51 | 2.48 | 2.38 | 3 |
| equi/b0/d2/L16 | 1 | 15.94 | 24.08 | 0.66 | 0.68 | 0 |
| equi/b0/d2/L16 | 2 | 15.99 | 20.49 | 0.78 | 0.81 | 0 |
| equi/b0/d2/L16 | 4 | 15.99 | 15.71 | 1.02 | 1.01 | 2 |
| equi/b0/d2/L16 | 8 | 15.95 | 13.49 | 1.18 | 1.18 | 2 |
| equi/b0/d2/L16 | 32 | 16.34 | 12.67 | 1.29 | 1.38 | 2 |
