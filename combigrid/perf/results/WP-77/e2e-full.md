A/B `e2e`: A=`WP-77-before` B=`WP-77-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 313.51ms | 280.12ms | 1.114 | [0.999, 1.244] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 273.38ms | 241.39ms | 1.130 | [1.011, 1.262] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.63ms | 38.17ms | 1.016 | [0.912, 1.131] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.90ms | 246.77ms | 1.005 | [0.994, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 246.43ms | 244.94ms | 1.007 | [0.995, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.40ms | 1.58ms | 0.878 | [0.770, 1.002] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.91ms | 495.59ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 495.38ms | 493.76ms | 1.001 | [0.990, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.41ms | 1.69ms | 0.664 | [0.418, 1.057] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 905.70ms | 897.52ms | 1.010 | [1.004, 1.016] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 902.11ms | 895.28ms | 1.009 | [1.002, 1.015] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.93ms | 2.15ms | 1.220 | [0.534, 2.790] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.67ms | 241.74ms | 1.000 | [0.995, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.22ms | 239.30ms | 0.999 | [0.991, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.43ms | 1.38ms | 1.130 | [0.675, 1.891] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.16ms | 490.32ms | 1.002 | [0.989, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 482.17ms | 481.50ms | 1.002 | [0.988, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.37ms | 6.05ms | 1.059 | [0.945, 1.186] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 216.24ms | 213.14ms | 1.015 | [0.991, 1.039] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 204.77ms | 201.98ms | 1.014 | [0.989, 1.040] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.17ms | 7.90ms | 1.034 | [0.765, 1.397] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.396s | 2.385s | 1.006 | [0.998, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.395s | 2.383s | 1.006 | [0.998, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.47ms | 1.51ms | 0.929 | [0.823, 1.049] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 441.91ms | 439.80ms | 1.012 | [0.972, 1.053] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 365.53ms | 370.70ms | 1.010 | [0.938, 1.087] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.13ms | 71.68ms | 0.966 | [0.836, 1.116] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.23ms | 71.88ms | 1.047 | [1.032, 1.062] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 73.86ms | 70.52ms | 1.048 | [1.035, 1.061] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.29ms | 1.27ms | 1.020 | [0.956, 1.088] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.99ms | 320.70ms | 1.002 | [0.997, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.33ms | 317.54ms | 1.005 | [1.001, 1.009] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.65ms | 3.22ms | 0.641 | [0.392, 1.046] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 690.93ms | 684.26ms | 1.009 | [1.004, 1.014] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 689.31ms | 682.67ms | 1.009 | [1.004, 1.014] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.43ms | 1.47ms | 0.938 | [0.812, 1.083] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.308s | 1.297s | 1.005 | [0.995, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.306s | 1.295s | 1.005 | [0.995, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.22ms | 2.13ms | 1.039 | [0.765, 1.410] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.80ms | 84.70ms | 1.107 | [1.096, 1.118] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.53ms | 81.19ms | 1.115 | [1.105, 1.126] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 884.74us | 1.04ms | 0.700 | [0.397, 1.232] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.65ms | 196.90ms | 1.027 | [1.019, 1.035] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 201.83ms | 195.96ms | 1.027 | [1.019, 1.036] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 489.23us | 557.05us | 0.876 | [0.679, 1.131] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.149s | 1.135s | 1.011 | [1.007, 1.015] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.148s | 1.134s | 1.012 | [1.008, 1.016] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 182.15us | 796.88us | 0.281 | [0.162, 0.486] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.410s | 5.371s | 1.009 | [1.005, 1.012] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.409s | 5.369s | 1.009 | [1.005, 1.012] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 341.31us | 339.57us | 0.814 | [0.552, 1.198] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.164s | 1.166s | 0.997 | [0.993, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.150s | 1.150s | 0.999 | [0.994, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.05ms | 6.00ms | 0.677 | [0.580, 0.790] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.137s | 1.141s | 0.995 | [0.989, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.110s | 1.110s | 0.997 | [0.992, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 10.94ms | 13.69ms | 0.799 | [0.776, 0.824] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.36ms | 239.91ms | 1.029 | [1.022, 1.035] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.38ms | 238.82ms | 1.029 | [1.022, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 644.91us | 733.47us | 0.883 | [0.821, 0.949] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.452s | 1.435s | 1.012 | [1.009, 1.016] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.451s | 1.433s | 1.013 | [1.009, 1.016] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 219.51us | 554.96us | 0.444 | [0.277, 0.712] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.866s | 6.827s | 1.005 | [1.000, 1.011] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.864s | 6.826s | 1.005 | [1.001, 1.010] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 527.79us | 333.63us | 1.232 | [0.803, 1.892] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.41ms | 83.92ms | 1.485 | [1.366, 1.614] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.15ms | 83.60ms | 1.486 | [1.368, 1.615] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 231.24us | 220.84us | 1.029 | [0.943, 1.124] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 186.30ms | 167.43ms | 1.096 | [1.054, 1.140] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.15ms | 167.29ms | 1.096 | [1.054, 1.140] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 90.51us | 91.98us | 1.009 | [0.937, 1.087] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.136s | 1.136s | 1.003 | [0.990, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.136s | 1.136s | 1.003 | [0.990, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 66.70us | 71.17us | 0.946 | [0.755, 1.187] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.009s | 987.21ms | 1.019 | [1.006, 1.032] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.009s | 987.09ms | 1.019 | [1.006, 1.032] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 63.42us | 66.21us | 0.916 | [0.809, 1.037] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.614s | 1.619s | 0.995 | [0.989, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.614s | 1.619s | 0.995 | [0.989, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 50.98us | 51.05us | 1.004 | [0.943, 1.069] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.12ms | 22.76ms | 1.504 | [1.477, 1.531] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.04ms | 22.69ms | 1.505 | [1.478, 1.532] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 42.25us | 35.76us | 1.212 | [0.847, 1.735] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 117.23ms | 109.16ms | 1.074 | [1.060, 1.088] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 117.15ms | 109.07ms | 1.074 | [1.060, 1.088] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 30.59us | 40.09us | 0.738 | [0.573, 0.949] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.87ms | 8.21ms | 1.372 | [1.180, 1.595] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.80ms | 8.14ms | 1.375 | [1.182, 1.600] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 31.99us | 27.73us | 1.095 | [0.932, 1.287] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.35ms | 52.43ms | 1.076 | [0.881, 1.314] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.99ms | 52.07ms | 1.076 | [0.880, 1.316] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 219.51us | 218.11us | 1.038 | [0.977, 1.102] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.15ms | 43.03ms | 1.006 | [0.935, 1.083] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 43.00ms | 42.86ms | 1.007 | [0.934, 1.084] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 61.46us | 63.42us | 0.922 | [0.778, 1.092] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 174.78ms | 174.82ms | 0.993 | [0.930, 1.061] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 174.66ms | 174.72ms | 0.993 | [0.930, 1.061] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 50.15us | 53.57us | 0.950 | [0.874, 1.033] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.27ms | 151.48ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.19ms | 151.39ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 41.00us | 45.26us | 0.899 | [0.852, 0.948] | SLOWER |
| interp/dsg/cc/b0/d10/L2/p8 | total | 399.07ms | 398.06ms | 1.004 | [0.998, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 399.01ms | 398.00ms | 1.004 | [0.998, 1.010] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 33.87us | 36.67us | 0.929 | [0.858, 1.006] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.55ms | 6.58ms | 0.977 | [0.926, 1.031] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.48ms | 6.52ms | 0.977 | [0.926, 1.032] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 25.21us | 26.26us | 0.964 | [0.943, 0.986] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 102.51ms | 100.70ms | 1.017 | [0.980, 1.056] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 95.80ms | 93.82ms | 1.017 | [0.976, 1.060] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.33ms | 2.32ms | 1.019 | [0.930, 1.116] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 363.37ms | 363.15ms | 1.004 | [0.979, 1.030] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 362.83ms | 362.60ms | 1.004 | [0.978, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 82.90us | 94.77us | 0.908 | [0.790, 1.043] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.194s | 1.194s | 1.004 | [0.982, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.192s | 1.191s | 1.005 | [0.983, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.01ms | 2.18ms | 0.822 | [0.519, 1.301] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.841s | 1.856s | 0.991 | [0.971, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.839s | 1.854s | 0.991 | [0.971, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.63ms | 1.60ms | 1.172 | [0.785, 1.748] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 156.20ms | 158.37ms | 0.984 | [0.973, 0.996] | SLOWER |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 150.88ms | 152.61ms | 0.985 | [0.973, 0.996] | SLOWER |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.22ms | 1.25ms | 0.970 | [0.864, 1.088] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 122.43ms | 122.40ms | 0.988 | [0.958, 1.018] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 121.83ms | 121.74ms | 0.988 | [0.958, 1.019] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 135.77us | 130.81us | 0.945 | [0.778, 1.147] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 240.76ms | 238.81ms | 1.003 | [0.978, 1.030] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 238.40ms | 235.77ms | 1.002 | [0.975, 1.030] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.11ms | 1.99ms | 1.095 | [0.465, 2.584] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 271.86ms | 271.67ms | 1.003 | [0.998, 1.007] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 270.21ms | 269.27ms | 1.004 | [0.998, 1.009] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.51ms | 1.52ms | 0.889 | [0.491, 1.610] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 693.79ms | 664.48ms | 1.041 | [0.987, 1.098] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 690.58ms | 661.29ms | 1.041 | [0.987, 1.099] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.66ms | 1.000 | [0.988, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 377.46ms | 323.36ms | 1.159 | [1.135, 1.184] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 369.80ms | 314.95ms | 1.167 | [1.140, 1.195] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.09ms | 7.15ms | 0.884 | [0.640, 1.222] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.30ms | 19.69ms | 1.330 | [1.305, 1.355] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.74ms | 17.13ms | 1.379 | [1.311, 1.451] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.39ms | 2.38ms | 1.000 | [0.989, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.06ms | 2.21ms | 1.304 | [1.067, 1.593] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.14ms | 1.22ms | 1.708 | [1.578, 1.850] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 756.59us | 750.16us | 0.854 | [0.550, 1.326] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.536s | 1.532s | 1.002 | [0.994, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.530s | 1.002 | [0.995, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.20ms | 2.34ms | 0.863 | [0.484, 1.539] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 571.62ms | 570.22ms | 1.004 | [0.998, 1.009] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.82ms | 567.91ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.20ms | 2.18ms | 1.400 | [0.759, 2.584] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.036s | 1.036s | 1.000 | [0.986, 1.015] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.034s | 1.034s | 1.001 | [0.987, 1.015] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.04ms | 2.22ms | 0.836 | [0.589, 1.186] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 520.47ms | 521.31ms | 0.999 | [0.975, 1.024] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.52ms | 518.17ms | 1.001 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 1.77ms | 2.01ms | 0.724 | [0.324, 1.614] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.156s | 1.564s | 1.377 | [1.372, 1.382] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.154s | 1.562s | 1.378 | [1.373, 1.383] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.15ms | 1.15ms | 1.004 | [0.983, 1.025] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 880.20ms | 551.72ms | 1.596 | [1.590, 1.602] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 878.89ms | 550.41ms | 1.597 | [1.591, 1.603] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.12ms | 1.13ms | 0.998 | [0.963, 1.033] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.072s | 1.028s | 1.043 | [1.028, 1.057] | faster |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.072s | 1.028s | 1.043 | [1.028, 1.057] | faster |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 60.62us | 81.22us | 0.780 | [0.687, 0.885] | SLOWER |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 503.23ms | 502.86ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 503.16ms | 502.78ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 44.84us | 49.03us | 0.948 | [0.791, 1.135] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 829.25ms | 828.06ms | 1.001 | [0.974, 1.029] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 829.18ms | 827.97ms | 1.001 | [0.974, 1.029] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 38.69us | 49.24us | 0.774 | [0.642, 0.932] | SLOWER |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.21ms | 345.35ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.14ms | 345.28ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 36.74us | 37.57us | 0.972 | [0.867, 1.089] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.475s | 1.846s | 1.341 | [1.336, 1.346] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.474s | 1.845s | 1.341 | [1.336, 1.346] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.17ms | 1.16ms | 1.005 | [0.995, 1.014] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 397.86ms | 331.74ms | 1.199 | [1.191, 1.208] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 397.80ms | 331.68ms | 1.199 | [1.191, 1.208] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 37.57us | 39.39us | 1.008 | [0.832, 1.220] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.290s | 1.289s | 1.004 | [0.972, 1.037] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.290s | 1.289s | 1.004 | [0.972, 1.037] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 50.49us | 52.80us | 0.983 | [0.898, 1.076] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.153s | 0.999 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.150s | 1.151s | 0.999 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.79ms | 1.74ms | 1.031 | [1.004, 1.060] | faster |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 349.98ms | 350.33ms | 0.999 | [0.991, 1.008] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 348.66ms | 348.64ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 347.74us | 385.17us | 0.789 | [0.541, 1.152] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 661.84ms | 495.28ms | 1.346 | [1.314, 1.378] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 658.31ms | 491.75ms | 1.348 | [1.316, 1.381] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.35ms | 3.34ms | 1.004 | [0.983, 1.024] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 333.67ms | 314.97ms | 1.059 | [1.055, 1.063] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 330.08ms | 311.35ms | 1.060 | [1.056, 1.064] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.23ms | 3.30ms | 0.983 | [0.977, 0.990] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 257.69ms | 252.32ms | 1.021 | [0.837, 1.246] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.68ms | 250.46ms | 1.022 | [0.842, 1.240] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 1.91ms | 1.96ms | 0.949 | [0.485, 1.859] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 98.78ms | 99.01ms | 0.994 | [0.979, 1.008] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.69ms | 96.76ms | 0.992 | [0.971, 1.014] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.02ms | 2.00ms | 1.083 | [0.746, 1.572] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.075s | 1.074s | 0.994 | [0.881, 1.122] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.073s | 1.073s | 0.994 | [0.879, 1.123] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 3.05ms | 2.10ms | 1.235 | [0.630, 2.424] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 208.40ms | 207.93ms | 1.000 | [0.990, 1.011] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 205.99ms | 205.83ms | 0.999 | [0.990, 1.008] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.05ms | 2.01ms | 1.131 | [0.378, 3.382] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.98ms | 621.23ms | 0.927 | [0.918, 0.937] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.08ms | 617.57ms | 0.926 | [0.920, 0.932] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 3.37ms | 1.96ms | 1.223 | [0.403, 3.713] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.50ms | 72.10ms | 0.976 | [0.969, 0.983] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.40ms | 71.97ms | 0.976 | [0.969, 0.983] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 44.07us | 47.77us | 0.912 | [0.859, 0.968] | SLOWER |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.47ms | 243.15ms | 1.002 | [0.914, 1.098] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.33ms | 243.04ms | 1.002 | [0.914, 1.098] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 36.25us | 39.60us | 0.907 | [0.781, 1.054] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 152.42ms | 153.00ms | 0.992 | [0.964, 1.021] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 143.45ms | 143.94ms | 0.991 | [0.961, 1.023] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 57.90us | 63.42us | 0.899 | [0.864, 0.934] | SLOWER |

Sum of medians (total, common cases, group suite): A = 36.326s, B = 35.935s, saved = 390.95ms (1.011x)
Sum of medians (total, common cases, group expensive/): A = 17.751s, B = 15.894s, saved = 1.858s (1.117x)
Sum of medians (total, common cases, group expensive/ cached): A = 1.043s, B = 1.089s, saved = -46105968.0ns (0.958x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d2/L20`: A 7.23e-14 -> B 7.23e-14 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 123.6 MiB | 120.5 MiB | 0.974 | 21.8% / 19.2% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.6 MiB | 12.8 MiB | 1.012 | 3.0% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.002 | 1.3% / 2.2% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 13.1 MiB | 1.017 | 1.5% / 0.8% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.3 MiB | 13.3 MiB | 1.001 | 2.8% / 1.4% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.5 MiB | 18.4 MiB | 0.996 | 2.8% / 1.9% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.3 MiB | 1.004 | 0.6% / 0.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.7 MiB | 12.9 MiB | 1.012 | 1.6% / 1.4% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 198.3 MiB | 203.0 MiB | 1.024 | 10.2% / 12.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.4 MiB | 15.1 MiB | 0.980 | 5.8% / 4.6% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 13.0 MiB | 1.013 | 1.2% / 1.0% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.0 MiB | 1.008 | 2.5% / 1.1% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.1 MiB | 1.010 | 2.7% / 2.4% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.8 MiB | 45.2 MiB | 1.032 | 2.2% / 1.7% | MEM+ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.2 MiB | 18.7 MiB | 1.025 | 3.2% / 1.1% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 29.1 MiB | 28.8 MiB | 0.990 | 6.9% / 3.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 30.1 MiB | 27.8 MiB | 0.923 | 4.0% / 5.2% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 51.6 MiB | 54.4 MiB | 1.054 | 0.4% / 0.2% | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.8 MiB | 86.3 MiB | 1.042 | 0.2% / 0.2% | MEM+ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.6 MiB | 18.7 MiB | 1.009 | 1.0% / 2.0% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.9 MiB | 29.1 MiB | 0.973 | 6.7% / 4.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.0 MiB | 28.8 MiB | 0.957 | 1.2% / 3.5% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 14.6 MiB | 14.8 MiB | 1.017 | 2.1% / 3.7% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.4 MiB | 1.015 | 1.8% / 0.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.3 MiB | 1.014 | 3.0% / 1.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.1 MiB | 12.3 MiB | 1.014 | 1.4% / 1.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.3 MiB | 1.010 | 1.8% / 2.1% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.3 MiB | 1.015 | 1.5% / 0.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.3 MiB | 12.4 MiB | 1.010 | 1.2% / 1.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.4 MiB | 1.014 | 5.0% / 2.7% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.5 MiB | 15.6 MiB | 1.010 | 2.8% / 2.2% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.7 MiB | 1.007 | 2.7% / 1.4% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.5 MiB | 1.003 | 1.2% / 1.0% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.5 MiB | 1.005 | 2.1% / 1.0% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 1.005 | 1.6% / 1.3% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 1.002 | 1.1% / 1.7% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 20.2 MiB | 20.4 MiB | 1.011 | 12.2% / 6.2% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.9 MiB | 12.8 MiB | 0.995 | 2.4% / 1.9% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 1.005 | 3.2% / 0.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.6 MiB | 0.998 | 1.7% / 1.5% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.6 MiB | 21.6 MiB | 0.999 | 2.8% / 3.2% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.2 MiB | 1.006 | 2.2% / 1.7% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.5 MiB | 12.5 MiB | 1.004 | 2.4% / 0.7% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.9 MiB | 16.0 MiB | 1.004 | 2.6% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.3 MiB | 13.5 MiB | 1.009 | 1.7% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 15.0 MiB | 1.005 | 2.6% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.3 MiB | 0.997 | 1.6% / 1.0% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.7 MiB | 12.7 MiB | 1.004 | 1.2% / 0.8% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 12.9 MiB | 13.1 MiB | 1.017 | 2.3% / 2.2% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.2 MiB | 13.2 MiB | 1.000 | 0.9% / 2.1% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.0 MiB | 13.1 MiB | 1.006 | 1.3% / 0.9% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.009 | 1.4% / 0.9% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.6 MiB | 12.5 MiB | 0.997 | 1.0% / 1.6% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.7 MiB | 0.997 | 1.0% / 1.3% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.2 MiB | 12.4 MiB | 1.015 | 2.0% / 1.6% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.5 MiB | 12.5 MiB | 1.003 | 0.8% / 1.5% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.3 MiB | 12.2 MiB | 0.998 | 2.2% / 3.1% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.4 MiB | 12.4 MiB | 1.005 | 1.7% / 2.2% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.6 MiB | 12.6 MiB | 1.003 | 1.5% / 1.9% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.2 MiB | 12.4 MiB | 1.016 | 1.3% / 4.8% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.0 MiB | 12.2 MiB | 1.016 | 1.9% / 2.0% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.8 MiB | 13.8 MiB | 1.003 | 1.5% / 1.6% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 15.9 MiB | 16.1 MiB | 1.013 | 1.3% / 1.7% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.1 MiB | 13.1 MiB | 0.996 | 0.4% / 2.7% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.5 MiB | 13.5 MiB | 0.998 | 1.1% / 1.8% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 12.8 MiB | 12.9 MiB | 1.009 | 1.9% / 3.0% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.1 MiB | 13.2 MiB | 1.006 | 2.0% / 0.4% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.8 MiB | 12.8 MiB | 1.000 | 2.6% / 2.0% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 14.1 MiB | 14.3 MiB | 1.014 | 4.9% / 3.4% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.8 MiB | 21.1 MiB | 1.016 | 2.1% / 1.7% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.4 MiB | 13.6 MiB | 1.013 | 4.7% / 3.2% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.4 MiB | 13.4 MiB | 0.994 | 1.8% / 2.7% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.4 MiB | 59.3 MiB | 0.998 | 0.7% / 0.6% | ~ |

Peak memory (common cases): geomean B/A = 1.005; largest B/A: `quad/dsg/equi-exp/b1/d15/L7` 51.6 MiB -> 54.4 MiB (1.054x); 3 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.1%, 0.1%
Round durations (s): 232, 232, 232, 232, 232 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1159s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
