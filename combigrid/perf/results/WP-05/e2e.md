A/B `e2e`: A=`WP-05-before` B=`WP-05-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 370.16ms | 336.90ms | 1.071 | [0.960, 1.195] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 323.76ms | 294.87ms | 1.071 | [0.959, 1.197] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 36.93ms | 39.40ms | 0.956 | [0.859, 1.064] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 265.41ms | 263.22ms | 1.007 | [0.993, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 263.89ms | 260.78ms | 1.008 | [0.992, 1.025] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.61ms | 1.62ms | 0.857 | [0.516, 1.422] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.25ms | 512.60ms | 0.997 | [0.948, 1.049] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 512.69ms | 511.13ms | 0.996 | [0.948, 1.046] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.45ms | 1.33ms | 1.330 | [0.708, 2.499] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 916.34ms | 914.35ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 911.17ms | 911.90ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 3.36ms | 1.93ms | 1.379 | [0.538, 3.539] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 291.50ms | 295.74ms | 0.985 | [0.960, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 289.11ms | 293.43ms | 0.985 | [0.960, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.55ms | 1.55ms | 1.026 | [0.848, 1.241] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 507.20ms | 502.15ms | 1.011 | [0.994, 1.028] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 498.00ms | 493.05ms | 1.010 | [0.995, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.54ms | 6.54ms | 1.104 | [0.842, 1.448] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 215.10ms | 213.24ms | 1.008 | [0.995, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 203.39ms | 201.59ms | 1.008 | [1.000, 1.016] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.32ms | 8.36ms | 0.994 | [0.736, 1.343] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.405s | 2.405s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.404s | 2.403s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.56ms | 1.45ms | 1.217 | [0.833, 1.777] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.237s | 777.10ms | 1.555 | [1.449, 1.668] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.159s | 707.20ms | 1.612 | [1.500, 1.733] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 64.02ms | 69.16ms | 0.890 | [0.763, 1.039] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 84.46ms | 76.88ms | 1.113 | [1.052, 1.178] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 83.46ms | 75.77ms | 1.121 | [1.060, 1.186] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 789.13us | 1.26ms | 0.673 | [0.572, 0.792] | SLOWER |
| quad/sg/cc-uq/b0/d5/L10 | total | 323.98ms | 319.47ms | 1.014 | [1.001, 1.027] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 322.34ms | 317.60ms | 1.013 | [1.001, 1.025] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.59ms | 1.72ms | 1.116 | [0.621, 2.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 698.55ms | 697.66ms | 1.001 | [0.980, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 696.97ms | 696.14ms | 1.000 | [0.980, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.46ms | 1.40ms | 1.214 | [0.761, 1.937] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.324s | 1.314s | 1.010 | [1.005, 1.016] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.322s | 1.311s | 1.011 | [1.005, 1.016] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.27ms | 2.20ms | 0.954 | [0.561, 1.625] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.24ms | 95.76ms | 0.994 | [0.982, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.52ms | 94.75ms | 0.993 | [0.979, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 744.57us | 880.90us | 1.082 | [0.605, 1.935] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.74ms | 211.68ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 211.13ms | 211.06ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 463.32us | 480.02us | 0.986 | [0.764, 1.272] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.192s | 1.190s | 1.012 | [0.982, 1.044] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.191s | 1.189s | 1.012 | [0.982, 1.044] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 176.21us | 178.79us | 0.990 | [0.911, 1.077] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.432s | 5.439s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.431s | 5.438s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 463.81us | 251.85us | 1.122 | [0.431, 2.921] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.169s | 1.172s | 0.998 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.160s | 1.163s | 0.997 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.01ms | 4.09ms | 1.021 | [0.875, 1.191] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.137s | 1.140s | 1.003 | [0.990, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.118s | 1.121s | 1.002 | [0.991, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 10.88ms | 10.84ms | 1.031 | [0.910, 1.169] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.76ms | 248.20ms | 1.003 | [0.999, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.96ms | 247.45ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 668.24us | 647.71us | 1.046 | [0.973, 1.124] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.450s | 1.448s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.449s | 1.448s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 223.28us | 222.37us | 1.027 | [0.897, 1.176] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.887s | 7.069s | 0.979 | [0.961, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.886s | 7.068s | 0.979 | [0.961, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 296.13us | 253.24us | 1.186 | [0.442, 3.184] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.51ms | 127.78ms | 1.009 | [0.991, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.22ms | 127.49ms | 1.009 | [0.991, 1.028] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 222.16us | 215.04us | 1.032 | [0.945, 1.127] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 187.21ms | 186.19ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 187.06ms | 186.06ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 93.94us | 89.40us | 1.026 | [0.953, 1.105] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.131s | 1.129s | 1.004 | [0.996, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.130s | 1.129s | 1.004 | [0.996, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 76.89us | 68.09us | 1.096 | [0.846, 1.420] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.008s | 1.014s | 0.993 | [0.980, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.008s | 1.014s | 0.993 | [0.980, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.19us | 70.40us | 0.969 | [0.884, 1.061] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.606s | 1.599s | 1.004 | [0.991, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.606s | 1.599s | 1.004 | [0.991, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 51.54us | 52.03us | 0.988 | [0.828, 1.179] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.31ms | 34.13ms | 1.004 | [0.999, 1.009] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.23ms | 34.06ms | 1.004 | [0.999, 1.009] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 34.85us | 34.57us | 1.018 | [0.711, 1.457] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 117.15ms | 116.55ms | 1.008 | [0.995, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 117.06ms | 116.46ms | 1.008 | [0.995, 1.022] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 38.06us | 38.55us | 1.010 | [0.785, 1.300] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.87ms | 11.81ms | 1.004 | [0.992, 1.016] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.79ms | 11.73ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 33.87us | 31.99us | 1.028 | [0.828, 1.278] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.18ms | 51.87ms | 1.002 | [0.992, 1.012] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.80ms | 51.51ms | 1.002 | [0.991, 1.012] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 218.11us | 217.49us | 1.019 | [0.986, 1.053] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.09ms | 43.25ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.94ms | 43.09ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 60.97us | 60.62us | 0.954 | [0.826, 1.103] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 173.98ms | 175.80ms | 0.990 | [0.962, 1.018] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 173.87ms | 175.70ms | 0.990 | [0.961, 1.018] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 49.66us | 50.36us | 0.982 | [0.865, 1.115] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.16ms | 150.98ms | 0.997 | [0.983, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.07ms | 150.89ms | 0.997 | [0.984, 1.011] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 44.49us | 44.56us | 0.989 | [0.966, 1.013] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 397.96ms | 398.47ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 397.90ms | 398.39ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 34.92us | 34.85us | 0.887 | [0.709, 1.110] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.51ms | 6.54ms | 0.886 | [0.610, 1.288] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.45ms | 6.47ms | 0.886 | [0.608, 1.291] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 24.86us | 24.79us | 1.005 | [0.987, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.90ms | 110.38ms | 0.998 | [0.961, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.17ms | 103.62ms | 0.999 | [0.958, 1.041] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.33ms | 2.33ms | 1.015 | [0.934, 1.103] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 363.08ms | 357.45ms | 1.031 | [0.979, 1.085] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 362.53ms | 356.91ms | 1.031 | [0.979, 1.085] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 81.85us | 79.76us | 1.038 | [0.913, 1.180] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.202s | 1.199s | 0.997 | [0.974, 1.020] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.200s | 1.197s | 0.997 | [0.975, 1.019] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.09ms | 2.21ms | 0.975 | [0.438, 2.167] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.866s | 1.850s | 1.006 | [0.986, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.862s | 1.848s | 1.006 | [0.986, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.59ms | 1.53ms | 1.233 | [0.787, 1.932] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 177.28ms | 177.44ms | 1.000 | [0.988, 1.013] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 171.92ms | 172.10ms | 1.000 | [0.988, 1.012] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.25ms | 1.26ms | 0.979 | [0.938, 1.023] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 181.54ms | 182.52ms | 0.987 | [0.958, 1.016] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 181.00ms | 181.97ms | 0.987 | [0.958, 1.016] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 76.62us | 82.55us | 0.899 | [0.821, 0.983] | SLOWER |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 371.26ms | 380.21ms | 0.941 | [0.767, 1.154] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 368.84ms | 376.23ms | 0.941 | [0.766, 1.157] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.17ms | 2.23ms | 0.872 | [0.594, 1.280] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 430.38ms | 431.92ms | 0.968 | [0.879, 1.065] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 428.85ms | 430.42ms | 0.968 | [0.880, 1.066] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.44ms | 1.47ms | 0.920 | [0.752, 1.125] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 784.69ms | 771.15ms | 1.028 | [0.974, 1.084] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 781.50ms | 766.88ms | 1.028 | [0.974, 1.085] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.69ms | 0.934 | [0.739, 1.180] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 488.23ms | 483.21ms | 0.984 | [0.916, 1.057] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 480.51ms | 475.51ms | 0.984 | [0.915, 1.058] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.14ms | 7.16ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.79ms | 26.47ms | 1.024 | [0.966, 1.086] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.25ms | 23.89ms | 1.028 | [0.964, 1.097] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.37ms | 2.39ms | 0.986 | [0.957, 1.016] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.93ms | 2.90ms | 1.011 | [0.989, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.03ms | 1.98ms | 1.019 | [0.941, 1.103] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 755.82us | 761.47us | 0.998 | [0.978, 1.018] | ~ (noise) |

Sum of medians (total, common cases): A = 38.028s, B = 37.660s, saved = 367.98ms (1.010x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 3.53e-13 -> B 9.86e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 341.7 MiB | 110.0 MiB | 0.322 | 8.3% / 21.6% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 12.7 MiB | 12.6 MiB | 0.993 | 1.0% / 3.6% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.7 MiB | 12.6 MiB | 0.990 | 2.6% / 2.5% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.6 MiB | 12.7 MiB | 1.003 | 4.6% / 1.7% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.2 MiB | 13.1 MiB | 0.998 | 1.8% / 0.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.4 MiB | 18.2 MiB | 0.991 | 1.6% / 1.2% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.1 MiB | 20.1 MiB | 0.999 | 1.0% / 0.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.8 MiB | 12.7 MiB | 0.993 | 3.2% / 2.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 862.2 MiB | 310.0 MiB | 0.359 | 2.4% / 21.2% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 33.8 MiB | 17.2 MiB | 0.508 | 9.8% / 5.8% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 14.2 MiB | 12.9 MiB | 0.908 | 4.9% / 3.5% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 12.8 MiB | 12.9 MiB | 1.003 | 1.4% / 2.8% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 0.999 | 2.1% / 2.8% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.6 MiB | 44.1 MiB | 1.012 | 2.2% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 17.9 MiB | 17.8 MiB | 0.997 | 2.0% / 4.3% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.7 MiB | 29.3 MiB | 1.020 | 4.1% / 9.8% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 30.7 MiB | 29.8 MiB | 0.972 | 4.5% / 4.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.5 MiB | 51.6 MiB | 1.000 | 0.3% / 0.3% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.8 MiB | 82.7 MiB | 0.999 | 0.2% / 0.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.4 MiB | 18.0 MiB | 0.979 | 3.9% / 2.5% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.0 MiB | 29.3 MiB | 1.011 | 3.6% / 6.0% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.0 MiB | 29.8 MiB | 0.992 | 4.2% / 3.2% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.4 MiB | 14.5 MiB | 1.005 | 1.6% / 3.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.2 MiB | 0.998 | 3.2% / 1.4% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.2 MiB | 1.003 | 1.4% / 0.7% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.1 MiB | 12.2 MiB | 1.002 | 0.6% / 1.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.1 MiB | 0.997 | 1.4% / 1.9% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.3 MiB | 0.994 | 1.9% / 4.1% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.3 MiB | 1.007 | 1.9% / 2.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 2.6% / 2.6% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.3 MiB | 15.5 MiB | 1.010 | 3.8% / 2.4% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.5 MiB | 12.6 MiB | 1.004 | 1.6% / 1.9% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.4 MiB | 1.003 | 1.5% / 0.9% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.4 MiB | 0.996 | 0.7% / 2.0% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 0.994 | 0.8% / 0.9% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.3 MiB | 0.999 | 0.8% / 1.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.9 MiB | 19.8 MiB | 0.993 | 3.6% / 4.8% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.8 MiB | 1.002 | 0.4% / 3.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 0.994 | 1.1% / 1.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.7 MiB | 1.002 | 1.4% / 1.8% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.1 MiB | 21.1 MiB | 0.997 | 2.8% / 1.6% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.0 MiB | 12.9 MiB | 0.995 | 1.2% / 4.0% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.9 MiB | 1.005 | 3.5% / 2.0% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.0 MiB | 15.2 MiB | 1.008 | 1.8% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 14.6 MiB | 13.4 MiB | 0.923 | 5.4% / 1.6% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.0 MiB | 15.0 MiB | 1.001 | 1.8% / 1.3% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.1 MiB | 0.995 | 2.3% / 0.3% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.5 MiB | 1.003 | 1.9% / 1.4% | ~ |

Peak memory (common cases): geomean B/A = 0.938; largest B/A: `quad/dsg/equi-exp/b0/d7/L10` 28.7 MiB -> 29.3 MiB (1.020x); 0 MEM+, 5 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 162, 163, 162, 163, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 811s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
