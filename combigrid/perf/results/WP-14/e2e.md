A/B `e2e`: A=`WP-14-before` B=`WP-14-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 414.33ms | 401.03ms | 1.017 | [0.911, 1.135] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 342.00ms | 330.08ms | 1.039 | [0.930, 1.160] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.67ms | 32.04ms | 0.948 | [0.852, 1.055] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 250.07ms | 252.78ms | 0.993 | [0.973, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 247.29ms | 250.11ms | 0.996 | [0.976, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.17ms | 2.72ms | 0.776 | [0.457, 1.318] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 499.90ms | 502.54ms | 0.997 | [0.987, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 497.54ms | 500.13ms | 0.998 | [0.987, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.80ms | 1.80ms | 0.980 | [0.891, 1.078] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 907.68ms | 934.38ms | 0.987 | [0.950, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 904.33ms | 932.21ms | 0.987 | [0.950, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.18ms | 2.19ms | 1.041 | [0.625, 1.734] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 297.96ms | 311.27ms | 0.962 | [0.946, 0.977] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | operator | 289.30ms | 302.80ms | 0.962 | [0.948, 0.976] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.25ms | 5.44ms | 0.907 | [0.750, 1.097] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 521.16ms | 521.29ms | 0.981 | [0.933, 1.032] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 497.07ms | 496.95ms | 0.982 | [0.937, 1.030] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.55ms | 15.68ms | 0.944 | [0.813, 1.094] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.78ms | 239.31ms | 0.971 | [0.920, 1.025] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 204.47ms | 208.44ms | 0.965 | [0.906, 1.027] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.50ms | 19.72ms | 1.076 | [0.796, 1.454] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.412s | 2.412s | 0.988 | [0.955, 1.022] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.409s | 2.410s | 0.989 | [0.957, 1.021] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.83ms | 1.92ms | 0.776 | [0.272, 2.210] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.204s | 1.220s | 0.991 | [0.897, 1.095] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.125s | 1.130s | 0.992 | [0.900, 1.094] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.69ms | 77.36ms | 1.030 | [0.844, 1.257] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.58ms | 87.40ms | 1.005 | [0.991, 1.019] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.91ms | 84.86ms | 1.004 | [0.992, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.16ms | 2.06ms | 1.077 | [0.994, 1.166] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 329.29ms | 328.38ms | 1.006 | [0.997, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 326.08ms | 325.58ms | 1.004 | [0.995, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.29ms | 2.29ms | 1.185 | [0.852, 1.647] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 699.11ms | 701.65ms | 0.997 | [0.992, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 696.55ms | 699.13ms | 0.997 | [0.992, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.95ms | 1.96ms | 1.001 | [0.929, 1.080] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.319s | 1.312s | 1.006 | [1.001, 1.012] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.316s | 1.309s | 1.007 | [1.001, 1.012] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.24ms | 2.39ms | 0.963 | [0.519, 1.786] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 98.44ms | 96.70ms | 1.023 | [1.011, 1.035] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 97.13ms | 95.44ms | 1.022 | [1.011, 1.032] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.03ms | 982.24us | 1.128 | [0.922, 1.380] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.41ms | 213.35ms | 0.984 | [0.974, 0.993] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 207.82ms | 210.78ms | 0.983 | [0.974, 0.992] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.46ms | 1.14ms | 1.170 | [0.907, 1.509] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.189s | 1.217s | 0.976 | [0.971, 0.980] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.181s | 1.210s | 0.975 | [0.971, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.09ms | 3.04ms | 1.014 | [0.959, 1.073] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.521s | 5.659s | 0.976 | [0.972, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.512s | 5.650s | 0.976 | [0.972, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.22ms | 4.13ms | 1.104 | [0.832, 1.466] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.890s | 4.090s | 0.952 | [0.948, 0.956] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.830s | 4.025s | 0.951 | [0.947, 0.954] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 36.53ms | 36.32ms | 1.040 | [0.891, 1.214] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.669s | 7.029s | 0.949 | [0.945, 0.952] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.574s | 6.937s | 0.948 | [0.944, 0.951] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 51.51ms | 51.15ms | 1.003 | [0.973, 1.033] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 257.60ms | 258.26ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 253.78ms | 254.59ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.23ms | 2.27ms | 0.991 | [0.860, 1.141] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.488s | 1.510s | 0.988 | [0.980, 0.995] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.479s | 1.501s | 0.987 | [0.980, 0.994] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.90ms | 4.27ms | 1.042 | [0.873, 1.245] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.950s | 7.020s | 0.990 | [0.984, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.940s | 7.012s | 0.990 | [0.986, 0.994] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.63ms | 4.24ms | 1.036 | [0.678, 1.583] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 188.27ms | 176.27ms | 1.068 | [1.048, 1.088] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 187.95ms | 175.90ms | 1.068 | [1.049, 1.088] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 209.10us | 205.26us | 1.013 | [0.927, 1.106] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 267.53ms | 244.62ms | 1.085 | [1.063, 1.107] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 267.38ms | 244.46ms | 1.085 | [1.063, 1.107] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 75.71us | 72.63us | 1.243 | [0.692, 2.235] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.621s | 1.450s | 1.118 | [1.111, 1.125] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.620s | 1.449s | 1.118 | [1.111, 1.125] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 102.60us | 98.27us | 1.021 | [0.814, 1.281] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.409s | 1.260s | 1.118 | [1.104, 1.132] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.409s | 1.260s | 1.118 | [1.104, 1.132] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 68.58us | 63.70us | 1.035 | [0.937, 1.144] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.170s | 1.993s | 1.086 | [1.068, 1.104] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.170s | 1.993s | 1.086 | [1.068, 1.104] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.90us | 34.57us | 1.594 | [0.510, 4.986] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 183.32ms | 181.37ms | 1.000 | [0.972, 1.029] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 183.24ms | 181.29ms | 1.000 | [0.972, 1.029] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.26us | 27.73us | 0.970 | [0.678, 1.388] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 353.09ms | 337.30ms | 1.049 | [1.036, 1.063] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 352.98ms | 337.20ms | 1.049 | [1.036, 1.063] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.29us | 32.27us | 1.070 | [0.831, 1.377] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.88ms | 51.02ms | 1.021 | [1.001, 1.041] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.81ms | 50.95ms | 1.021 | [1.001, 1.042] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.47us | 22.00us | 1.016 | [0.874, 1.183] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.73ms | 109.33ms | 1.011 | [0.974, 1.050] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.60ms | 103.08ms | 1.011 | [0.970, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.73ms | 1.63ms | 1.082 | [0.995, 1.176] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 489.29ms | 438.12ms | 1.135 | [1.036, 1.243] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 485.47ms | 435.27ms | 1.134 | [1.035, 1.243] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.26ms | 2.13ms | 1.182 | [0.814, 1.718] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.676s | 1.502s | 1.114 | [1.089, 1.140] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.674s | 1.500s | 1.115 | [1.090, 1.140] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.25ms | 2.17ms | 0.951 | [0.591, 1.531] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.685s | 2.453s | 1.095 | [1.073, 1.118] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.683s | 2.451s | 1.095 | [1.073, 1.118] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.69ms | 1.055 | [0.947, 1.175] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 736.50ms | 742.01ms | 1.004 | [0.951, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 731.12ms | 736.19ms | 1.005 | [0.953, 1.060] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.65ms | 5.11ms | 0.850 | [0.669, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.803s | 1.999s | 0.916 | [0.867, 0.968] | SLOWER |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.62ms | 490.21ms | 0.968 | [0.931, 1.006] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.327s | 1.508s | 0.899 | [0.843, 0.959] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.702s | 2.094s | 0.812 | [0.807, 0.817] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.14ms | 23.56ms | 0.967 | [0.870, 1.075] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.677s | 2.070s | 0.810 | [0.805, 0.815] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.878s | 2.074s | 0.902 | [0.889, 0.916] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.15ms | 0.978 | [0.903, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.876s | 2.072s | 0.902 | [0.888, 0.916] | SLOWER |

Sum of medians (total, common cases): A = 52.771s, B = 53.424s, saved = -653584634.0ns (0.988x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.13e-16 -> B 2.26e-16 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 222, 221, 221, 220, 221 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1105s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
