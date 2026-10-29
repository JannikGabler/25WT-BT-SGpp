A/B `e2e`: A=`WP-23-before` B=`WP-23-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 413.84ms | 399.45ms | 1.046 | [0.938, 1.168] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 337.23ms | 325.82ms | 1.041 | [0.931, 1.163] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.96ms | 32.61ms | 0.987 | [0.886, 1.098] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 252.82ms | 252.36ms | 1.001 | [0.989, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 250.27ms | 249.91ms | 1.001 | [0.989, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 2.03ms | 1.003 | [0.879, 1.145] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 501.82ms | 503.79ms | 0.984 | [0.954, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 499.31ms | 501.24ms | 0.984 | [0.954, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.87ms | 1.79ms | 1.040 | [0.983, 1.100] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 933.90ms | 934.77ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 931.35ms | 932.19ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.12ms | 2.23ms | 0.831 | [0.551, 1.252] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 305.78ms | 303.60ms | 1.006 | [0.993, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 295.97ms | 294.93ms | 1.005 | [0.989, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.51ms | 5.06ms | 1.009 | [0.785, 1.298] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 522.01ms | 521.11ms | 0.996 | [0.982, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 497.69ms | 497.05ms | 0.998 | [0.982, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.90ms | 15.88ms | 0.920 | [0.796, 1.063] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.22ms | 237.03ms | 0.996 | [0.970, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.30ms | 208.83ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.58ms | 18.54ms | 0.996 | [0.737, 1.346] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415s | 2.414s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.412s | 2.411s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.95ms | 1.87ms | 1.028 | [0.925, 1.142] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.181s | 1.206s | 0.977 | [0.929, 1.027] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.105s | 1.125s | 0.974 | [0.942, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 77.32ms | 80.44ms | 1.030 | [0.808, 1.313] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.15ms | 87.90ms | 0.995 | [0.981, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.38ms | 85.18ms | 0.993 | [0.981, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 2.05ms | 0.989 | [0.869, 1.125] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.50ms | 325.61ms | 0.991 | [0.969, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 322.92ms | 322.70ms | 0.991 | [0.969, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.16ms | 2.38ms | 0.948 | [0.809, 1.111] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 701.97ms | 701.80ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 699.21ms | 699.06ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.95ms | 1.95ms | 0.996 | [0.923, 1.074] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.311s | 1.314s | 1.005 | [0.988, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.308s | 1.309s | 1.006 | [0.988, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.25ms | 4.23ms | 0.689 | [0.481, 0.988] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.37ms | 93.05ms | 1.000 | [0.988, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 92.03ms | 91.75ms | 1.001 | [0.988, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.01ms | 1.06ms | 0.949 | [0.775, 1.161] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.90ms | 201.58ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.20ms | 199.30ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.19ms | 1.14ms | 1.038 | [0.804, 1.340] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.134s | 1.135s | 0.997 | [0.980, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.126s | 1.128s | 0.997 | [0.980, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.06ms | 2.93ms | 1.289 | [0.868, 1.915] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.403s | 5.398s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.395s | 5.390s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.46ms | 3.18ms | 1.126 | [0.848, 1.494] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.218s | 1.219s | 1.002 | [0.989, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.143s | 1.146s | 1.003 | [0.991, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.99ms | 42.31ms | 1.045 | [0.895, 1.220] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.223s | 1.226s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.122s | 1.123s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.26ms | 59.39ms | 1.005 | [0.969, 1.042] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.32ms | 247.97ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.68ms | 244.38ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.42ms | 2.46ms | 0.968 | [0.872, 1.074] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.439s | 1.440s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.429s | 1.430s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.93ms | 4.75ms | 1.028 | [0.814, 1.299] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.776s | 6.781s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.767s | 6.773s | 1.000 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.56ms | 4.48ms | 0.995 | [0.651, 1.521] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 176.65ms | 177.09ms | 0.978 | [0.925, 1.033] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 176.29ms | 176.75ms | 0.978 | [0.925, 1.033] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 210.36us | 203.31us | 1.029 | [0.942, 1.124] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 245.01ms | 245.02ms | 0.996 | [0.966, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.87ms | 244.86ms | 0.996 | [0.966, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 74.31us | 72.35us | 1.033 | [0.960, 1.113] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.447s | 1.451s | 1.000 | [0.994, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.447s | 1.450s | 1.000 | [0.994, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.88us | 112.09us | 0.948 | [0.756, 1.189] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.260s | 1.262s | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.260s | 1.262s | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 65.30us | 63.00us | 1.039 | [0.968, 1.115] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.989s | 1.993s | 0.995 | [0.989, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.989s | 1.993s | 0.995 | [0.989, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.27us | 32.97us | 0.463 | [0.115, 1.867] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.90ms | 181.35ms | 1.015 | [0.996, 1.035] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 182.82ms | 181.27ms | 1.015 | [0.996, 1.035] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.68us | 27.24us | 1.012 | [0.707, 1.447] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 341.82ms | 335.87ms | 1.018 | [1.005, 1.031] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 341.71ms | 335.76ms | 1.018 | [1.005, 1.031] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.99us | 33.87us | 1.033 | [0.803, 1.329] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.06ms | 50.90ms | 1.015 | [0.977, 1.055] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.99ms | 50.83ms | 1.015 | [0.977, 1.055] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.37us | 19.77us | 1.038 | [0.884, 1.220] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.39ms | 110.37ms | 1.014 | [0.935, 1.099] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.26ms | 104.27ms | 1.012 | [0.932, 1.100] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.71ms | 1.63ms | 1.076 | [0.895, 1.294] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 437.83ms | 447.76ms | 0.982 | [0.957, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 434.81ms | 443.25ms | 0.985 | [0.959, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.23ms | 3.80ms | 0.651 | [0.371, 1.145] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.532s | 1.537s | 0.999 | [0.977, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.529s | 1.534s | 0.999 | [0.977, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.38ms | 2.21ms | 1.058 | [0.700, 1.602] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.472s | 2.482s | 0.992 | [0.970, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.469s | 2.479s | 0.992 | [0.970, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.78ms | 0.989 | [0.912, 1.073] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 742.38ms | 732.80ms | 1.025 | [0.962, 1.092] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 738.71ms | 729.02ms | 1.025 | [0.962, 1.093] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.00ms | 3.04ms | 0.981 | [0.963, 0.999] | SLOWER |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 483.15ms | 484.40ms | 1.012 | [0.972, 1.054] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.52ms | 475.78ms | 1.013 | [0.972, 1.055] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.49ms | 7.61ms | 0.989 | [0.973, 1.006] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.87ms | 26.54ms | 1.019 | [1.000, 1.037] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.18ms | 23.84ms | 1.022 | [0.971, 1.075] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.42ms | 2.44ms | 0.991 | [0.985, 0.997] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.10ms | 3.16ms | 0.967 | [0.885, 1.057] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.08ms | 2.06ms | 1.021 | [0.943, 1.106] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 809.66us | 882.23us | 0.860 | [0.666, 1.109] | ~ (noise) |

Sum of medians (total, common cases): A = 38.423s, B = 38.462s, saved = -38340671.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.02e-15 -> B 2.15e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.7 MiB | 1.000 | 0.1% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.6 MiB | 15.4 MiB | 0.983 | 1.2% / 2.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.3 MiB | 14.3 MiB | 1.002 | 2.9% / 2.5% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.6 MiB | 12.9 MiB | 1.021 | 4.6% / 4.8% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 25.7 MiB | 0.993 | 2.3% / 4.2% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.2 MiB | 53.4 MiB | 1.005 | 7.1% / 4.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 57.5 MiB | 57.4 MiB | 0.999 | 7.0% / 8.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.5 MiB | 0.994 | 1.1% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1071.5 MiB | 1072.5 MiB | 1.001 | 1.9% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.8 MiB | 40.1 MiB | 0.982 | 6.5% / 8.6% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.4 MiB | 1.009 | 2.0% / 2.6% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.9 MiB | 14.8 MiB | 0.992 | 3.2% / 6.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 12.9 MiB | 0.991 | 6.3% / 1.1% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 62.3 MiB | 0.998 | 3.5% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.1 MiB | 26.1 MiB | 1.001 | 1.9% / 3.4% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.1 MiB | 42.7 MiB | 1.014 | 3.8% / 5.3% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.3 MiB | 41.6 MiB | 0.983 | 4.0% / 1.6% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.3 MiB | 149.9 MiB | 0.997 | 1.5% / 2.6% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.3 MiB | 217.2 MiB | 1.000 | 1.3% / 0.1% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.4 MiB | 25.4 MiB | 1.004 | 4.0% / 2.9% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 43.0 MiB | 42.5 MiB | 0.989 | 5.4% / 3.6% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.5 MiB | 42.1 MiB | 0.991 | 2.9% / 5.0% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.7 MiB | 14.7 MiB | 1.003 | 1.7% / 3.4% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 1.000 | 1.5% / 0.6% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 11.9 MiB | 0.992 | 1.4% / 2.1% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 12.8 MiB | 0.980 | 2.3% / 4.9% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.8 MiB | 24.4 MiB | 0.980 | 4.1% / 4.7% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.8 MiB | 0.994 | 3.9% / 2.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 11.9 MiB | 0.998 | 2.7% / 2.6% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 11.8 MiB | 0.997 | 1.9% / 2.0% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.4 MiB | 19.4 MiB | 1.000 | 2.2% / 3.1% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 12.8 MiB | 0.983 | 1.8% / 3.7% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.8 MiB | 0.992 | 3.8% / 4.1% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.1 MiB | 13.6 MiB | 1.034 | 7.1% / 5.6% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.3 MiB | 1.011 | 3.4% / 4.5% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.2 MiB | 1.007 | 4.3% / 3.4% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.8 MiB | 1.002 | 1.0% / 1.9% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.2 MiB | 0.996 | 3.3% / 1.4% | ~ |

Peak memory (common cases): geomean B/A = 0.998; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.1 MiB -> 13.6 MiB (1.034x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 163, 162, 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 812s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
