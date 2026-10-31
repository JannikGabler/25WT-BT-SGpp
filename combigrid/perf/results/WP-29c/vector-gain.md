# WP-29c vector gain (1 thread, snapshot WP-29c-after, one `perf.py run micro`)

gain = time of m scalar runs (`*_scalar_x_m`) / time of one vector run (`*_vec`); max ulp = vector vs. scalar result per output (counter `max_ulp`). WP-24 `innerk` prototype (`micro/vec/interp/innerk` vs. `micro/vec/interp/scalar_x_m`, prototype copies of the kernels) from the same run.

| kernel / cfg | m=1 | m=2 | m=4 | m=8 | m=32 | max ulp (m=1..32) | WP-24 innerk |
|---|---|---|---|---|---|---|---|
| interp/global_tg cc/b0/d2/L12 | 0.82 | 1.10 | 1.31 | 1.44 | 1.55 | 0/0/0/0/0 | 0.85/1.11/1.31/1.28/1.37 |
| interp/global_tg cc/b0/d5/L6 | 0.89 | 1.04 | 1.15 | 1.19 | 1.25 | 1/1/3/3/6 | 0.92/1.06/1.15/1.19/1.24 |
| interp/global_tg cheb1/b0/d5/L4 | 0.98 | 1.36 | 1.66 | 1.89 | 2.12 | 0/0/0/0/0 | 0.96/1.31/1.62/1.84/2.04 |
| interp/global_tg cc/b0/d10/L2 | 0.93 | 1.07 | 1.25 | 1.38 | 1.55 | 26/26/26/26/23396 | 0.99/1.10/1.27/1.35/1.44 |
| interplin/tg equi/b1/d2/L16 | 0.89 | 1.15 | 1.33 | 1.47 | 1.59 | 0/0/0/0/0 | – |
| interplin/tg equi/b1/d8/L5 | 0.95 | 1.06 | 1.11 | 1.16 | 1.21 | 0/0/0/0/0 | – |
| interplin/tg equi/b1/d10/L5 | 1.01 | 1.00 | 1.20 | 1.24 | 1.44 | 0/0/0/0/0 | – |
