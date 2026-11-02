A/B `e2e`: A=`WP-28-before` B=`WP-28-refactor`, 16 thread(s), 3 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 402.56ms | 398.14ms | 1.009 | [0.810, 1.256] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 340.29ms | 330.20ms | 1.009 | [0.808, 1.260] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.54ms | 30.14ms | 1.032 | [0.833, 1.280] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.69ms | 247.76ms | 1.000 | [0.977, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.15ms | 245.46ms | 0.999 | [0.976, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 1.78ms | 1.118 | [0.858, 1.457] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 498.70ms | 496.02ms | 1.003 | [0.983, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 496.21ms | 493.58ms | 1.003 | [0.982, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.76ms | 1.80ms | 1.043 | [0.784, 1.388] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 917.21ms | 914.38ms | 1.003 | [0.991, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 913.08ms | 912.22ms | 1.002 | [0.989, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 3.70ms | 2.03ms | 1.481 | [0.465, 4.712] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 300.76ms | 301.93ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 292.16ms | 293.28ms | 0.997 | [0.981, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.27ms | 5.26ms | 1.000 | [0.683, 1.463] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 518.67ms | 519.41ms | 1.001 | [0.976, 1.027] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.00ms | 495.10ms | 1.002 | [0.974, 1.031] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.55ms | 15.59ms | 0.990 | [0.789, 1.243] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.16ms | 231.27ms | 1.011 | [0.984, 1.039] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.53ms | 205.16ms | 1.003 | [0.992, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 17.94ms | 17.60ms | 1.068 | [0.585, 1.951] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.404s | 2.400s | 1.002 | [0.986, 1.019] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.401s | 2.397s | 1.002 | [0.986, 1.019] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.94ms | 1.93ms | 1.003 | [0.812, 1.238] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.204s | 1.223s | 0.971 | [0.804, 1.172] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.117s | 1.142s | 0.965 | [0.804, 1.159] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 73.99ms | 73.98ms | 0.979 | [0.602, 1.590] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 85.48ms | 86.33ms | 0.991 | [0.962, 1.021] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.29ms | 83.83ms | 0.997 | [0.972, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.06ms | 0.986 | [0.961, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.99ms | 321.14ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.11ms | 318.53ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.33ms | 2.34ms | 1.017 | [0.846, 1.222] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 692.08ms | 691.17ms | 1.001 | [0.989, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.50ms | 688.44ms | 1.001 | [0.990, 1.011] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.94ms | 2.04ms | 0.966 | [0.783, 1.191] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.305s | 1.303s | 1.003 | [0.992, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.303s | 1.301s | 1.002 | [0.992, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 4.08ms | 2.27ms | 1.434 | [0.444, 4.636] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.25ms | 91.83ms | 1.002 | [0.992, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.90ms | 90.47ms | 1.004 | [0.994, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 1.11ms | 0.849 | [0.567, 1.271] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.47ms | 201.85ms | 1.008 | [0.988, 1.029] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 200.28ms | 199.21ms | 1.009 | [0.987, 1.031] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.11ms | 1.06ms | 1.052 | [0.615, 1.801] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.135s | 1.137s | 0.993 | [0.965, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.128s | 1.129s | 0.993 | [0.966, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.16ms | 3.18ms | 0.994 | [0.962, 1.027] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.412s | 5.411s | 1.000 | [0.993, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.404s | 5.402s | 1.000 | [0.993, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.59ms | 3.67ms | 1.014 | [0.575, 1.788] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.219s | 1.225s | 0.994 | [0.986, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.146s | 1.149s | 0.996 | [0.985, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.47ms | 41.10ms | 0.977 | [0.717, 1.332] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.226s | 1.224s | 1.001 | [0.990, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.123s | 1.125s | 0.997 | [0.990, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.69ms | 59.36ms | 1.056 | [0.837, 1.333] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.71ms | 249.01ms | 0.999 | [0.991, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.74ms | 245.26ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.35ms | 2.50ms | 1.025 | [0.559, 1.880] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.439s | 1.437s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.429s | 1.427s | 1.001 | [0.993, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.95ms | 5.01ms | 0.996 | [0.760, 1.307] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.788s | 6.782s | 1.005 | [0.988, 1.022] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.778s | 6.773s | 1.005 | [0.987, 1.023] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.22ms | 3.96ms | 1.081 | [0.462, 2.527] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.83ms | 175.28ms | 1.000 | [0.963, 1.038] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 174.37ms | 174.87ms | 1.000 | [0.964, 1.037] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.29us | 215.60us | 0.959 | [0.804, 1.144] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 253.51ms | 241.53ms | 1.011 | [0.897, 1.139] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 253.36ms | 241.37ms | 1.011 | [0.897, 1.139] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.54us | 72.98us | 1.011 | [0.872, 1.172] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.443s | 1.445s | 1.005 | [0.983, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.443s | 1.444s | 1.005 | [0.983, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 113.77us | 102.95us | 1.158 | [0.736, 1.822] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.259s | 1.261s | 0.997 | [0.972, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.259s | 1.261s | 0.997 | [0.972, 1.023] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.21us | 65.86us | 0.970 | [0.871, 1.080] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.984s | 1.966s | 1.005 | [0.995, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.984s | 1.966s | 1.005 | [0.995, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.78us | 36.46us | 0.860 | [0.446, 1.659] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.92ms | 180.72ms | 0.981 | [0.861, 1.118] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.84ms | 180.64ms | 0.981 | [0.860, 1.118] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.80us | 26.26us | 0.908 | [0.381, 2.165] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.27ms | 335.61ms | 0.999 | [0.973, 1.025] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.15ms | 335.49ms | 0.999 | [0.973, 1.025] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.36us | 36.11us | 0.921 | [0.556, 1.527] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.91ms | 50.80ms | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.84ms | 50.73ms | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.46us | 19.48us | 1.031 | [0.757, 1.402] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.38ms | 107.79ms | 1.153 | [0.657, 2.024] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.34ms | 101.52ms | 1.161 | [0.645, 2.092] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.73ms | 1.60ms | 1.058 | [0.895, 1.250] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 443.07ms | 440.66ms | 1.012 | [0.954, 1.074] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 440.15ms | 437.80ms | 1.015 | [0.960, 1.072] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.08ms | 3.39ms | 0.681 | [0.340, 1.364] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.534s | 1.511s | 1.012 | [0.967, 1.060] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.531s | 1.508s | 1.012 | [0.967, 1.058] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.30ms | 2.17ms | 1.217 | [0.531, 2.788] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.438s | 2.429s | 1.001 | [0.961, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.435s | 2.427s | 1.001 | [0.961, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.80ms | 1.77ms | 1.307 | [0.409, 4.180] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 768.32ms | 741.85ms | 1.024 | [0.902, 1.163] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 764.61ms | 738.07ms | 1.024 | [0.901, 1.164] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.01ms | 3.04ms | 0.989 | [0.962, 1.018] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 484.54ms | 496.88ms | 0.988 | [0.890, 1.097] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 475.92ms | 488.37ms | 0.987 | [0.889, 1.096] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 8.15ms | 7.51ms | 1.058 | [0.927, 1.207] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 27.53ms | 26.23ms | 1.382 | [0.343, 5.577] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.44ms | 23.52ms | 0.999 | [0.902, 1.105] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.45ms | 2.44ms | 2.646 | [0.040, 176.712] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.27ms | 3.16ms | 1.132 | [0.736, 1.742] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.05ms | 2.05ms | 0.934 | [0.654, 1.333] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 959.12us | 889.70us | 1.516 | [0.321, 7.171] | ~ (noise) |

Sum of medians (total, common cases): A = 38.385s, B = 38.304s, saved = 81.04ms (1.002x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.58e-15 -> B 2.15e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.7 MiB | 620.1 MiB | 1.001 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.8 MiB | 15.7 MiB | 0.992 | 3.5% / 3.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.8 MiB | 1.015 | 2.5% / 2.8% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.7 MiB | 13.1 MiB | 1.026 | 2.7% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.8 MiB | 26.1 MiB | 1.012 | 1.0% / 2.2% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.6 MiB | 53.7 MiB | 1.003 | 1.7% / 1.3% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 58.0 MiB | 57.7 MiB | 0.995 | 4.1% / 1.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.8 MiB | 14.8 MiB | 0.998 | 2.3% / 1.2% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1080.2 MiB | 1058.4 MiB | 0.980 | 1.8% / 4.5% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.3 MiB | 40.5 MiB | 1.004 | 4.0% / 1.5% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.7 MiB | 1.031 | 3.0% / 4.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.1 MiB | 15.1 MiB | 0.997 | 1.5% / 0.8% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.0 MiB | 0.998 | 0.7% / 2.8% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.1 MiB | 62.7 MiB | 1.010 | 2.7% / 1.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 26.0 MiB | 1.005 | 0.4% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.7 MiB | 43.0 MiB | 1.031 | 2.8% / 2.3% | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.6 MiB | 41.4 MiB | 0.996 | 1.8% / 2.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 149.9 MiB | 151.9 MiB | 1.013 | 3.0% / 1.7% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.3 MiB | 217.6 MiB | 1.001 | 0.6% / 0.6% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.2 MiB | 25.9 MiB | 1.029 | 1.4% / 0.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.6 MiB | 43.5 MiB | 1.020 | 2.7% / 6.4% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 41.9 MiB | 43.0 MiB | 1.026 | 2.2% / 5.7% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 15.1 MiB | 1.018 | 3.7% / 2.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.3 MiB | 1.009 | 0.4% / 2.4% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.2 MiB | 1.008 | 1.2% / 1.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.2 MiB | 1.019 | 0.8% / 2.2% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.1 MiB | 25.2 MiB | 1.047 | 2.3% / 1.9% | MEM+ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.1 MiB | 1.022 | 2.0% / 1.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 12.1 MiB | 1.018 | 2.0% / 1.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.1 MiB | 1.015 | 1.1% / 0.8% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.6 MiB | 19.8 MiB | 1.010 | 1.0% / 1.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.3 MiB | 1.006 | 0.7% / 1.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.8 MiB | 0.995 | 1.0% / 2.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.6 MiB | 13.5 MiB | 0.992 | 3.9% / 2.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.3 MiB | 1.012 | 3.4% / 2.0% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.3 MiB | 17.1 MiB | 0.988 | 3.1% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 13.1 MiB | 1.019 | 1.2% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.4 MiB | 0.995 | 2.1% / 2.2% | ~ |

Peak memory (common cases): geomean B/A = 1.009; largest B/A: `interp/sg/cc/b0/d10/L2/p8` 24.1 MiB -> 25.2 MiB (1.047x); 2 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%
Round durations (s): 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 486s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
