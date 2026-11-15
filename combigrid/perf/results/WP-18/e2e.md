A/B `e2e`: A=`WP-18-before` B=`WP-18-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 464.54ms | 375.26ms | 1.227 | [1.099, 1.369] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 391.23ms | 330.07ms | 1.179 | [1.055, 1.317] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.48ms | 36.51ms | 0.839 | [0.754, 0.934] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | total | 265.83ms | 269.95ms | 0.979 | [0.933, 1.027] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 263.22ms | 266.52ms | 0.979 | [0.936, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 3.01ms | 0.827 | [0.484, 1.413] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.77ms | 515.44ms | 1.006 | [0.979, 1.033] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 512.34ms | 513.68ms | 1.005 | [0.979, 1.032] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.80ms | 1.53ms | 1.006 | [0.632, 1.601] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 920.87ms | 918.46ms | 1.000 | [0.989, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 914.91ms | 913.21ms | 0.999 | [0.988, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 4.02ms | 2.18ms | 1.355 | [0.905, 2.028] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 300.39ms | 292.60ms | 1.020 | [0.978, 1.064] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 292.95ms | 290.22ms | 0.999 | [0.957, 1.044] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.25ms | 1.55ms | 3.345 | [2.766, 4.045] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 523.41ms | 517.10ms | 1.016 | [0.991, 1.042] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.61ms | 507.64ms | 0.985 | [0.956, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.34ms | 6.67ms | 2.326 | [2.018, 2.681] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 230.58ms | 215.43ms | 1.048 | [0.979, 1.122] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 203.03ms | 203.42ms | 0.979 | [0.932, 1.028] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.52ms | 8.61ms | 1.959 | [1.287, 2.981] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.414s | 2.413s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.411s | 2.412s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.85ms | 1.58ms | 0.919 | [0.552, 1.532] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.234s | 1.138s | 1.096 | [1.013, 1.185] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.147s | 1.078s | 1.084 | [1.005, 1.170] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.04ms | 62.28ms | 1.341 | [1.113, 1.614] | faster |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.79ms | 84.66ms | 1.034 | [1.019, 1.050] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.36ms | 83.80ms | 1.009 | [0.997, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 788.29us | 2.913 | [1.884, 4.505] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.41ms | 323.25ms | 0.991 | [0.944, 1.040] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 322.66ms | 321.54ms | 0.988 | [0.941, 1.037] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.11ms | 1.65ms | 1.297 | [1.119, 1.505] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 697.29ms | 704.16ms | 0.991 | [0.985, 0.998] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 694.62ms | 701.44ms | 0.990 | [0.985, 0.995] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.97ms | 1.49ms | 1.286 | [0.582, 2.838] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.322s | 1.331s | 0.996 | [0.985, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.320s | 1.329s | 0.996 | [0.985, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.26ms | 2.15ms | 1.182 | [0.853, 1.640] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 97.30ms | 96.47ms | 1.003 | [0.967, 1.040] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.94ms | 95.51ms | 0.998 | [0.963, 1.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 947.46us | 1.181 | [0.965, 1.445] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.99ms | 212.86ms | 0.993 | [0.961, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.70ms | 211.49ms | 0.980 | [0.960, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.19ms | 509.77us | 2.052 | [0.511, 8.245] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.177s | 1.196s | 0.985 | [0.980, 0.990] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.168s | 1.195s | 0.979 | [0.973, 0.985] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.17ms | 177.40us | 17.807 | [17.518, 18.100] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.390s | 5.471s | 0.986 | [0.982, 0.989] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.382s | 5.470s | 0.984 | [0.981, 0.988] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.18ms | 469.75us | 8.809 | [5.360, 14.476] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.233s | 1.172s | 1.051 | [1.041, 1.060] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.155s | 1.163s | 0.995 | [0.987, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 44.62ms | 4.45ms | 10.143 | [8.690, 11.839] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.219s | 1.146s | 1.067 | [1.057, 1.078] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.118s | 1.126s | 0.994 | [0.987, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 61.00ms | 11.51ms | 5.401 | [4.891, 5.965] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.96ms | 248.32ms | 1.004 | [0.994, 1.014] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.34ms | 247.54ms | 0.992 | [0.981, 1.002] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.35ms | 650.64us | 3.580 | [3.297, 3.888] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.453s | 1.456s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.443s | 1.455s | 0.992 | [0.984, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.14ms | 224.40us | 18.942 | [14.583, 24.604] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.894s | 6.929s | 0.994 | [0.988, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.884s | 6.928s | 0.993 | [0.989, 0.997] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.92ms | 271.12us | 13.327 | [8.719, 20.371] | faster |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.91ms | 129.20ms | 0.910 | [0.708, 1.170] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.66ms | 128.92ms | 0.910 | [0.708, 1.171] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.99us | 220.56us | 0.945 | [0.866, 1.033] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 192.60ms | 188.06ms | 1.015 | [0.896, 1.150] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 192.45ms | 187.92ms | 1.015 | [0.896, 1.150] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 77.31us | 92.82us | 0.797 | [0.686, 0.925] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.147s | 1.157s | 0.988 | [0.967, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.146s | 1.157s | 0.988 | [0.967, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 141.29us | 71.17us | 1.876 | [1.496, 2.353] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.022s | 1.018s | 1.009 | [0.985, 1.035] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.021s | 1.018s | 1.009 | [0.983, 1.035] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 139.19us | 70.19us | 3.188 | [0.527, 19.278] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.626s | 1.623s | 0.993 | [0.944, 1.044] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.626s | 1.623s | 0.993 | [0.944, 1.044] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 59.16us | 53.85us | 0.995 | [0.850, 1.166] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 35.15ms | 34.78ms | 1.080 | [0.883, 1.322] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 35.07ms | 34.70ms | 1.080 | [0.883, 1.322] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 34.85us | 36.67us | 0.890 | [0.622, 1.273] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 118.34ms | 117.31ms | 1.116 | [0.834, 1.493] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 118.22ms | 117.23ms | 1.115 | [0.833, 1.493] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 42.11us | 36.88us | 1.257 | [0.901, 1.754] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 13.06ms | 11.96ms | 1.160 | [0.866, 1.555] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 13.00ms | 11.89ms | 1.162 | [0.865, 1.559] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.37us | 33.31us | 0.662 | [0.568, 0.770] | SLOWER |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.47ms | 51.98ms | 1.007 | [1.003, 1.011] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.18ms | 51.62ms | 1.008 | [1.004, 1.012] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 135.35us | 218.11us | 0.617 | [0.608, 0.625] | SLOWER |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.46ms | 53.25ms | 0.976 | [0.465, 2.045] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 43.23ms | 53.11ms | 0.974 | [0.464, 2.047] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 66.42us | 61.04us | 1.072 | [1.051, 1.093] | faster |
| interp/dsg/cc/b0/d5/L6/p64 | total | 172.32ms | 174.56ms | 1.043 | [0.868, 1.254] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 172.07ms | 174.46ms | 1.042 | [0.867, 1.253] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 76.55us | 47.63us | 1.507 | [1.271, 1.787] | faster |
| interp/dsg/cc/b0/d7/L4/p16 | total | 150.20ms | 153.61ms | 0.996 | [0.927, 1.069] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 150.03ms | 153.53ms | 0.995 | [0.927, 1.069] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 58.67us | 45.89us | 1.369 | [1.118, 1.676] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | total | 394.03ms | 404.08ms | 0.980 | [0.941, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 393.94ms | 404.01ms | 0.980 | [0.941, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 36.18us | 36.60us | 0.971 | [0.920, 1.026] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.51ms | 6.54ms | 1.135 | [0.789, 1.633] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.44ms | 6.47ms | 1.134 | [0.786, 1.636] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 22.42us | 25.21us | 0.935 | [0.736, 1.188] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.06ms | 110.16ms | 1.018 | [0.964, 1.075] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 106.89ms | 103.38ms | 1.024 | [0.966, 1.086] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.63ms | 2.30ms | 0.714 | [0.657, 0.776] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 361.28ms | 360.71ms | 1.029 | [0.858, 1.234] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 358.35ms | 360.17ms | 1.022 | [0.852, 1.227] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.19ms | 89.68us | 24.911 | [21.913, 28.320] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.265s | 1.260s | 1.023 | [0.935, 1.118] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.263s | 1.258s | 1.022 | [0.935, 1.118] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.25ms | 2.17ms | 1.045 | [0.690, 1.581] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.931s | 1.952s | 0.984 | [0.936, 1.035] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.929s | 1.950s | 0.984 | [0.936, 1.034] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.72ms | 1.54ms | 0.954 | [0.569, 1.601] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 184.95ms | 178.80ms | 1.026 | [1.003, 1.050] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 180.04ms | 173.37ms | 1.028 | [1.004, 1.052] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.24ms | 1.25ms | 1.008 | [0.956, 1.062] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 189.81ms | 182.20ms | 1.025 | [0.842, 1.249] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 185.62ms | 181.65ms | 1.008 | [0.825, 1.232] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 3.51ms | 83.25us | 36.356 | [25.374, 52.090] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 377.10ms | 377.06ms | 1.024 | [0.950, 1.104] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 373.49ms | 374.92ms | 1.022 | [0.947, 1.103] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.29ms | 2.19ms | 1.194 | [0.915, 1.560] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 451.90ms | 450.94ms | 0.997 | [0.899, 1.104] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 449.58ms | 448.00ms | 0.997 | [0.899, 1.105] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.66ms | 2.83ms | 0.791 | [0.514, 1.219] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 806.38ms | 803.02ms | 1.016 | [0.963, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 800.25ms | 799.82ms | 1.014 | [0.961, 1.070] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.07ms | 2.68ms | 1.372 | [0.977, 1.928] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 481.55ms | 485.06ms | 0.977 | [0.914, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 472.97ms | 477.32ms | 0.975 | [0.910, 1.045] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.54ms | 7.16ms | 1.050 | [1.029, 1.072] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.57ms | 25.94ms | 1.020 | [0.960, 1.083] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.87ms | 23.38ms | 1.017 | [0.950, 1.088] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.39ms | 1.013 | [0.994, 1.032] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.55ms | 3.03ms | 1.440 | [0.884, 2.344] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.12ms | 1.005 | [0.928, 1.088] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.22ms | 755.12us | 2.133 | [0.831, 5.475] | ~ (noise) |

Sum of medians (total, common cases): A = 38.519s, B = 38.338s, saved = 181.84ms (1.005x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 3.68e-12 -> B 8.08e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.1 MiB | 346.3 MiB | 0.558 | 0.0% / 4.5% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 15.9 MiB | 12.6 MiB | 0.797 | 0.8% / 2.6% | MEM- |
| quad/sg/equi-exp/b0/d7/L7 | 14.8 MiB | 12.7 MiB | 0.859 | 1.8% / 4.1% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 13.0 MiB | 12.8 MiB | 0.983 | 2.0% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.2 MiB | 13.3 MiB | 0.509 | 1.8% / 1.9% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 54.0 MiB | 18.4 MiB | 0.340 | 8.1% / 1.3% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 58.6 MiB | 20.2 MiB | 0.345 | 4.2% / 1.7% | MEM- |
| quad/sg/equi-sin/b0/d7/L7 | 14.8 MiB | 12.7 MiB | 0.863 | 3.2% / 1.1% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 1066.0 MiB | 872.6 MiB | 0.819 | 8.5% / 6.6% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 42.7 MiB | 34.8 MiB | 0.817 | 7.8% / 7.8% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 17.8 MiB | 14.2 MiB | 0.797 | 3.0% / 3.6% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 15.3 MiB | 12.9 MiB | 0.841 | 1.5% / 6.7% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 12.7 MiB | 0.973 | 3.5% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.6 MiB | 43.7 MiB | 0.698 | 3.5% / 1.7% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 26.7 MiB | 18.1 MiB | 0.678 | 6.1% / 1.7% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 43.5 MiB | 29.0 MiB | 0.665 | 7.2% / 4.6% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 41.9 MiB | 30.1 MiB | 0.717 | 5.4% / 4.1% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 153.3 MiB | 51.7 MiB | 0.337 | 4.1% / 0.4% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 217.7 MiB | 82.9 MiB | 0.381 | 0.7% / 0.1% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 26.1 MiB | 18.3 MiB | 0.703 | 3.6% / 2.5% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 43.3 MiB | 28.8 MiB | 0.666 | 5.2% / 4.4% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 42.7 MiB | 29.7 MiB | 0.694 | 2.0% / 6.6% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 16.0 MiB | 14.5 MiB | 0.907 | 2.3% / 1.8% | MEM- |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.4 MiB | 1.004 | 3.0% / 2.1% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 3.1% / 1.1% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.3 MiB | 12.3 MiB | 1.003 | 3.1% / 1.2% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 0.998 | 1.6% / 1.2% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.4 MiB | 1.005 | 1.8% / 3.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.3 MiB | 12.2 MiB | 0.991 | 1.7% / 2.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.3 MiB | 0.988 | 3.3% / 1.7% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.4 MiB | 15.4 MiB | 0.936 | 2.7% / 1.7% | MEM- |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.7 MiB | 1.006 | 1.2% / 1.6% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.6 MiB | 12.6 MiB | 0.994 | 1.7% / 1.8% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.7 MiB | 12.6 MiB | 0.995 | 2.0% / 1.1% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.4 MiB | 12.4 MiB | 0.997 | 1.4% / 0.8% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.4 MiB | 1.013 | 1.7% / 1.4% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.9 MiB | 20.1 MiB | 1.011 | 1.8% / 6.5% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.5 MiB | 12.7 MiB | 0.944 | 3.4% / 3.0% | MEM- |
| interplin/sg/equi/b1/d8/L5/p256 | 13.1 MiB | 12.8 MiB | 0.975 | 3.6% / 2.3% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.9 MiB | 12.8 MiB | 0.926 | 2.3% / 2.6% | MEM- |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.8 MiB | 21.7 MiB | 1.042 | 1.1% / 2.8% | MEM+ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.1 MiB | 0.984 | 2.8% / 4.1% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.8 MiB | 0.984 | 3.6% / 3.2% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 17.3 MiB | 15.6 MiB | 0.903 | 3.8% / 1.2% | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.6 MiB | 14.4 MiB | 0.872 | 2.0% / 2.9% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.5 MiB | 15.1 MiB | 0.863 | 5.6% / 1.5% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.2 MiB | 1.006 | 3.1% / 2.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.6 MiB | 1.011 | 2.8% / 1.0% | ~ |

Peak memory (common cases): geomean B/A = 0.810; largest B/A: `interplin/dsg/equi/b1/d2/L16/p16384` 20.8 MiB -> 21.7 MiB (1.042x); 1 MEM+, 27 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.7 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.6%, 0.6%, 0.6%, 0.8%
Round durations (s): 166, 166, 165, 165, 165 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 826s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
