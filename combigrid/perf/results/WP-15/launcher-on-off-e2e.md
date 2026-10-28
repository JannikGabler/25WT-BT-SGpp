Launcher A/B on `WP-14-after`: A = direct launch (as before WP-15), B = through GNU time (WP-15); 5 rounds, one process per case and round, interleaved; speedup = A/B (> 1: B faster), 95% t-CI with the baseline noise model

| case | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 388.36ms | 394.88ms | 0.987 | [0.884, 1.101] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | 252.28ms | 252.37ms | 0.998 | [0.986, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | 503.53ms | 501.22ms | 1.010 | [0.986, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | 936.33ms | 938.16ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | 307.93ms | 305.79ms | 1.012 | [1.003, 1.021] | faster |
| quad/sg/equi-exp/b1/d15/L6 | 522.65ms | 526.68ms | 0.993 | [0.973, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | 234.55ms | 239.45ms | 0.985 | [0.950, 1.022] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | 2.411s | 2.412s | 1.001 | [0.993, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | 1.212s | 1.207s | 1.015 | [0.965, 1.068] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | 86.97ms | 87.34ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | 327.58ms | 325.81ms | 1.004 | [0.977, 1.032] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | 700.90ms | 700.46ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | 1.311s | 1.309s | 0.999 | [0.987, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | 97.13ms | 95.68ms | 1.012 | [1.007, 1.017] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | 213.52ms | 213.31ms | 1.005 | [0.975, 1.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | 1.216s | 1.216s | 1.000 | [0.988, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | 5.662s | 5.668s | 1.000 | [0.991, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | 4.085s | 4.097s | 0.991 | [0.974, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | 7.022s | 7.012s | 1.000 | [0.993, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | 258.37ms | 258.15ms | 0.999 | [0.992, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | 1.511s | 1.518s | 0.998 | [0.991, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | 7.052s | 7.020s | 1.002 | [0.996, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | 176.47ms | 176.18ms | 0.985 | [0.934, 1.040] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | 245.56ms | 244.60ms | 1.003 | [0.982, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | 1.453s | 1.459s | 0.991 | [0.967, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | 1.260s | 1.261s | 1.002 | [0.986, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | 2.001s | 2.000s | 1.002 | [0.986, 1.018] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | 181.69ms | 181.37ms | 1.009 | [0.989, 1.030] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | 338.13ms | 337.93ms | 0.972 | [0.901, 1.048] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | 51.36ms | 51.09ms | 0.961 | [0.851, 1.085] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | 110.64ms | 110.84ms | 0.986 | [0.949, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | 439.31ms | 449.64ms | 0.923 | [0.761, 1.120] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | 1.519s | 1.499s | 1.002 | [0.979, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | 2.459s | 2.451s | 1.010 | [0.985, 1.036] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 741.45ms | 743.76ms | 0.992 | [0.941, 1.047] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1.997s | 1.997s | 1.006 | [0.989, 1.022] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 2.096s | 2.099s | 1.000 | [0.994, 1.007] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 2.069s | 2.081s | 0.999 | [0.990, 1.008] | ~ (noise) |

Sum of medians: A = 53.453s, B = 53.440s (1.0002x); geomean speedup 0.9960
Foreign CPU load per round: 0.3%, 0.3%, 0.9%, 0.3%, 0.2%
