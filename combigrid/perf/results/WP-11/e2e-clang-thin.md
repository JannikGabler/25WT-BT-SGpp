A/B `e2e`: A=`WP-11-gcc` B=`WP-11-clang-thin`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 419.90ms | 513.81ms | 0.832 | [0.745, 0.928] | SLOWER |
| quad/sg/equi-exp/b0/d2/L20 | operator | 356.60ms | 389.53ms | 0.917 | [0.821, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.25ms | 27.57ms | 1.160 | [1.042, 1.292] | faster |
| quad/sg/equi-exp/b0/d5/L10 | total | 269.45ms | 260.30ms | 1.036 | [1.024, 1.048] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 266.37ms | 259.00ms | 1.030 | [1.018, 1.042] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.94ms | 605.59us | 3.549 | [2.293, 5.494] | faster |
| quad/sg/equi-exp/b0/d7/L7 | total | 515.01ms | 540.37ms | 0.951 | [0.936, 0.966] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.90ms | 539.17ms | 0.946 | [0.933, 0.960] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.84ms | 541.41us | 2.945 | [2.155, 4.025] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 939.75ms | 922.09ms | 1.017 | [1.008, 1.026] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 935.17ms | 921.57ms | 1.014 | [1.007, 1.021] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 4.17ms | 245.70us | 12.177 | [7.416, 19.995] | faster |
| quad/sg/equi-exp/b1/d10/L7 | total | 305.54ms | 278.20ms | 1.102 | [1.082, 1.122] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 296.81ms | 270.49ms | 1.100 | [1.080, 1.122] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.95ms | 4.26ms | 1.373 | [1.135, 1.660] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 529.17ms | 540.32ms | 0.980 | [0.968, 0.992] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | operator | 503.45ms | 513.78ms | 0.981 | [0.967, 0.995] | SLOWER |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.70ms | 16.16ms | 1.031 | [0.920, 1.155] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 240.79ms | 237.77ms | 1.002 | [0.964, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.69ms | 207.90ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 21.70ms | 18.18ms | 1.108 | [0.780, 1.573] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.425s | 2.645s | 0.916 | [0.909, 0.924] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.422s | 2.644s | 0.916 | [0.908, 0.923] | SLOWER |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.94ms | 526.11us | 3.146 | [2.109, 4.694] | faster |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.258s | 1.222s | 0.993 | [0.910, 1.083] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.170s | 1.143s | 0.984 | [0.896, 1.080] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.56ms | 70.40ms | 1.170 | [1.013, 1.352] | faster |
| quad/sg/cc-uq/b0/d3/L14 | total | 89.52ms | 93.11ms | 0.967 | [0.953, 0.981] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | operator | 86.96ms | 91.20ms | 0.958 | [0.946, 0.970] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 1.78ms | 1.171 | [1.077, 1.272] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 342.59ms | 348.69ms | 0.982 | [0.976, 0.988] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | operator | 339.64ms | 347.09ms | 0.978 | [0.973, 0.984] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.42ms | 1.06ms | 2.178 | [1.953, 2.428] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 713.15ms | 743.44ms | 0.963 | [0.945, 0.982] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 710.43ms | 741.66ms | 0.962 | [0.943, 0.980] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.03ms | 981.68us | 2.110 | [1.848, 2.410] | faster |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.312s | 1.363s | 0.962 | [0.956, 0.968] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.309s | 1.362s | 0.960 | [0.955, 0.965] | SLOWER |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.45ms | 289.56us | 8.847 | [6.658, 11.755] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | total | 107.49ms | 99.34ms | 1.070 | [1.032, 1.110] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 106.13ms | 97.93ms | 1.072 | [1.032, 1.113] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 1.10ms | 0.985 | [0.805, 1.205] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.24ms | 209.86ms | 1.069 | [1.055, 1.082] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.62ms | 207.55ms | 1.067 | [1.053, 1.080] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.31ms | 789.27us | 1.592 | [1.201, 2.111] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.230s | 1.246s | 0.991 | [0.981, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.222s | 1.237s | 0.991 | [0.981, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.70ms | 3.80ms | 1.039 | [0.922, 1.172] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.664s | 5.565s | 1.021 | [1.013, 1.030] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.655s | 5.556s | 1.022 | [1.013, 1.030] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.18ms | 4.35ms | 1.003 | [0.755, 1.332] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.960s | 4.095s | 0.968 | [0.961, 0.974] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.895s | 4.010s | 0.970 | [0.965, 0.975] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 41.76ms | 42.47ms | 1.057 | [0.906, 1.234] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.767s | 7.112s | 0.951 | [0.946, 0.956] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.659s | 6.990s | 0.952 | [0.946, 0.958] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 74.16ms | 60.42ms | 1.159 | [1.038, 1.294] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 268.98ms | 264.82ms | 1.017 | [1.004, 1.031] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.20ms | 261.65ms | 1.015 | [1.003, 1.028] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.49ms | 1.97ms | 1.277 | [1.145, 1.425] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.519s | 1.562s | 0.972 | [0.967, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.508s | 1.551s | 0.972 | [0.967, 0.976] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.63ms | 4.84ms | 1.093 | [0.931, 1.283] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.973s | 7.104s | 0.983 | [0.977, 0.988] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.964s | 7.093s | 0.983 | [0.977, 0.988] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.28ms | 5.03ms | 0.969 | [0.634, 1.482] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 233.19ms | 185.07ms | 1.255 | [1.231, 1.278] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 232.83ms | 184.78ms | 1.255 | [1.232, 1.278] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 204.63us | 238.58us | 0.855 | [0.783, 0.934] | SLOWER |
| interp/sg/cc/b0/d3/L9/p64 | total | 308.31ms | 286.41ms | 1.060 | [1.023, 1.097] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 308.15ms | 284.77ms | 1.061 | [1.024, 1.100] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 79.20us | 95.89us | 0.414 | [0.099, 1.731] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.745s | 1.578s | 1.107 | [1.098, 1.116] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.745s | 1.577s | 1.107 | [1.098, 1.116] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 122.22us | 123.34us | 0.972 | [0.775, 1.218] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.476s | 1.354s | 1.088 | [1.074, 1.102] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.476s | 1.354s | 1.088 | [1.074, 1.102] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.62us | 86.81us | 0.975 | [0.855, 1.112] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.253s | 2.014s | 1.117 | [1.112, 1.123] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.253s | 2.013s | 1.117 | [1.112, 1.123] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 41.14us | 46.16us | 1.016 | [0.635, 1.628] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.70ms | 205.76ms | 0.922 | [0.883, 0.963] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.60ms | 205.64ms | 0.922 | [0.883, 0.963] | SLOWER |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 29.12us | 45.82us | 0.637 | [0.445, 0.911] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 373.23ms | 367.27ms | 1.030 | [1.005, 1.055] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 373.11ms | 367.12ms | 1.030 | [1.005, 1.056] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.74us | 62.51us | 0.705 | [0.548, 0.907] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.42ms | 58.33ms | 0.931 | [0.856, 1.012] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.34ms | 58.25ms | 0.931 | [0.856, 1.012] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 24.79us | 29.40us | 0.741 | [0.499, 1.101] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 114.73ms | 126.54ms | 0.895 | [0.808, 0.992] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 108.60ms | 119.26ms | 0.898 | [0.807, 0.998] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.76ms | 1.26ms | 1.548 | [1.164, 2.060] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 494.40ms | 463.43ms | 1.067 | [1.028, 1.108] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 491.45ms | 462.28ms | 1.063 | [1.025, 1.103] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.21ms | 286.77us | 8.424 | [7.095, 10.003] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.681s | 1.557s | 1.085 | [1.060, 1.110] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.679s | 1.556s | 1.084 | [1.060, 1.108] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.25ms | 273.22us | 8.838 | [5.841, 13.373] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.683s | 2.486s | 1.102 | [1.044, 1.164] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.681s | 2.485s | 1.102 | [1.043, 1.164] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.76ms | 729.84us | 2.498 | [2.292, 2.723] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 792.07ms | 758.99ms | 1.045 | [0.991, 1.102] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 786.66ms | 753.51ms | 1.045 | [0.991, 1.103] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.63ms | 4.68ms | 0.987 | [0.968, 1.006] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.854s | 1.436s | 1.289 | [1.268, 1.310] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 481.86ms | 487.47ms | 0.987 | [0.964, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.371s | 947.87ms | 1.444 | [1.427, 1.462] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.812s | 1.232s | 1.470 | [1.460, 1.481] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 25.04ms | 27.52ms | 0.899 | [0.854, 0.945] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.787s | 1.204s | 1.484 | [1.474, 1.494] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.890s | 1.180s | 1.597 | [1.577, 1.618] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.08ms | 2.32ms | 0.897 | [0.828, 0.971] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.888s | 1.177s | 1.599 | [1.578, 1.619] | faster |

Sum of medians (total, common cases): A = 54.025s, B = 52.253s, saved = 1.772s (1.034x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.76e-09 -> B 1.90e-08 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.2%, 0.1%, 0.1%, 0.1%
Round durations (s): 221, 221, 221, 221, 221 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1104s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
