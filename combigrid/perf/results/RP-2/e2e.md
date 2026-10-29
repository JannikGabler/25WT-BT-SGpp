A/B `e2e`: A=`baseline` B=`rp-2`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 712.11ms | 395.25ms | 1.832 | [1.642, 2.044] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 674.74ms | 326.65ms | 2.114 | [1.892, 2.362] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.08ms | 31.03ms | 0.984 | [0.883, 1.095] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 705.92ms | 252.61ms | 2.772 | [2.695, 2.851] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 702.92ms | 250.12ms | 2.788 | [2.711, 2.868] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.10ms | 1.89ms | 1.089 | [0.954, 1.243] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.629s | 501.73ms | 3.256 | [3.213, 3.298] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.623s | 499.34ms | 3.265 | [3.222, 3.308] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.97ms | 1.77ms | 1.305 | [0.761, 2.237] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.309s | 935.37ms | 3.524 | [3.500, 3.548] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.306s | 932.10ms | 3.530 | [3.507, 3.554] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.64ms | 2.10ms | 1.255 | [0.675, 2.333] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.112s | 303.92ms | 3.642 | [3.566, 3.719] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.103s | 295.43ms | 3.713 | [3.637, 3.790] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.94ms | 5.22ms | 1.143 | [0.938, 1.392] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.396s | 521.68ms | 4.654 | [4.431, 4.888] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.370s | 497.67ms | 4.817 | [4.586, 5.061] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.28ms | 15.42ms | 1.291 | [1.024, 1.627] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.118s | 235.25ms | 4.826 | [4.658, 5.001] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.085s | 209.62ms | 5.262 | [5.084, 5.445] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 22.89ms | 17.48ms | 1.351 | [1.000, 1.826] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.534s | 2.419s | 1.442 | [1.385, 1.501] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.531s | 2.416s | 1.442 | [1.385, 1.501] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 2.01ms | 1.85ms | 1.143 | [0.984, 1.326] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.377s | 1.191s | 1.154 | [1.109, 1.201] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.285s | 1.110s | 1.155 | [1.094, 1.218] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 85.73ms | 76.15ms | 1.061 | [0.839, 1.343] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 182.53ms | 87.80ms | 2.091 | [1.996, 2.190] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 180.37ms | 85.00ms | 2.129 | [2.039, 2.223] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.05ms | 1.080 | [0.809, 1.441] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 779.50ms | 325.27ms | 2.381 | [2.329, 2.435] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 776.68ms | 322.69ms | 2.392 | [2.342, 2.444] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.43ms | 2.13ms | 1.077 | [0.979, 1.185] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.829s | 701.97ms | 2.589 | [2.555, 2.624] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.826s | 699.40ms | 2.595 | [2.561, 2.630] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.02ms | 1.98ms | 1.043 | [0.968, 1.125] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.606s | 1.309s | 2.750 | [2.716, 2.786] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.604s | 1.307s | 2.756 | [2.722, 2.791] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.41ms | 3.50ms | 0.801 | [0.470, 1.364] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.54ms | 92.94ms | 1.144 | [1.128, 1.161] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.17ms | 91.62ms | 1.148 | [1.133, 1.162] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 1.04ms | 0.950 | [0.777, 1.163] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.43ms | 201.99ms | 1.107 | [1.102, 1.113] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.67ms | 199.29ms | 1.108 | [1.103, 1.114] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.37ms | 1.26ms | 1.077 | [0.835, 1.390] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.236s | 1.135s | 1.087 | [1.074, 1.101] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.224s | 1.128s | 1.087 | [1.074, 1.100] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.58ms | 3.02ms | 1.386 | [0.915, 2.100] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.663s | 5.406s | 1.048 | [1.044, 1.052] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.654s | 5.397s | 1.048 | [1.044, 1.052] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.79ms | 3.61ms | 1.231 | [0.822, 1.844] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.962s | 1.218s | 3.262 | [3.234, 3.290] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.897s | 1.145s | 3.407 | [3.387, 3.428] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.16ms | 43.20ms | 0.957 | [0.820, 1.117] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.758s | 1.225s | 5.517 | [5.497, 5.538] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.644s | 1.122s | 5.922 | [5.900, 5.944] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 73.17ms | 63.09ms | 1.137 | [1.072, 1.206] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.24ms | 248.03ms | 1.088 | [1.082, 1.095] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.29ms | 244.09ms | 1.088 | [1.082, 1.095] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.67ms | 2.28ms | 1.223 | [0.980, 1.526] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.529s | 1.440s | 1.060 | [1.055, 1.065] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.519s | 1.430s | 1.061 | [1.056, 1.066] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.09ms | 4.38ms | 1.111 | [0.940, 1.314] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.979s | 6.772s | 1.031 | [1.025, 1.037] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.971s | 6.762s | 1.031 | [1.026, 1.036] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.54ms | 4.28ms | 1.047 | [0.685, 1.601] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 237.52ms | 176.30ms | 1.344 | [1.319, 1.369] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 235.50ms | 175.92ms | 1.343 | [1.318, 1.368] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 216.51us | 216.09us | 1.536 | [0.464, 5.082] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 307.83ms | 245.26ms | 1.256 | [1.231, 1.282] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 307.59ms | 245.11ms | 1.256 | [1.231, 1.282] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 81.50us | 74.80us | 1.270 | [0.897, 1.799] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.768s | 1.475s | 1.196 | [1.071, 1.336] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.768s | 1.475s | 1.196 | [1.071, 1.336] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 123.41us | 99.73us | 1.182 | [0.942, 1.482] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.470s | 1.279s | 1.187 | [0.961, 1.468] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.470s | 1.279s | 1.187 | [0.961, 1.466] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 81.02us | 66.77us | 1.399 | [0.098, 19.886] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.251s | 1.988s | 1.183 | [1.046, 1.338] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.251s | 1.988s | 1.183 | [1.046, 1.337] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 40.86us | 35.27us | 2.433 | [0.316, 18.767] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.70ms | 181.78ms | 1.014 | [0.957, 1.075] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.62ms | 181.62ms | 1.014 | [0.957, 1.075] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.71us | 28.15us | 0.901 | [0.492, 1.652] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 366.19ms | 336.46ms | 1.079 | [1.025, 1.135] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 366.07ms | 336.34ms | 1.079 | [1.025, 1.135] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 40.51us | 34.85us | 1.161 | [0.902, 1.494] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.30ms | 51.03ms | 1.021 | [0.947, 1.100] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.23ms | 50.96ms | 1.021 | [0.947, 1.100] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.14us | 21.65us | 1.108 | [0.953, 1.290] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 112.47ms | 108.21ms | 1.031 | [0.993, 1.071] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 106.24ms | 102.05ms | 1.031 | [0.990, 1.075] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.71ms | 1.62ms | 1.048 | [0.938, 1.172] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 563.71ms | 443.83ms | 1.204 | [1.031, 1.406] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 558.80ms | 440.82ms | 1.205 | [1.031, 1.409] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.24ms | 2.29ms | 0.992 | [0.562, 1.753] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.689s | 1.584s | 1.098 | [1.006, 1.199] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.687s | 1.582s | 1.098 | [1.006, 1.199] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.32ms | 2.15ms | 1.058 | [0.699, 1.601] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.723s | 2.515s | 1.082 | [1.060, 1.105] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.720s | 2.512s | 1.082 | [1.060, 1.105] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.82ms | 1.75ms | 1.036 | [0.959, 1.119] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.201s | 740.17ms | 3.038 | [2.880, 3.205] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.196s | 734.92ms | 3.053 | [2.893, 3.220] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.73ms | 4.55ms | 1.027 | [1.002, 1.052] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.246s | 1.896s | 1.725 | [1.697, 1.753] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.870s | 477.19ms | 3.945 | [3.842, 4.050] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.380s | 1.406s | 0.978 | [0.966, 0.990] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.916s | 1.886s | 1.015 | [0.998, 1.031] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 103.69ms | 24.87ms | 4.204 | [3.946, 4.478] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.818s | 1.862s | 0.972 | [0.959, 0.987] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.916s | 1.829s | 1.047 | [1.039, 1.055] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 9.82ms | 2.08ms | 3.935 | [2.098, 7.381] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.905s | 1.827s | 1.043 | [1.034, 1.052] | faster |

