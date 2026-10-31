# WP-29b: vector-valued DSG quadrature vs. m scalar runs (1 thread, snapshot WP-29b-after)

One `perf.py run micro` (raw: `vector-gain-run.json`, foreign CPU load 0.3 %). Integrand `vec_proto::expSumK`
(output k: exp(0.5 (1 + k/64) sum x_i); one output costs one scalar evaluation). gain = time(m scalar runs) /
time(one vector run) (> 1: the vector run is faster). prod = production kernels (`micro/dsg/<kernel>_vec` vs.
`<kernel>_scalar_x_m`, this WP; `process_delta_tg` = largest delta tensor grid, `process_delta_tgs` = every 256th
delta tensor grid incl. the projected sub-tries, which the vector run builds once for all outputs); WP-24 rt = the
WP-24 prototype kernel (`micro/vec/dsg/rt` vs. `scalar_x_m`, largest delta tensor grid, same run); max ulp = largest
distance of a vector output from the scalar run of that output, per delta tensor grid (counter `max_ulp`; 0 =
bitwise equal everywhere).

| config | m | scalar x m [ns/pt/output] | vector [ns/pt/output] | gain prod | gain WP-24 rt | max ulp prod |
|---|---|---|---|---|---|---|
| equi/b0/d10/L7 (largest delta TG) | 1 | 57.46 | 60.53 | 0.95 | 0.94 | 0 |
| equi/b0/d10/L7 (largest delta TG) | 2 | 57.36 | 35.42 | 1.62 | 1.45 | 0 |
| equi/b0/d10/L7 (largest delta TG) | 4 | 57.44 | 30.66 | 1.87 | 1.81 | 0 |
| equi/b0/d10/L7 (largest delta TG) | 8 | 57.34 | 23.83 | 2.41 | 2.32 | 0 |
| equi/b0/d10/L7 (largest delta TG) | 32 | 57.42 | 19.42 | 2.96 | 2.80 | 0 |
| equi/b1/d20/L6 (largest delta TG) | 1 | 49.48 | 53.48 | 0.93 | 0.91 | 0 |
| equi/b1/d20/L6 (largest delta TG) | 2 | 49.49 | 35.02 | 1.41 | 1.30 | 0 |
| equi/b1/d20/L6 (largest delta TG) | 4 | 49.80 | 33.49 | 1.49 | 1.44 | 0 |
| equi/b1/d20/L6 (largest delta TG) | 8 | 49.63 | 27.79 | 1.79 | 1.73 | 0 |
| equi/b1/d20/L6 (largest delta TG) | 32 | 49.55 | 25.34 | 1.96 | 1.91 | 0 |
| equi/b1/d20/L6/s256 (every 256th delta TG, incl. sub-tries) | 1 | 83.44 | 86.86 | 0.96 | – | 0 |
| equi/b1/d20/L6/s256 (every 256th delta TG, incl. sub-tries) | 2 | 84.05 | 49.03 | 1.71 | – | 0 |
| equi/b1/d20/L6/s256 (every 256th delta TG, incl. sub-tries) | 4 | 83.96 | 41.98 | 2.00 | – | 0 |
| equi/b1/d20/L6/s256 (every 256th delta TG, incl. sub-tries) | 8 | 84.03 | 31.81 | 2.64 | – | 0 |
| equi/b1/d20/L6/s256 (every 256th delta TG, incl. sub-tries) | 32 | 84.03 | 26.42 | 3.18 | – | 0 |
