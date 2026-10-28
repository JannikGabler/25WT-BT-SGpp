Launcher A/B on `WP-14-after`: A = direct launch (as before WP-15), B = through GNU time (WP-15); 8 rounds, one process per case and round, interleaved; speedup = A/B (> 1: B faster), 95% t-CI with the baseline noise model

| case | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 416.40ms | 393.89ms | 1.038 | [0.964, 1.117] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | 96.09ms | 96.32ms | 0.993 | [0.987, 0.999] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | 338.85ms | 338.06ms | 1.000 | [0.991, 1.009] | ~ (noise) |

Sum of medians: A = 0.851s, B = 0.828s (1.0278x); geomean speedup 1.0102
Foreign CPU load per round: 0.2%, 0.2%, 0.2%, 0.2%, 0.3%, 0.2%, 0.2%, 0.3%