Sum of medians (total, common cases): A = 70.025s, B = 43.655s, saved = 26.371s (1.604x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-sin/b0/d7/L7`: A 7.76e-12 -> B 1.29e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 363.7 MiB | 619.7 MiB | 1.704 | 0.2% / 0.1% | MEM+ |
| quad/sg/equi-exp/b0/d5/L10 | 15.4 MiB | 15.5 MiB | 1.010 | 4.1% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.3 MiB | 14.6 MiB | 1.021 | 4.9% / 2.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 12.7 MiB | 0.983 | 1.7% / 3.4% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.6 MiB | 26.0 MiB | 1.016 | 4.4% / 1.4% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 54.5 MiB | 52.8 MiB | 0.970 | 4.1% / 3.6% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 60.0 MiB | 56.9 MiB | 0.947 | 10.8% / 6.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.6 MiB | 0.989 | 5.6% / 5.0% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 833.3 MiB | 1072.2 MiB | 1.287 | 6.0% / 3.4% | MEM+ |
| quad/sg/cc-uq/b0/d3/L14 | 38.8 MiB | 41.3 MiB | 1.065 | 6.9% / 7.5% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.3 MiB | 1.005 | 3.9% / 4.8% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.7 MiB | 15.1 MiB | 1.030 | 2.6% / 3.6% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 12.9 MiB | 0.988 | 4.6% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.1 MiB | 62.6 MiB | 1.009 | 2.0% / 2.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.3 MiB | 25.9 MiB | 1.065 | 3.1% / 1.6% | MEM+ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.6 MiB | 42.5 MiB | 1.132 | 3.2% / 6.0% | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 36.6 MiB | 41.7 MiB | 1.139 | 2.9% / 4.3% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 230.7 MiB | 150.9 MiB | 0.654 | 1.1% / 2.3% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 330.6 MiB | 217.1 MiB | 0.657 | 2.2% / 1.4% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 24.3 MiB | 25.5 MiB | 1.046 | 3.3% / 2.7% | MEM+ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.2 MiB | 43.0 MiB | 1.157 | 3.0% / 4.7% | MEM+ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.3 MiB | 42.6 MiB | 1.143 | 1.5% / 3.4% | MEM+ |
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.7 MiB | 0.988 | 2.5% / 3.0% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.1 MiB | 1.007 | 1.4% / 2.7% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.1 MiB | 1.000 | 1.9% / 2.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.0 MiB | 12.9 MiB | 0.993 | 4.5% / 2.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.9 MiB | 24.7 MiB | 0.994 | 4.7% / 4.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.0 MiB | 1.002 | 3.3% / 1.8% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.0 MiB | 0.998 | 1.2% / 1.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.7 MiB | 11.9 MiB | 1.013 | 2.5% / 0.8% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.6 MiB | 19.5 MiB | 0.997 | 0.9% / 2.1% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.2 MiB | 1.012 | 1.0% / 1.8% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 13.0 MiB | 1.011 | 2.9% / 3.4% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.5 MiB | 1.018 | 4.1% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.1 MiB | 0.996 | 3.3% / 0.9% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.4 MiB | 17.6 MiB | 1.012 | 2.1% / 4.3% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.7 MiB | 12.7 MiB | 0.997 | 3.4% / 4.3% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.1 MiB | 12.3 MiB | 1.011 | 4.2% / 4.2% | ~ |

Peak memory (common cases): geomean B/A = 1.017; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 363.7 MiB -> 619.7 MiB (1.704x); 8 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 1.5%, 0.4%, 0.4%, 0.3%, 0.3%
Round durations (s): 241, 235, 236, 236, 236 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1183s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
