A/B `e2e`: A=`WP-26-after-r` B=`WP-26-final`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 399.32ms | 405.63ms | 0.968 | [0.867, 1.080] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 325.96ms | 332.04ms | 0.977 | [0.874, 1.091] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.37ms | 32.17ms | 0.999 | [0.897, 1.112] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.76ms | 247.63ms | 1.001 | [0.990, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.18ms | 245.20ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.99ms | 1.92ms | 1.023 | [0.896, 1.167] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.79ms | 497.06ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.25ms | 494.65ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.79ms | 1.78ms | 1.009 | [0.953, 1.068] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 915.65ms | 916.14ms | 1.002 | [0.993, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 913.23ms | 913.74ms | 1.002 | [0.992, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.18ms | 2.16ms | 0.858 | [0.598, 1.231] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 301.28ms | 304.76ms | 0.994 | [0.975, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 292.66ms | 296.61ms | 0.993 | [0.976, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.35ms | 5.11ms | 1.022 | [0.749, 1.393] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 519.42ms | 520.15ms | 1.003 | [0.986, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.96ms | 494.98ms | 1.006 | [0.989, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 14.18ms | 15.75ms | 0.932 | [0.832, 1.044] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.73ms | 232.09ms | 1.007 | [0.974, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.18ms | 205.53ms | 0.999 | [0.993, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.17ms | 18.16ms | 1.046 | [0.758, 1.443] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.398s | 2.400s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.396s | 2.397s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.84ms | 1.88ms | 0.998 | [0.899, 1.109] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.245s | 1.203s | 1.038 | [1.012, 1.065] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.149s | 1.124s | 1.031 | [0.996, 1.066] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.64ms | 74.40ms | 1.142 | [0.989, 1.320] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.64ms | 87.04ms | 0.996 | [0.978, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.06ms | 84.22ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.06ms | 0.974 | [0.909, 1.044] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.07ms | 321.21ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.36ms | 318.39ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.22ms | 2.31ms | 0.982 | [0.917, 1.052] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 691.69ms | 691.51ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.06ms | 688.67ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.92ms | 1.94ms | 0.967 | [0.894, 1.045] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.304s | 1.301s | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.301s | 1.298s | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.36ms | 2.49ms | 0.882 | [0.640, 1.217] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.52ms | 92.60ms | 0.995 | [0.984, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.98ms | 91.27ms | 0.995 | [0.985, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 994.46us | 1.03ms | 0.965 | [0.789, 1.180] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.73ms | 201.90ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.20ms | 199.53ms | 0.999 | [0.994, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.11ms | 1.07ms | 1.050 | [0.774, 1.423] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.136s | 1.137s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.128s | 1.129s | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.00ms | 3.03ms | 0.864 | [0.589, 1.267] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.411s | 5.408s | 1.006 | [0.996, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.402s | 5.398s | 1.006 | [0.996, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.23ms | 3.35ms | 1.034 | [0.628, 1.705] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.228s | 1.221s | 1.009 | [0.999, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.153s | 1.145s | 1.008 | [0.998, 1.019] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.83ms | 43.45ms | 0.986 | [0.845, 1.151] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.226s | 1.226s | 0.998 | [0.993, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.125s | 0.996 | [0.990, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 62.32ms | 59.95ms | 1.040 | [0.981, 1.101] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.10ms | 248.70ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.16ms | 244.98ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.36ms | 2.47ms | 0.967 | [0.900, 1.040] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.439s | 1.439s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.430s | 1.429s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.05ms | 4.46ms | 0.934 | [0.770, 1.134] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.784s | 6.771s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.775s | 6.760s | 1.000 | [0.995, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.59ms | 4.60ms | 0.966 | [0.632, 1.477] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.95ms | 173.95ms | 1.023 | [0.972, 1.077] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 174.54ms | 173.58ms | 1.023 | [0.972, 1.077] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 204.07us | 208.13us | 1.004 | [0.919, 1.096] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 241.13ms | 242.31ms | 0.995 | [0.975, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 240.98ms | 242.16ms | 0.995 | [0.975, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.54us | 74.80us | 0.987 | [0.916, 1.062] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.448s | 1.444s | 1.005 | [0.994, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.447s | 1.444s | 1.005 | [0.994, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 113.28us | 117.05us | 1.021 | [0.774, 1.348] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.255s | 1.258s | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.255s | 1.258s | 0.999 | [0.986, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.98us | 66.91us | 1.009 | [0.955, 1.065] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.970s | 1.974s | 0.999 | [0.988, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.969s | 1.974s | 0.999 | [0.988, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.13us | 34.78us | 0.993 | [0.885, 1.114] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.34ms | 183.14ms | 1.005 | [0.965, 1.048] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.26ms | 183.07ms | 1.005 | [0.965, 1.048] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 29.05us | 27.94us | 0.976 | [0.682, 1.397] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 334.47ms | 336.11ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 334.36ms | 335.99ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 33.80us | 34.36us | 0.967 | [0.752, 1.245] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.59ms | 50.90ms | 0.992 | [0.967, 1.017] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.52ms | 50.83ms | 0.992 | [0.967, 1.017] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.09us | 21.79us | 0.979 | [0.841, 1.139] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.79ms | 108.71ms | 1.001 | [0.964, 1.040] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.65ms | 102.72ms | 1.001 | [0.961, 1.044] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.69ms | 1.68ms | 0.996 | [0.913, 1.087] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 443.80ms | 445.81ms | 0.996 | [0.971, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 440.89ms | 442.81ms | 0.996 | [0.970, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.15ms | 2.27ms | 0.940 | [0.605, 1.458] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.533s | 1.524s | 1.009 | [0.986, 1.033] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.531s | 1.519s | 1.009 | [0.987, 1.032] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.21ms | 2.15ms | 0.889 | [0.588, 1.345] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.429s | 2.452s | 0.994 | [0.974, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.426s | 2.449s | 0.994 | [0.974, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 1.82ms | 0.971 | [0.898, 1.049] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 734.86ms | 734.63ms | 0.997 | [0.945, 1.052] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 731.00ms | 730.84ms | 0.999 | [0.947, 1.054] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.01ms | 3.06ms | 0.775 | [0.396, 1.517] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 482.95ms | 485.22ms | 0.975 | [0.910, 1.044] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.45ms | 476.74ms | 0.975 | [0.910, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.48ms | 7.51ms | 0.941 | [0.797, 1.110] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.15ms | 26.89ms | 0.979 | [0.919, 1.043] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.45ms | 24.18ms | 0.976 | [0.909, 1.048] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.43ms | 1.003 | [0.992, 1.013] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.30ms | 3.25ms | 1.010 | [0.817, 1.249] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.20ms | 2.14ms | 1.054 | [0.884, 1.256] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 867.00us | 870.08us | 0.930 | [0.552, 1.565] | ~ (noise) |

Sum of medians (total, common cases): A = 38.349s, B = 38.314s, saved = 35.39ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 5.65e-16 -> B 2.26e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.7 MiB | 1.000 | 0.0% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.5 MiB | 15.6 MiB | 1.009 | 0.9% / 4.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.5 MiB | 14.6 MiB | 1.009 | 2.7% / 5.1% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.006 | 0.9% / 2.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.7 MiB | 26.1 MiB | 1.015 | 4.0% / 3.3% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 51.9 MiB | 53.4 MiB | 1.030 | 3.8% / 3.9% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.8 MiB | 58.0 MiB | 0.969 | 11.1% / 10.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.5 MiB | 14.6 MiB | 1.008 | 3.5% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1072.6 MiB | 1071.8 MiB | 0.999 | 1.3% / 1.3% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.9 MiB | 40.9 MiB | 1.001 | 8.6% / 5.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.3 MiB | 17.5 MiB | 1.011 | 1.2% / 4.9% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.0 MiB | 14.8 MiB | 0.986 | 2.2% / 1.8% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.0 MiB | 0.992 | 3.0% / 3.7% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.7 MiB | 62.4 MiB | 0.995 | 1.3% / 2.8% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.1 MiB | 25.9 MiB | 0.990 | 6.2% / 5.9% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.3 MiB | 42.2 MiB | 1.022 | 4.0% / 5.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.6 MiB | 41.7 MiB | 1.003 | 3.3% / 2.6% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.3 MiB | 150.9 MiB | 1.004 | 3.3% / 4.2% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.8 MiB | 217.3 MiB | 0.998 | 0.4% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.5 MiB | 25.6 MiB | 1.002 | 3.3% / 1.5% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 42.6 MiB | 43.6 MiB | 1.025 | 3.1% / 3.1% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.1 MiB | 42.1 MiB | 1.000 | 2.8% / 6.1% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 15.0 MiB | 1.002 | 2.8% / 2.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.1 MiB | 1.008 | 1.9% / 0.5% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.1 MiB | 1.010 | 2.2% / 1.5% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.0 MiB | 13.0 MiB | 1.001 | 2.7% / 2.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.0 MiB | 24.3 MiB | 0.976 | 4.9% / 6.4% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.0 MiB | 1.004 | 2.5% / 1.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.8 MiB | 12.0 MiB | 1.015 | 3.0% / 1.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 11.9 MiB | 1.011 | 2.8% / 2.0% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.6 MiB | 0.995 | 3.7% / 0.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.0 MiB | 0.997 | 3.8% / 6.8% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 13.0 MiB | 1.022 | 2.1% / 2.3% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.3 MiB | 1.009 | 2.5% / 5.5% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.2 MiB | 16.3 MiB | 1.003 | 1.7% / 4.4% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.2 MiB | 17.2 MiB | 1.000 | 3.0% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.9 MiB | 12.9 MiB | 1.003 | 1.1% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.3 MiB | 12.3 MiB | 1.005 | 1.2% / 3.4% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `quad/sg/equi-exp/b1/d15/L6` 51.9 MiB -> 53.4 MiB (1.030x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.3%, 0.2%
Round durations (s): 162, 162, 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 809s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
