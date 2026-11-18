A/B `e2e`: A=`rp-5` B=`rp-6`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 329.10ms | 306.86ms | 1.051 | [0.942, 1.173] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 291.88ms | 269.72ms | 1.060 | [0.949, 1.185] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.75ms | 37.79ms | 1.025 | [0.921, 1.141] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 261.89ms | 248.69ms | 1.053 | [1.041, 1.065] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 260.24ms | 247.22ms | 1.054 | [1.042, 1.066] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.55ms | 1.50ms | 0.954 | [0.775, 1.175] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 512.63ms | 496.44ms | 1.035 | [1.025, 1.046] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.15ms | 494.96ms | 1.035 | [1.025, 1.046] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.51ms | 1.36ms | 1.064 | [0.963, 1.175] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 915.39ms | 902.32ms | 1.013 | [1.007, 1.019] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 912.34ms | 900.00ms | 1.012 | [1.005, 1.018] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 3.45ms | 1.85ms | 1.490 | [0.661, 3.357] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 292.78ms | 241.15ms | 1.210 | [1.196, 1.224] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 290.54ms | 238.90ms | 1.211 | [1.197, 1.226] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.58ms | 1.55ms | 1.020 | [0.843, 1.233] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 503.42ms | 491.72ms | 1.034 | [1.014, 1.054] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 494.17ms | 481.73ms | 1.035 | [1.015, 1.055] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.60ms | 6.71ms | 0.983 | [0.878, 1.101] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 212.94ms | 213.24ms | 0.998 | [0.986, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.21ms | 201.38ms | 0.999 | [0.993, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.43ms | 8.51ms | 0.995 | [0.736, 1.344] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.404s | 2.397s | 1.002 | [0.994, 1.011] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.402s | 2.396s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.56ms | 1.40ms | 1.291 | [0.934, 1.785] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 792.47ms | 441.00ms | 1.783 | [1.702, 1.867] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 722.57ms | 360.41ms | 1.986 | [1.879, 2.099] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.86ms | 80.51ms | 0.870 | [0.753, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.43ms | 75.45ms | 1.015 | [1.001, 1.029] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 75.08ms | 74.10ms | 1.017 | [1.004, 1.029] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.13ms | 1.27ms | 0.920 | [0.807, 1.049] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.13ms | 322.16ms | 0.995 | [0.986, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 317.87ms | 320.19ms | 0.995 | [0.986, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.70ms | 1.65ms | 0.955 | [0.517, 1.763] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 686.18ms | 690.00ms | 0.996 | [0.985, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 684.71ms | 688.50ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.41ms | 1.38ms | 0.725 | [0.403, 1.305] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310s | 1.308s | 1.003 | [0.997, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.305s | 1.307s | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.01ms | 2.00ms | 1.237 | [0.637, 2.402] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.33ms | 93.80ms | 1.016 | [1.009, 1.022] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.34ms | 90.60ms | 1.040 | [1.035, 1.046] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 903.81us | 689.96us | 1.396 | [0.872, 2.234] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.57ms | 202.71ms | 1.044 | [1.039, 1.049] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 210.99ms | 201.88ms | 1.045 | [1.041, 1.049] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 476.66us | 455.22us | 1.034 | [0.801, 1.333] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.191s | 1.148s | 1.038 | [1.034, 1.042] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.190s | 1.147s | 1.038 | [1.034, 1.042] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 181.24us | 183.75us | 0.989 | [0.905, 1.080] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.428s | 5.406s | 1.004 | [1.000, 1.008] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.427s | 5.404s | 1.004 | [1.000, 1.008] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 226.63us | 464.86us | 0.774 | [0.403, 1.486] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.169s | 1.161s | 1.008 | [1.003, 1.013] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.160s | 1.147s | 1.013 | [1.008, 1.017] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.05ms | 4.08ms | 1.024 | [0.877, 1.195] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.138s | 1.136s | 1.001 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.119s | 1.109s | 1.008 | [1.003, 1.013] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 11.19ms | 11.24ms | 1.004 | [0.975, 1.035] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.07ms | 247.37ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.30ms | 246.37ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 644.56us | 642.12us | 1.009 | [0.939, 1.085] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.446s | 1.449s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.446s | 1.448s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 223.28us | 240.60us | 0.927 | [0.809, 1.061] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.890s | 6.871s | 1.006 | [0.994, 1.018] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.889s | 6.869s | 1.006 | [0.994, 1.018] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 266.65us | 661.88us | 0.597 | [0.328, 1.086] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 127.92ms | 128.76ms | 0.995 | [0.977, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 127.61ms | 128.49ms | 0.995 | [0.977, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 219.30us | 215.60us | 1.010 | [0.887, 1.149] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 186.74ms | 185.62ms | 1.008 | [0.987, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.61ms | 185.49ms | 1.008 | [0.987, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 87.79us | 88.21us | 1.005 | [0.933, 1.082] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.129s | 1.134s | 0.996 | [0.991, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.129s | 1.134s | 0.996 | [0.991, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 71.38us | 71.59us | 0.978 | [0.780, 1.227] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.005s | 1.009s | 0.996 | [0.983, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.005s | 1.009s | 0.996 | [0.983, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.19us | 65.37us | 1.065 | [0.804, 1.410] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.605s | 1.614s | 0.996 | [0.985, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.605s | 1.614s | 0.996 | [0.985, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 50.42us | 40.72us | 1.243 | [1.175, 1.315] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.21ms | 34.00ms | 1.006 | [0.993, 1.018] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.13ms | 33.91ms | 1.006 | [0.993, 1.019] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 34.36us | 42.39us | 0.793 | [0.554, 1.134] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.48ms | 117.29ms | 1.005 | [0.973, 1.038] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.40ms | 117.20ms | 1.005 | [0.973, 1.038] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 37.16us | 38.69us | 0.972 | [0.755, 1.250] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.84ms | 11.81ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.77ms | 11.74ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 33.38us | 32.97us | 0.942 | [0.810, 1.096] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 51.79ms | 52.17ms | 0.992 | [0.985, 0.999] | SLOWER |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.44ms | 51.82ms | 0.992 | [0.985, 0.998] | SLOWER |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 219.51us | 217.06us | 1.006 | [0.983, 1.029] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.05ms | 43.06ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.91ms | 42.91ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 60.62us | 60.34us | 1.007 | [0.996, 1.018] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 174.68ms | 175.38ms | 0.993 | [0.979, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 174.58ms | 175.27ms | 0.993 | [0.979, 1.007] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 49.03us | 49.66us | 0.977 | [0.914, 1.045] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.73ms | 151.99ms | 0.983 | [0.946, 1.023] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.65ms | 151.92ms | 0.983 | [0.946, 1.023] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 45.40us | 40.44us | 1.120 | [1.093, 1.149] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | total | 398.60ms | 398.91ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 398.54ms | 398.85ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 35.97us | 34.36us | 1.058 | [0.988, 1.133] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.51ms | 6.55ms | 0.979 | [0.949, 1.011] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.45ms | 6.49ms | 0.979 | [0.949, 1.011] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 24.45us | 24.86us | 0.993 | [0.963, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.47ms | 111.28ms | 0.991 | [0.954, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.85ms | 104.60ms | 0.989 | [0.949, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.50ms | 2.32ms | 1.089 | [0.945, 1.254] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 356.86ms | 361.77ms | 0.982 | [0.958, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 356.32ms | 361.24ms | 0.982 | [0.957, 1.008] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 80.11us | 81.02us | 1.038 | [0.900, 1.198] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.196s | 1.207s | 0.992 | [0.970, 1.015] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.194s | 1.203s | 0.993 | [0.971, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 1.97ms | 3.64ms | 0.657 | [0.434, 0.994] | SLOWER |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.845s | 1.864s | 0.993 | [0.973, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.842s | 1.861s | 0.993 | [0.973, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.59ms | 1.59ms | 0.989 | [0.486, 2.014] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 178.49ms | 155.98ms | 1.143 | [1.125, 1.161] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 173.19ms | 150.79ms | 1.149 | [1.131, 1.167] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.22ms | 1.25ms | 0.982 | [0.932, 1.034] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 181.63ms | 121.25ms | 1.513 | [1.466, 1.561] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 181.07ms | 120.65ms | 1.516 | [1.469, 1.564] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 80.11us | 140.52us | 0.578 | [0.568, 0.589] | SLOWER |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 372.96ms | 236.87ms | 1.578 | [1.540, 1.617] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 369.13ms | 234.56ms | 1.582 | [1.543, 1.622] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.12ms | 2.09ms | 1.198 | [0.773, 1.856] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 429.48ms | 271.80ms | 1.588 | [1.574, 1.602] | faster |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 428.06ms | 270.26ms | 1.593 | [1.582, 1.604] | faster |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.48ms | 1.43ms | 0.914 | [0.595, 1.403] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 777.08ms | 689.30ms | 1.132 | [1.073, 1.194] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 773.89ms | 686.11ms | 1.132 | [1.073, 1.194] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.65ms | 1.004 | [0.992, 1.016] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 489.87ms | 370.75ms | 1.330 | [1.284, 1.377] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 482.16ms | 363.08ms | 1.337 | [1.291, 1.385] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.15ms | 7.11ms | 0.998 | [0.973, 1.025] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.61ms | 26.65ms | 0.787 | [0.414, 1.498] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.69ms | 24.08ms | 0.990 | [0.941, 1.042] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.37ms | 2.39ms | 0.531 | [0.084, 3.336] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.08ms | 3.08ms | 0.990 | [0.865, 1.134] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 1.98ms | 2.12ms | 0.979 | [0.890, 1.077] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 783.82us | 801.98us | 1.019 | [0.747, 1.388] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | (not in A) | 1.536s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | (not in A) | 570.90ms | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | (not in A) | 1.037s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | (not in A) | 520.62ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | (not in A) | 2.156s | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | (not in A) | 881.46ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | (not in A) | 1.073s | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | (not in A) | 503.04ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | (not in A) | 825.90ms | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | (not in A) | 345.42ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | (not in A) | 2.478s | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | (not in A) | 397.87ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | (not in A) | 1.289s | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | (not in A) | 1.153s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | (not in A) | 349.73ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | (not in A) | 669.61ms | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | (not in A) | 332.92ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | (not in A) | 258.40ms | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | (not in A) | 98.95ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | (not in A) | 1.072s | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | (not in A) | 208.70ms | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | (not in A) | 578.04ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | (not in A) | 70.50ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | (not in A) | 243.44ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | (not in A) | 153.11ms | - | - | new case |

Sum of medians (total, common cases): A = 37.443s, B = 36.332s, saved = 1.111s (1.031x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.32e-09 -> B 6.33e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 111.4 MiB | 115.3 MiB | 1.035 | 24.0% / 19.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.6 MiB | 12.7 MiB | 1.011 | 3.2% / 2.7% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.7 MiB | 12.9 MiB | 1.015 | 2.2% / 3.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.7 MiB | 13.1 MiB | 1.032 | 1.5% / 2.3% | MEM+ |
| quad/sg/equi-exp/b1/d10/L7 | 13.3 MiB | 13.4 MiB | 1.010 | 3.1% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.2 MiB | 18.4 MiB | 1.012 | 2.4% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.1 MiB | 20.3 MiB | 1.006 | 1.6% / 0.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.6 MiB | 12.9 MiB | 1.025 | 1.8% / 1.7% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 284.4 MiB | 189.8 MiB | 0.667 | 35.4% / 1.3% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 17.5 MiB | 15.2 MiB | 0.868 | 6.1% / 4.6% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 13.0 MiB | 12.9 MiB | 0.994 | 4.2% / 1.8% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.0 MiB | 1.013 | 1.4% / 2.5% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.009 | 2.1% / 3.4% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.8 MiB | 43.5 MiB | 0.994 | 1.5% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.0 MiB | 18.0 MiB | 1.002 | 1.4% / 1.3% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 29.2 MiB | 28.9 MiB | 0.988 | 2.7% / 6.7% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 29.7 MiB | 30.2 MiB | 1.016 | 5.6% / 3.3% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.6 MiB | 51.8 MiB | 1.005 | 0.3% / 0.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.7 MiB | 83.0 MiB | 1.003 | 0.1% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.1 MiB | 18.2 MiB | 1.008 | 2.2% / 1.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.3 MiB | 28.9 MiB | 0.987 | 4.0% / 3.6% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.1 MiB | 30.4 MiB | 1.010 | 4.8% / 4.0% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.4 MiB | 14.8 MiB | 1.025 | 1.4% / 1.6% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.3 MiB | 1.002 | 1.2% / 1.6% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.2 MiB | 1.008 | 0.8% / 1.1% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.1 MiB | 12.2 MiB | 1.009 | 3.5% / 2.7% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.2 MiB | 1.000 | 1.7% / 2.7% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.1 MiB | 12.4 MiB | 1.021 | 3.6% / 3.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.4 MiB | 1.014 | 1.5% / 1.7% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.4 MiB | 1.011 | 1.6% / 2.5% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.2 MiB | 15.5 MiB | 1.022 | 2.0% / 1.7% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.4 MiB | 12.7 MiB | 1.021 | 3.0% / 2.7% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.6 MiB | 1.022 | 1.5% / 0.7% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.5 MiB | 1.016 | 1.0% / 0.6% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.4 MiB | 1.020 | 1.9% / 1.2% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.5 MiB | 1.017 | 1.5% / 0.8% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 20.0 MiB | 1.015 | 4.3% / 4.7% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.9 MiB | 1.007 | 1.9% / 3.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 0.996 | 3.2% / 2.7% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.8 MiB | 1.007 | 2.4% / 3.6% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.4 MiB | 21.4 MiB | 1.002 | 1.5% / 2.5% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 12.9 MiB | 13.1 MiB | 1.021 | 3.4% / 1.2% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.4 MiB | 0.979 | 1.3% / 3.7% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.3 MiB | 15.9 MiB | 1.042 | 4.9% / 2.2% | MEM+ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.6 MiB | 13.5 MiB | 0.997 | 1.1% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.0 MiB | 15.1 MiB | 1.007 | 2.0% / 0.9% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.1 MiB | 13.4 MiB | 1.018 | 3.7% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.6 MiB | 1.008 | 2.6% / 2.0% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | (not in A) | 13.1 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | (not in A) | 13.1 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | (not in A) | 13.0 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | (not in A) | 13.3 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | (not in A) | 12.6 MiB | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | (not in A) | 12.7 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | (not in A) | 12.1 MiB | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | (not in A) | 12.5 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | (not in A) | 12.3 MiB | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | (not in A) | 12.3 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | (not in A) | 12.6 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | (not in A) | 12.2 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | (not in A) | 12.2 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | (not in A) | 13.7 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | (not in A) | 16.0 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | (not in A) | 13.2 MiB | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | (not in A) | 13.6 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | (not in A) | 12.8 MiB | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | (not in A) | 13.0 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | (not in A) | 12.8 MiB | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | (not in A) | 14.2 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | (not in A) | 20.9 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | (not in A) | 13.3 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | (not in A) | 13.4 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | (not in A) | 59.3 MiB | - | - | new case |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `interplin/dsg/equi/b1/d10/L5/p32` 15.3 MiB -> 15.9 MiB (1.042x); 2 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.2%, 0.1%, 0.1%
Round durations (s): 198, 198, 199, 198, 198 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 991s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
