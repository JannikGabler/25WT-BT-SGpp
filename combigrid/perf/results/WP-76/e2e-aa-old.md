A/B `e2e`: A=`WP-76-before` B=`WP-76-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 362.57ms | 334.38ms | 1.065 | [0.954, 1.188] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 323.78ms | 297.65ms | 1.073 | [0.961, 1.199] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 36.66ms | 36.50ms | 0.997 | [0.895, 1.110] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.85ms | 248.51ms | 1.065 | [1.053, 1.077] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 263.17ms | 246.06ms | 1.069 | [1.057, 1.081] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.62ms | 1.55ms | 0.750 | [0.385, 1.459] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 513.13ms | 498.14ms | 1.030 | [1.020, 1.041] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.67ms | 496.48ms | 1.031 | [1.020, 1.042] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.33ms | 1.51ms | 0.797 | [0.498, 1.275] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 912.11ms | 904.16ms | 1.008 | [0.990, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 909.90ms | 901.82ms | 1.008 | [0.991, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.03ms | 2.29ms | 0.828 | [0.472, 1.452] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 248.62ms | 241.35ms | 1.028 | [0.988, 1.070] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 246.35ms | 239.08ms | 1.030 | [0.990, 1.071] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.54ms | 1.59ms | 0.920 | [0.760, 1.112] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.58ms | 491.50ms | 1.002 | [0.980, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 481.77ms | 482.08ms | 0.996 | [0.982, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.72ms | 6.71ms | 1.238 | [0.673, 2.276] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.19ms | 213.37ms | 1.001 | [0.988, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.35ms | 201.49ms | 1.001 | [0.996, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.52ms | 8.49ms | 1.002 | [0.741, 1.354] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.409s | 2.407s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.408s | 2.406s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.48ms | 1.44ms | 1.230 | [0.735, 2.058] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 453.65ms | 437.00ms | 1.023 | [0.985, 1.062] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 377.52ms | 356.27ms | 1.039 | [0.985, 1.095] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 68.89ms | 69.65ms | 0.935 | [0.809, 1.081] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.25ms | 75.28ms | 1.014 | [0.999, 1.028] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.97ms | 73.99ms | 1.014 | [1.001, 1.027] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.26ms | 1.25ms | 1.016 | [0.980, 1.054] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.07ms | 321.28ms | 1.006 | [0.985, 1.028] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.81ms | 319.60ms | 1.001 | [0.988, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.82ms | 1.71ms | 1.613 | [0.744, 3.496] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 691.98ms | 689.45ms | 1.003 | [0.988, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 690.35ms | 687.26ms | 1.003 | [0.988, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.45ms | 1.46ms | 1.009 | [0.835, 1.220] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.315s | 1.312s | 1.005 | [0.993, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.313s | 1.309s | 1.005 | [0.992, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 1.82ms | 2.54ms | 0.716 | [0.444, 1.156] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.19ms | 93.31ms | 1.038 | [1.018, 1.059] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.43ms | 92.34ms | 1.036 | [1.018, 1.054] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 868.54us | 597.77us | 1.501 | [0.895, 2.518] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 213.87ms | 202.78ms | 1.056 | [1.035, 1.078] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 213.30ms | 202.19ms | 1.057 | [1.036, 1.078] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 455.99us | 470.73us | 0.957 | [0.742, 1.234] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.192s | 1.151s | 1.039 | [1.021, 1.057] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.191s | 1.150s | 1.039 | [1.021, 1.057] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 177.53us | 200.93us | 0.733 | [0.419, 1.283] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.433s | 5.403s | 1.008 | [1.000, 1.016] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.432s | 5.402s | 1.008 | [1.000, 1.016] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 465.14us | 474.15us | 0.863 | [0.428, 1.741] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.170s | 1.169s | 1.003 | [0.996, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.160s | 1.154s | 1.005 | [1.001, 1.008] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 5.05ms | 6.44ms | 0.861 | [0.424, 1.747] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.150s | 1.136s | 1.002 | [0.980, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.131s | 1.117s | 1.002 | [0.980, 1.026] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 11.18ms | 11.10ms | 1.006 | [0.976, 1.036] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 245.25ms | 247.46ms | 0.998 | [0.982, 1.013] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.51ms | 246.69ms | 0.998 | [0.982, 1.013] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 650.36us | 648.96us | 0.999 | [0.929, 1.073] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.448s | 1.451s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.447s | 1.450s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 223.84us | 226.42us | 0.985 | [0.860, 1.128] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.885s | 6.872s | 1.006 | [0.994, 1.018] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.884s | 6.871s | 1.006 | [0.994, 1.018] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 595.18us | 635.83us | 0.813 | [0.532, 1.243] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.57ms | 128.08ms | 0.989 | [0.958, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.30ms | 127.83ms | 0.989 | [0.958, 1.022] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 225.66us | 217.06us | 0.977 | [0.885, 1.078] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 187.33ms | 186.62ms | 1.004 | [0.973, 1.035] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 187.17ms | 186.48ms | 1.004 | [0.973, 1.036] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 89.26us | 88.91us | 1.032 | [0.957, 1.113] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.158s | 1.137s | 1.003 | [0.959, 1.049] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.158s | 1.136s | 1.003 | [0.959, 1.049] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 79.62us | 71.87us | 1.064 | [0.849, 1.335] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.006s | 1.029s | 0.985 | [0.963, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.006s | 1.029s | 0.985 | [0.963, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 69.98us | 67.61us | 1.096 | [0.911, 1.318] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.624s | 1.610s | 1.006 | [0.990, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.624s | 1.610s | 1.006 | [0.990, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 50.91us | 42.39us | 1.235 | [1.097, 1.391] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.07ms | 34.04ms | 0.982 | [0.947, 1.019] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.00ms | 33.96ms | 0.982 | [0.947, 1.019] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 34.64us | 40.72us | 0.850 | [0.594, 1.216] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.58ms | 117.45ms | 0.994 | [0.981, 1.007] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.50ms | 117.36ms | 0.994 | [0.981, 1.007] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 38.90us | 41.48us | 0.972 | [0.724, 1.306] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 27.16ms | 11.87ms | 1.669 | [0.364, 7.646] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 27.07ms | 11.79ms | 1.673 | [0.363, 7.702] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 38.62us | 33.45us | 1.053 | [0.837, 1.324] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 53.63ms | 52.06ms | 1.083 | [0.972, 1.207] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 53.27ms | 51.67ms | 1.084 | [0.972, 1.208] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 222.58us | 219.02us | 1.003 | [0.933, 1.079] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.08ms | 42.90ms | 1.005 | [1.000, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.93ms | 42.75ms | 1.005 | [1.000, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 61.74us | 61.95us | 1.010 | [0.984, 1.038] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 173.99ms | 174.53ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 173.88ms | 174.41ms | 1.000 | [0.992, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 50.29us | 50.63us | 0.972 | [0.851, 1.111] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.22ms | 151.69ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.13ms | 151.60ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 44.21us | 45.54us | 0.972 | [0.951, 0.994] | SLOWER |
| interp/dsg/cc/b0/d10/L2/p8 | total | 398.18ms | 398.59ms | 1.006 | [0.988, 1.024] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 398.10ms | 398.52ms | 1.006 | [0.988, 1.024] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 36.25us | 36.25us | 0.994 | [0.746, 1.325] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.55ms | 6.51ms | 1.006 | [0.929, 1.089] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.49ms | 6.43ms | 1.006 | [0.928, 1.091] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 25.21us | 24.79us | 1.005 | [0.951, 1.063] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.42ms | 111.17ms | 1.001 | [0.961, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.76ms | 104.49ms | 1.000 | [0.959, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.44ms | 2.36ms | 1.026 | [0.944, 1.115] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 358.11ms | 360.21ms | 1.001 | [0.976, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 357.58ms | 359.69ms | 1.001 | [0.975, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 88.98us | 80.60us | 1.071 | [0.851, 1.347] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.204s | 1.199s | 1.009 | [0.984, 1.035] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.202s | 1.195s | 1.009 | [0.984, 1.034] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 3.59ms | 2.04ms | 1.129 | [0.553, 2.307] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.912s | 1.916s | 0.995 | [0.946, 1.047] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.910s | 1.913s | 0.995 | [0.946, 1.047] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.60ms | 1.57ms | 0.916 | [0.654, 1.284] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 159.83ms | 160.88ms | 0.994 | [0.985, 1.002] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 154.44ms | 155.23ms | 0.994 | [0.985, 1.003] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.28ms | 1.24ms | 1.047 | [0.962, 1.140] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 121.38ms | 120.84ms | 1.006 | [0.992, 1.021] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 120.78ms | 120.25ms | 1.006 | [0.992, 1.021] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 123.06us | 136.33us | 0.928 | [0.812, 1.061] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 238.13ms | 239.13ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 235.92ms | 236.41ms | 1.003 | [0.991, 1.015] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.04ms | 2.11ms | 0.828 | [0.567, 1.208] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 271.72ms | 271.24ms | 1.001 | [0.996, 1.007] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 270.12ms | 269.82ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.46ms | 1.28ms | 1.241 | [0.749, 2.056] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 759.96ms | 692.10ms | 1.080 | [1.024, 1.139] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 756.73ms | 688.87ms | 1.080 | [1.024, 1.140] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.67ms | 2.68ms | 0.997 | [0.985, 1.008] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 378.14ms | 373.05ms | 1.002 | [0.977, 1.028] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 370.40ms | 365.35ms | 1.002 | [0.976, 1.029] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.16ms | 7.17ms | 0.999 | [0.987, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.55ms | 26.93ms | 0.991 | [0.961, 1.022] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.99ms | 24.36ms | 0.990 | [0.941, 1.041] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.37ms | 2.39ms | 0.997 | [0.988, 1.006] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.07ms | 3.04ms | 1.002 | [0.990, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.13ms | 2.13ms | 0.970 | [0.883, 1.066] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 767.41us | 762.59us | 1.085 | [0.877, 1.342] | ~ (noise) |

Sum of medians (total, common cases): A = 36.758s, B = 36.454s, saved = 303.90ms (1.008x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 3.29e-09 -> B 6.84e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 110.7 MiB | 124.5 MiB | 1.125 | 24.8% / 24.7% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.6 MiB | 12.8 MiB | 1.012 | 3.9% / 1.8% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.006 | 1.7% / 2.3% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.005 | 2.7% / 0.9% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.1 MiB | 13.3 MiB | 1.020 | 1.4% / 1.1% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.2 MiB | 18.3 MiB | 1.006 | 1.8% / 0.7% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.3 MiB | 1.007 | 2.0% / 1.0% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.7 MiB | 12.9 MiB | 1.013 | 2.6% / 2.4% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 194.2 MiB | 192.2 MiB | 0.990 | 7.2% / 8.4% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.0 MiB | 15.2 MiB | 1.018 | 7.2% / 6.1% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.8 MiB | 12.8 MiB | 1.003 | 3.6% / 1.8% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.7 MiB | 12.9 MiB | 1.011 | 2.8% / 1.2% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.007 | 2.4% / 0.7% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 44.0 MiB | 43.5 MiB | 0.990 | 2.6% / 1.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.1 MiB | 18.1 MiB | 1.000 | 3.3% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.5 MiB | 28.4 MiB | 0.999 | 4.8% / 3.9% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 29.7 MiB | 30.0 MiB | 1.010 | 3.0% / 2.7% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.5 MiB | 51.7 MiB | 1.003 | 0.3% / 0.4% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.7 MiB | 82.9 MiB | 1.003 | 0.2% / 0.4% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.1 MiB | 18.3 MiB | 1.011 | 1.9% / 1.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 28.6 MiB | 29.9 MiB | 1.044 | 4.9% / 7.3% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.2 MiB | 30.3 MiB | 1.001 | 2.2% / 2.5% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.4 MiB | 14.6 MiB | 1.017 | 2.7% / 2.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.4 MiB | 1.011 | 1.7% / 1.8% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.005 | 1.9% / 1.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.0 MiB | 12.2 MiB | 1.013 | 2.2% / 1.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.1 MiB | 12.2 MiB | 1.010 | 2.4% / 1.8% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.2 MiB | 0.995 | 4.0% / 2.4% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.2 MiB | 0.998 | 0.7% / 2.0% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.4 MiB | 1.011 | 3.5% / 1.2% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.2 MiB | 15.5 MiB | 1.017 | 3.7% / 2.5% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.4 MiB | 12.6 MiB | 1.014 | 2.8% / 2.9% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.5 MiB | 1.016 | 1.1% / 3.1% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.4 MiB | 1.003 | 1.4% / 1.4% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.3 MiB | 1.009 | 1.3% / 1.4% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.2 MiB | 1.000 | 1.2% / 3.2% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 20.1 MiB | 1.016 | 10.0% / 3.2% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.9 MiB | 1.008 | 2.7% / 4.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.5 MiB | 12.8 MiB | 1.024 | 2.7% / 3.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.5 MiB | 12.6 MiB | 1.007 | 3.1% / 1.7% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.3 MiB | 21.5 MiB | 1.010 | 5.2% / 3.8% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.3 MiB | 1.018 | 3.6% / 1.5% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.3 MiB | 12.4 MiB | 1.000 | 2.0% / 4.8% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 16.0 MiB | 16.0 MiB | 1.001 | 1.2% / 2.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.4 MiB | 13.4 MiB | 1.001 | 1.1% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.8 MiB | 15.0 MiB | 1.017 | 2.4% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.3 MiB | 1.009 | 1.0% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.6 MiB | 12.8 MiB | 1.016 | 2.1% / 2.8% | ~ |

Peak memory (common cases): geomean B/A = 1.011; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 110.7 MiB -> 124.5 MiB (1.125x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.7 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.4%, 0.3%, 0.4%
Round durations (s): 157, 157, 157, 157, 157 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 785s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
